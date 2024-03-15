
#include <iostream>
#include "tgaimage.hpp"
#include "model.h"
#include "Rasterizer.hpp"
#include "MyShader.hpp"
#include "MyCamera.hpp"

const int width = 800;
const int height = 800;

Vec3f lightDir(1, 0.75, 1); // 平行光方向

Vec3f eye_position(-1.f, 1.f, 2.5f);
Vec3f center(0.f, 0.f, 0.f); //
Vec3f Up(0.f, 1.f, 0.f);     // 相机向上的方向

float fovy = 90.f;    // 视野角度
float aspect = 1.f;   // 宽高比
float zNear = 1.f;    // 近裁剪面
float zFar = 1 / 3.f; // 远裁剪面

MyMtrix::Matrix modelMatrix()
{
        return MyMtrix::Matrix::identity(4);
}

int main(int argc, char **argv)
{

        Model *model = nullptr;
        std::string name;
        std::string output = "output.tga"; // OUPUT FILE NAME
        std::string _diffuse;              // diffuse map
        std::string nm;                    // normal map
        std::string sp;                    // specular map
        bool set_depth_buffer_shadow = false;
        MyCamera::Camera *camera = new MyCamera::Camera(eye_position, center, Up, fovy, aspect, zNear, zFar);
        std::function<Vec4f(MyShader::VertexShader *, MyMtrix::Matrix)> VertexShader = MyShader::vertex_shader;
        std::function<Vec3f(MyShader::FragmentShader *)> FragmentShader;
        if (argc > 1)
        {
                if (argc < 4)
                {
                        std::cerr << "Usage: " << argv[0] << " <obj_file>  <shading methods>  <ouput_file>" << std::endl;
                        return -2;
                }
                std::cout << "argv[1] = " << argv[1] << std::endl;
                if (std::strcmp(argv[1],"diablo3")  == 0)
                {
                        name = "diablo3_pose";
                        model = new Model("obj/diablo3_pose/diablo3_pose.obj");
                }

                else if (std::strcmp(argv[1],"african_head")  == 0)
                {
                        name = "african_head";
                        model = new Model("obj/african_head/african_head.obj");
                }
                else
                {
                        std::cerr << "<obj_file> " << argv[1] << ": not in {diablo3,african_head}" << std::endl;
                        return -1;
                }
                if (std::strcmp(argv[2],"flat") == 0)
                {
                        FragmentShader = MyShader::flat_shader; // 片元着色器
                }
                else if (std::strcmp(argv[2],"gouraud") == 0)
                {
                        FragmentShader = MyShader::gouraud_shading; // 片元着色器
                }
                else if (std::strcmp(argv[2],"texture") == 0)
                {
                        FragmentShader = MyShader::texture_shader; // 片元着色器
                }
                else if (std::strcmp(argv[2],"normal") == 0)
                {
                        FragmentShader = MyShader::normal_fragment_shader; // 片元着色器
                }
                else if (std::strcmp(argv[2],"blinn_phong") == 0 || std::strcmp(argv[2],"blinn") == 0)
                {
                        FragmentShader = MyShader::Blinn_Phong; // 片元着色器
                }
                else if (std::strcmp(argv[2],"depth") == 0 ||std::strcmp(argv[2],"Depth") == 0 )
                {
                        FragmentShader = MyShader::depth_shading; // 片元着色器
                }
                else if (std::strcmp(argv[2],"blinn_phong_shadow") == 0 || std::strcmp(argv[2],"shadow") == 0)
                {
                        FragmentShader = MyShader::Blinn_Phong_With_ShadowMapping; // 片元着色器
                        set_depth_buffer_shadow = true;
                }
                else
                {
                        std::cerr << "<shading methods> " << argv[1] << ": not in {flat,gouraud,texture,normal,blinn_phong,blinn_phong_shadow}" << std::endl;
                        return -3;
                }

                output = std::string(argv[3]) + ".tga";
        }
        else
        {
                model = new Model("obj/diablo3_pose/diablo3_pose.obj");
                name = "diablo3_pose";
                FragmentShader = MyShader::flat_shader; // 片元着色器
        }

        // makeModel(model, width, height);
        TGAImage image(width, height, TGAImage::Format::RGB);
        rst::rasterizer r(width, height, camera, lightDir);

        if (set_depth_buffer_shadow)
        {
                std::vector<float> *depth_buffer_shadow = new std::vector<float>(width * height, -std::numeric_limits<float>::max());
                r.set_depth_buffer(depth_buffer_shadow);
        }

        r.SetMASS(false);
        if (argc > 4)
                r.SetMASS( (std::strcmp(argv[4],"msaa") || std::strcmp(argv[4],"MSAA")) ? true : false);
        // 存储所有需要绘制的三角形面片
        // model->getTriangles();

        // 给定纹理并设置
        _diffuse = "obj/" + name + "/" + name + "_diffuse.tga";
        Texture *tex = new Texture(_diffuse);
        r.set_texture(tex);

        nm = "obj/" + name + "/" + name + "_nm.tga";
        Texture *norm_map = new Texture(nm);
        r.set_normal_map(norm_map);

        // specular map
        sp = "obj/" + name + "/" + name + "_spec.tga";
        Texture *sp_map = new Texture(sp);
        r.set_specular_map(sp_map);

        // 清空帧缓冲和zBuffer
        r.clear(rst::Buffers::Color);
        r.clear(rst::Buffers::Depth);

        // 设置MVP矩阵
        r.set_model(modelMatrix());

        // 设置顶点着色器和片元着色器

        r.set_vertexShader(VertexShader);
        r.set_fragmentShader(FragmentShader);

        try
        {
                r.draw(model->getTriangles());
        }
        catch (const std::exception &e)
        {
                std::cerr << e.what() << '\n';
        }

        std::vector<Vec3f> &frame_buffer = r.getFrameBuffer();

        // 将帧缓冲中的颜色值写入image中
        for (int i = 0; i < width; i++)
        {
                for (int j = 0; j < height; j++)
                {
                        Vec3f color = frame_buffer[j * width + i];
                        image.set(i, j, TGAColor(color.x, color.y, color.z, 255));
                }
        }

        image.flip_vertically();
        image.write_tga_file(output.c_str());

        delete model;
        delete camera;
        return 0;
}