#include "Rasterizer.hpp"

void rst::rasterizer::set_model(const MyMtrix::Matrix &m)
{
        modelMatrix = m;
}

int rst::rasterizer::getindex(int x, int y)
{
        x = std::clamp(x, 0, width - 1);
        y = std::clamp(y, 0, height - 1);
        return x + y * width;
}

void rst::rasterizer::set_texture(Texture *tex)
{
        texture = tex;
}

void rst::rasterizer::set_normal_map(Texture *nmp)
{
        normal_map = nmp;
}

void rst::rasterizer::set_specular_map(Texture *sp)
{
        if (sp != nullptr)
                specular_map = sp;
}

bool rst::rasterizer::set_pixel(Vec2i &point, Vec3f &color)
{
        int ind = point.x + point.y * width;
        if (ind < 0 || ind >= frame_buffer.size())
                return false;

        try
        {
                frame_buffer[ind] = color;
        }
        catch (const std::exception &e)
        {
                std::cerr << e.what() << '\n';
                return false;
        }
        return true;
}

void rst::rasterizer::set_pixel_super_sampling(Vec2i &point, Vec3f &color)
{
        int ind = (point.x + point.y * width);
        int ind_depth = (point.x + point.y * width) * MSAA_sample;
        if (ind < 0 || ind >= frame_buffer.size())
                return;
        if (ind_depth >= depth_buffer_x4.value().size())
                return;
        frame_buffer[ind] = color;
}

void rst::rasterizer::set_vertexShader(std::function<Vec4f(MyShader::VertexShader *, MyMtrix::Matrix modelMatrix)> vert_shader)
{
        vertexShader = vert_shader;
}

void rst::rasterizer::set_fragmentShader(std::function<Vec3f(MyShader::FragmentShader *)> frag_shader)
{
        fragmentShader = frag_shader;
}

void rst::rasterizer::set_depth_buffer(std::vector<float> *db)
{
        depth_buffer_shadow = db;
}

void rst::rasterizer::draw(std::vector<Triangle> &TriangleList)
{

        auto ViewPortTM = Camera->ViewPort(width / 8, height / 8, width * 3 / 4, height * 3 / 4, 255.f);
        this->DeepthCamera = new MyCamera::Camera(lightDir, Camera->Getcenter(), Camera->GetUp(), Camera->Getfovy(), Camera->Getaspect(), Camera->GetNear(), Camera->GetFar(), MyCamera::Orth);
        this->DeepthCamera.value()->SetMshadow(ViewPortTM * DeepthCamera.value()->GetMVMatrix() * ((ViewPortTM * Camera->GetMVMatrix()).invert()));

        std::cout << "DeepthCamera Mshadow: " << this->DeepthCamera.value()->GetMshadow() << std::endl;
        if (depth_buffer_shadow)
        {
                if (!DeepthCamera.has_value())
                {
                        std::cerr << "DeepthCamera is not set" << std::endl;
                        return;
                }
                if (!(DeepthCamera.value()->HasMshadow()))
                {
                        std::cerr << "DeepthCamera Mshadow is not set" << std::endl;
                        return;
                }

                for (auto &tri : TriangleList)
                {
                        Triangle newtri(tri); // copy

                        std::array<Vec3f, 3> pts = tri.to_vec3();
                        std::array<Vec4f, 3> pts_clip;
                        for (int i = 0; i < 3; i++)
                        {
                                MyShader::VertexShader *vs = new MyShader::VertexShader(Vec4f(pts[i].x, pts[i].y, pts[i].z, 1.0f), DeepthCamera.value()->GetMVMatrix(), ViewPortTM);
                                pts_clip[i] = vertexShader(vs, modelMatrix); // NDC
                        }

                        for (int i = 0; i < 3; i++)
                        {
                                newtri.setVert(i, pts_clip[i]);
                        }
                        // parepare shadow buffer
                        rasterizer_preshadow(newtri, depth_buffer_shadow.value());
                }
                // DEBUG
                //  int i = 0;
                //  for(const auto& v: *(depth_buffer_shadow.value()))
                //  {
                //          if(v != -std::numeric_limits<float>::max() && v != -std::numeric_limits<float>::infinity())
                //                  ++i;
                //  }
                //  std::cout << "valied shadow buffer: " << i << std::endl;
        }

        // debug
        // for (int i = 0; i < width; ++i)
        //         for (int j = 0; j < height; ++j)
        //                 if (depth_buffer_shadow.value()->at(getindex(i, j)) != -std::numeric_limits<float>::max())
        //                 {
        //                         auto color = Vec3f(255, 255, 255) * std::clamp(depth_buffer_shadow.value()->at(getindex(i, j)) / 255.f, 0.f, 1.f);
        //                         set_pixel(Vec2i(i, j), color);
        //                 }
        this->clear(rst::Buffers::Color);
        this->clear(rst::Buffers::Depth);

        for (auto &tri : TriangleList)
        {

                Triangle newtri(tri); // copy
                std::array<Vec3f, 3> pts = tri.to_vec3();
                std::array<Vec4f, 3> pts_clip;
                for (int i = 0; i < 3; i++)
                {
                        MyShader::VertexShader *vs = new MyShader::VertexShader(Vec4f(pts[i].x, pts[i].y, pts[i].z, 1.0f), Camera->GetMVMatrix(), ViewPortTM);
                        pts_clip[i] = vertexShader(vs, modelMatrix);
                }

                for (int i = 0; i < 3; i++)
                {
                        newtri.setVert(i, pts_clip[i]);
                }

                rasterizer_triangle(newtri);
        }
}

