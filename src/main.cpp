
#include <iostream>
#include "tgaimage.hpp"
#include "model.h"
#include "Rasterizer.hpp"
#include "MyShader.hpp"
#include "MyCamera.hpp"

const int width = 800;
const int height = 800;

Vec3f lightDir(1, 1, 3); // 平行光方向

Vec3f eye_position(-1.f, 1.f, 3.f);
Vec3f center(0.f, 0.f, 0.f);//
Vec3f Up(0.f, 1.f, 0.f);//相机向上的方向

float fovy = 90.f;   // 视野角度
float aspect = 1.f; // 宽高比
float zNear = 1.f;  // 近裁剪面
float zFar = 1/3.f;   // 远裁剪面

MyCamera::Camera* camera = new MyCamera::Camera(eye_position, center, Up, fovy, aspect, zNear, zFar);

MyMtrix::Matrix modelMatrix()
{
        return MyMtrix::Matrix::identity(4);
}

int main(int argc, char **argv)
{
        Model *model = NULL;

        if (argc == 2)
        {
                model = new Model(argv[1]);
        }
        else
        {
                model = new Model("obj/african_head.obj");
        }

        // makeModel(model, width, height);
        TGAImage image(width, height, TGAImage::Format::RGB);
        // 创建光栅化对象
        rst::rasterizer r(width, height, camera,lightDir);
        std::function<Vec4f(MyShader::VertexShader,MyMtrix::Matrix)> VertexShader = MyShader::vertex_shader;     // 顶点着色器
        std::function<Vec3f(MyShader::FragmentShader)> FragmentShader = MyShader::Blinn_Phong; // 片元着色器

        // 清空帧缓冲和zBuffer
        r.clear(rst::Buffers::Color);
        r.clear(rst::Buffers::Depth);

        // 存储所有需要绘制的三角形面片
        // model->getTriangles();

        // 给定纹理并设置
        std::string _diffuse = "obj/african_head_diffuse.tga";
        Texture *tex = new Texture(_diffuse);
        r.set_texture(tex);

        std::string nm = "obj/african_head_nm.tga";
        Texture *norm_map = new Texture(nm);
        r.set_normal_map(norm_map);

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
        image.write_tga_file("output_owsp.tga");

        delete model;
        return 0;
}