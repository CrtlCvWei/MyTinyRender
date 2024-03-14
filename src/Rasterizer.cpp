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

int rst::rasterizer::getindex_msaa(int x, int y)
{
        return getindex(x, y) * MSAA_sample;
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
        if (ind_depth >= depth_buffer_msaa.value().size())
                return;
        frame_buffer[ind] = color;
}

Vec3f rst::rasterizer::getColor_msaa(std::vector<Vec3f> &color, const int head_idx)
{
        Vec3f result = Vec3f(0, 0, 0);
        int ignore = 0;
        for (int i = 0; i < MSAA_sample; ++i)
        {
                if (depth_buffer_msaa.value()[head_idx + i] == -std::numeric_limits<float>::max() || color[head_idx + i] == Vec3f(0,0,0))
                        ignore++;
                else
                        result = result + color[head_idx + i];
        }
        int avg = MSAA_sample - ignore;
        if (avg == 0)
                return Vec3f(0, 0, 0);
        return result / avg;
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
                // int i = 0;
                // for (const auto &v : *(depth_buffer_shadow.value()))
                // {
                //         if (v != -std::numeric_limits<float>::max() && v != -std::numeric_limits<float>::infinity())
                //                 ++i;
                // }
                // std::cout << "valied shadow buffer: " << i << std::endl;
        }

        // debug
        //  for (int i = 0; i < width; ++i)
        //          for (int j = 0; j < height; ++j)
        //                  if (depth_buffer_shadow.value()->at(getindex(i, j)) != -std::numeric_limits<float>::max())
        //                  {

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

Vec3f *Trans2Pts(Triangle &t)
{
        Vec3f *pts = new Vec3f[3]{
            Vec3f(t.a().x, t.a().y, t.a().z),
            Vec3f(t.b().x, t.b().y, t.b().z),
            Vec3f(t.c().x, t.c().y, t.c().z),
        };
        return pts;
}

void rst::rasterizer::rasterizer_triangle(Triangle &t)
{
        // Vec3f pts[3] = {t.a().toVec3(), t.b().toVec3(), t.c().toVec3()};
        Vec3f *pts = Trans2Pts(t);
        auto normal_tri = (pts[1] - pts[0]) ^ (pts[2] - pts[0]);
        delete pts;
        Scanbbox(t, normal_tri, rst::ForNormalMapping);
}

void rst::rasterizer::rasterizer_preshadow(Triangle &t, std::vector<float> *dbs)
{
        Vec3f *pts = Trans2Pts(t);
        auto normal_tri = (pts[1] - pts[0]) ^ (pts[2] - pts[0]);
        delete pts;
        Scanbbox(t, normal_tri, rst::ForShadowMapping);
}

void rst::rasterizer::clear(Buffers buf)
{
        if (buf == Buffers::Color)
        {
                std::fill(frame_buffer.begin(), frame_buffer.end(), Vec3f(0.0f, 0.0f, 0.0f));
                if (MSAA)
                {
                        std::fill(frame_buffer_msaa.value().begin(), frame_buffer_msaa.value().end(), Vec3f(0.0f, 0.0f, 0.0f));
                }
        }
        else if (buf == Buffers::Depth)
        {
                std::fill(depth_buffer.begin(), depth_buffer.end(), -std::numeric_limits<float>::infinity());
                if (MSAA)
                {
                        std::fill(depth_buffer_msaa.value().begin(), depth_buffer_msaa.value().end(), -std::numeric_limits<float>::max());
                }
        }
}