void rst::rasterizer::rasterizer_triangle(Triangle &t)
{
        // Vec3f pts[3] = {t.a().toVec3(), t.b().toVec3(), t.c().toVec3()};
        Vec3f pts[3] = {
            Vec3f(t.a().x, t.a().y, t.a().z),
            Vec3f(t.b().x, t.b().y, t.b().z),
            Vec3f(t.c().x, t.c().y, t.c().z),
        };
        auto normal_tri = (pts[1] - pts[0]) ^ (pts[2] - pts[0]);
        Scanbbox(t, normal_tri, rst::ForNormalMapping);
}

void rst::rasterizer::rasterizer_preshadow(Triangle &t, std::vector<float> *dbs)
{

        Vec3f pts[3] = {
            Vec3f(t.a().x, t.a().y, t.a().z),
            Vec3f(t.b().x, t.b().y, t.b().z),
            Vec3f(t.c().x, t.c().y, t.c().z),
        };
        auto normal_tri = (pts[1] - pts[0]) ^ (pts[2] - pts[0]);
        Scanbbox(t, normal_tri, rst::ForShadowMapping);
}

void rst::rasterizer::clear(Buffers buf)
{
        if (buf == Buffers::Color)
        {
                std::fill(frame_buffer.begin(), frame_buffer.end(), Vec3f(0.0f, 0.0f, 0.0f));
        }
        else if (buf == Buffers::Depth)
        {
                std::fill(depth_buffer.begin(), depth_buffer.end(), -std::numeric_limits<float>::infinity());
                if (MSAA)
                {
                        std::fill(depth_buffer_x4.value().begin(), depth_buffer_x4.value().end(), -std::numeric_limits<float>::infinity());
                }
        }
}

