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

void rst::rasterizer::set_normal_map(Texture *tex)
{
        normal_map = tex;
}

bool rst::rasterizer::set_pixel(Vec2i &point, Vec3f &color)
{
        int ind = point.x + point.y * width;
        if (ind < 0 || ind >= frame_buffer.size())
                return false;

        try
        {
                // std::cout <<"before: " << frame_buffer[ind] << std::endl;
                frame_buffer[ind] = color;
                // std::cout <<"after: " << frame_buffer[ind] << std::endl;
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

void rst::rasterizer::set_vertexShader(std::function<Vec4f(MyShader::VertexShader, MyMtrix::Matrix modelMatrix)> vert_shader)
{
        vertexShader = vert_shader;
}

void rst::rasterizer::set_fragmentShader(std::function<Vec3f(MyShader::FragmentShader)> frag_shader)
{
        fragmentShader = frag_shader;
}

void rst::rasterizer::draw(std::vector<Triangle> &TriangleList)
{

        auto ViewPortTM = Camera->ViewPort(width / 8, height / 8, width * 3 / 4, height * 3 / 4, 255.f);

        for (auto &tri : TriangleList)
        {

                Triangle newtri(tri); // copy
                std::array<Vec3f, 3> pts = tri.to_vec3();
                std::array<Vec4f, 3> pts_clip;
                for (int i = 0; i < 3; i++)
                {
                        MyShader::VertexShader vs( Vec4f(pts[i].x, pts[i].y, pts[i].z, 1.0f), Camera->MVMatrix(), ViewPortTM);
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
        Vec2i bboxMin(width - 1, height - 1);
        Vec2i bboxMax(0, 0);

        bboxMin.x = std::min({bboxMin.x, static_cast<int>(pts[0].x), static_cast<int>(pts[1].x), static_cast<int>(pts[2].x)});
        bboxMin.y = std::min({bboxMin.y, static_cast<int>(pts[0].y), static_cast<int>(pts[1].y), static_cast<int>(pts[2].y)});
        bboxMax.x = std::max({bboxMax.x, static_cast<int>(pts[0].x), static_cast<int>(pts[1].x), static_cast<int>(pts[2].x)});
        bboxMax.y = std::max({bboxMax.y, static_cast<int>(pts[0].y), static_cast<int>(pts[1].y), static_cast<int>(pts[2].y)});

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

                                if (P.z > depth_buffer[getindex(x, y)])
                                {

                                        depth_buffer[getindex(x, y)] = P.z; // update

                                        auto interpolated_color = t.getColor(0) * coefficient.x + t.getColor(1) * coefficient.y + t.getColor(2) * coefficient.z;
                                        auto interpolated_normal = coefficient.x * t.getNormal(0) + coefficient.y * t.getNormal(1) + coefficient.z * t.getNormal(2);

                                        auto interpolated_texcoords = coefficient.x * t.getUV(0) + coefficient.y * t.getUV(1) + coefficient.z * t.getUV(2);

                                        Vec3f color = Vec3f(interpolated_color.r, interpolated_color.g, interpolated_color.b);

                                        // Check the constructor of MyShader::FragmentShader and ensure it accepts the provided arguments.
                                        MyShader::FragmentShader fs(P, color, interpolated_normal, normal_tri, interpolated_texcoords, texture ? texture.value() : nullptr, normal_map ? normal_map.value() : nullptr, this->lightDir);
                                        fs.set_M(Camera->MVMatrix());
                                        fs.set_MIT(Camera->MITMatrix());

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