void rst::rasterizer::Scanbbox(Triangle &t, Vec3f &pts_norm, RenderMode mode)
{
        // 0 for shadow mapping
        Vec3f *pts = Trans2Pts(t);
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

                        if (MSAA)
                        {
                                /* hard code */
                                std::vector<Vec2f> supersampling = {
                                    Vec2f(0.25f, 0.25f),
                                    Vec2f(0.75f, 0.25f),
                                    Vec2f(0.25f, 0.75f),
                                    Vec2f(0.75f, 0.75f)};
                                bool flag = false;
                                for (int i = 0; i < this->MSAA_sample; ++i)
                                {
                                        auto P = Vec3f(supersampling[i].x + (float)x, supersampling[i].y + (float)y, 0);

                                        if (isInTriangle(pts, P, this->MSAA))
                                        {
                                                Vec3f coefficient = barycentric(pts, P);
                                                P.z = coefficient.x * pts[0].z + coefficient.y * pts[1].z + coefficient.z * pts[2].z; // z_interpolation < 0
                                                if (P.z > depth_buffer_msaa.value()[getindex_msaa(x, y) + i]) // early - z test
                                                {
                                                        depth_buffer_msaa.value()[getindex_msaa(x, y) + i] = P.z; // update
                                                        flag = true;                            
                                                        fragment_shading(t, pts_norm, coefficient, mode, P, i);
                                                }
                                        }
                                }
                                if (flag)
                                {
                                        if (!set_pixel(Vec2i(x, y), getColor_msaa((*frame_buffer_msaa), getindex_msaa(x, y))))
                                        {
                                                std::cout << "set pixel failed" << std::endl;
                                        }
                                }
                        }
                        else
                        {
                                Vec3f P(x + 0.5f, y + 0.5f, 0);
                                if (isInTriangle(pts, P, this->MSAA))
                                {
                                        Vec3f coefficient = barycentric(pts, P);
                                        P.z = coefficient.x * pts[0].z + coefficient.y * pts[1].z + coefficient.z * pts[2].z; // z_interpolation < 0

                                        if (P.z > depth_buffer[getindex(x, y)]) // early z test
                                        {
                                                depth_buffer[getindex(x, y)] = P.z; // update
                                                fragment_shading(t, pts_norm, coefficient, mode, P);
                                        }
                                }
                        }
                }
        }
        delete pts;
}

inline void rst::rasterizer::fragment_shading(Triangle &t, Vec3f &pts_norm, const Vec3f bar, RenderMode mode, const Vec3f &point, const int i)
{
        const int x = static_cast<int>(point.x);
        const int y = static_cast<int>(point.y);
        const int idx = MSAA ? getindex_msaa(x, y) : getindex(x, y);
        Vec3f color = Vec3f(255, 255, 255);
        auto interpolated_normal = pts_norm;
        auto interpolated_texcoords = Vec2f(-1, -1);
        std::function<Vec3f(MyShader::FragmentShader *)> FS_method = nullptr;

        if (mode == rst::ForNormalMapping)
        {
                auto interpolated_color = t.getColor(0) * bar.x + t.getColor(1) * bar.y + t.getColor(2) * bar.z;
                interpolated_normal = bar.x * t.getNormal(0) + bar.y * t.getNormal(1) + bar.z * t.getNormal(2);
                interpolated_texcoords = bar.x * t.getUV(0) + bar.y * t.getUV(1) + bar.z * t.getUV(2);
                color = Vec3f(interpolated_color.r, interpolated_color.g, interpolated_color.b);
        }
        else if (mode == rst::ForShadowMapping)
        {
                if (!depth_buffer_shadow.has_value())
                {
                        std::cerr << "depth_buffer_shadow is not set" << std::endl;
                        return;
                }
                // uniform_Mshadow
                FS_method = MyShader::depth_shading; // 片元着色器
        }
        MyShader::DeepFragmentShader *fs = new MyShader::DeepFragmentShader(point, color, interpolated_normal, pts_norm, interpolated_texcoords, texture.has_value() ? texture.value() : nullptr,
                                                                            normal_map.has_value() ? normal_map.value() : nullptr,
                                                                            lightDir, depth_buffer_shadow.has_value() ? depth_buffer_shadow.value() : nullptr,
                                                                            DeepthCamera.value()->GetMshadow());
        fs->width = this->width;
        fs->height = this->height;
        fs->set_M(Camera->GetMVMatrix());
        fs->set_MIT(Camera->GetMITMatrix());
        if (specular_map.has_value()) // specular map
                fs->set_sp_map(specular_map.value());
        fs->set_msaa(MSAA);
        fs->set_msaa_sample(MSAA_sample);
        fs->set_msaa_index(i);

        if (mode == rst::ForShadowMapping)
        {
                auto pixel_color = FS_method(fs);
        }
        else if (mode == rst::ForNormalMapping)
        {
                auto pixel_color = fragmentShader(fs);
                if (this->MSAA)
                {
                        (*frame_buffer_msaa)[idx + i] = pixel_color;
                }
                else
                {
                        if (!set_pixel(Vec2i(x, y), pixel_color))
                        {
                                std::cout << "set pixel failed" << std::endl;
                        }
                }
        }
        delete fs;
}