void rst::rasterizer::Scanbbox(Triangle &t, Vec3f &pts_norm, RenderMode mode)
{
        // 0 for shadow mapping
        Vec3f pts[3] = {
            Vec3f(t.a().x, t.a().y, t.a().z),
            Vec3f(t.b().x, t.b().y, t.b().z),
            Vec3f(t.c().x, t.c().y, t.c().z),
        };
        Vec2i bboxMin(width - 1, height - 1);
        Vec2i bboxMax(0, 0);

        bboxMin.x = std::min({bboxMin.x, static_cast<int>(pts[0].x), static_cast<int>(pts[1].x), static_cast<int>(pts[2].x)});
        bboxMin.y = std::min({bboxMin.y, static_cast<int>(pts[0].y), static_cast<int>(pts[1].y), static_cast<int>(pts[2].y)});
        bboxMax.x = std::max({bboxMax.x, static_cast<int>(pts[0].x), static_cast<int>(pts[1].x), static_cast<int>(pts[2].x)});
        bboxMax.y = std::max({bboxMax.y, static_cast<int>(pts[0].y), static_cast<int>(pts[1].y), static_cast<int>(pts[2].y)});

        // clipping
        if (bboxMin.x < 0 || bboxMin.y < 0 || bboxMax.x > width - 1 || bboxMax.y > height - 1)
        {
                return;
        }
        for (int x = bboxMin.x; x <= bboxMax.x; ++x)
        {
                for (int y = bboxMin.y; y <= bboxMax.y; ++y)
                {
                        Vec3f P(x, y, 0);
                        if (isInTriangle(pts, P))
                        {
                                Vec3f coefficient = barycentric(pts, P);
                                P.z = coefficient.x * pts[0].z + coefficient.y * pts[1].z + coefficient.z * pts[2].z; // z_interpolation < 0

                                if (P.z > depth_buffer[getindex(x, y)]) // early z test
                                {
                                        if (mode == rst::ForShadowMapping)
                                        {
                                                if (!depth_buffer_shadow.has_value())
                                                {
                                                        std::cerr << "depth_buffer_shadow is not set" << std::endl;
                                                        return;
                                                }
                                                depth_buffer[getindex(x, y)] = P.z; // update
                                                // uniform_Mshadow
                                                std::function<Vec3f(MyShader::FragmentShader *)> FS_method = MyShader::depth_shading; // 片元着色器
                                                // transform screen coordinates of current fragment into screen coordinates inside the shadowbuffer
                                                // P.z = P.z / (coefficient.x + coefficient.y  + coefficient.z );
                                                // std::cout << "coff: " <<(coefficient.x + coefficient.y  + coefficient.z ) << std::endl;
                                                // std::cout << "P.z: " << P.z << std::endl;
                                                MyShader::DeepFragmentShader *Deepfs = new MyShader::DeepFragmentShader(P, Vec3f(255, 255, 255), pts_norm, pts_norm, Vec2f(-1, -1), nullptr, nullptr, lightDir, depth_buffer_shadow.value(), DeepthCamera.value()->GetMshadow());
                                                Deepfs->width = this->width;
                                                Deepfs->height = this->height;
                                                auto pixel_color = FS_method(Deepfs);
                                                // set_pixel(Vec2i(x, y), pixel_color);
                                        }
                                        else if (mode == rst::ForNormalMapping)
                                        {
                                                depth_buffer[getindex(x, y)] = P.z; // update

                                                auto interpolated_color = t.getColor(0) * coefficient.x + t.getColor(1) * coefficient.y + t.getColor(2) * coefficient.z;
                                                auto interpolated_normal = coefficient.x * t.getNormal(0) + coefficient.y * t.getNormal(1) + coefficient.z * t.getNormal(2);
                                                auto interpolated_texcoords = coefficient.x * t.getUV(0) + coefficient.y * t.getUV(1) + coefficient.z * t.getUV(2);

                                                Vec3f color = Vec3f(interpolated_color.r, interpolated_color.g, interpolated_color.b);

                                                // Check the constructor of MyShader::FragmentShader and ensure it accepts the provided arguments.
                                                // MyShader::FragmentShader fs(P, color, interpolated_normal, pts_norm, interpolated_texcoords, texture ? texture.value() : nullptr, normal_map ? normal_map.value() : nullptr, this->lightDir);
                                                MyShader::DeepFragmentShader *fs = new MyShader::DeepFragmentShader(P, color, interpolated_normal, pts_norm, interpolated_texcoords, texture.has_value() ? texture.value() : nullptr,
                                                                                                                    normal_map.has_value() ? normal_map.value() : nullptr,
                                                                                                                    lightDir, depth_buffer_shadow.has_value() ? depth_buffer_shadow.value() : nullptr,
                                                                                                                    DeepthCamera.value()->GetMshadow());
                                                fs->width = this->width;
                                                fs->height = this->height;
                                                fs->set_M(Camera->GetMVMatrix());
                                                fs->set_MIT(Camera->GetMITMatrix());
                                                if (specular_map.has_value()) // specular map
                                                        fs->set_sp_map(specular_map.value());

                                                auto pixel_color = fragmentShader(fs);

                                                if (!set_pixel(Vec2i(x, y), pixel_color))
                                                {
                                                        std::cout << "set pixel failed" << std::endl;
                                                }
                                        }
                                }
                        }
                }
        }
}