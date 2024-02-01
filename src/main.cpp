
#include <iostream>
#include "tgaimage.hpp"
#include "LineDraw.hpp"
#include "model.h"
#include "Triangle.hpp"

const int width = 800;
const int height = 800;

Vec3f world2screen(Vec3f v)
{
        return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

void makeModel(Model *model, int width, int height)
{
        TGAImage image = TGAImage(width, height, TGAImage::Format::RGB);
        float *zbuffer = new float[width * height];
        for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

        for (int i = 0; i < model->nfaces(); i++)
        {
                std::vector<int> face = model->face(i); //
                Vec3f screen_coords[3];
                Vec3f world_coords[3];
                Vec2i textures[3];
                Vec3f Light{-1, 0, -1};
                Vec3f View{0, 0, -1};

                for (int j = 0; j < 3; j++)
                {
                        world_coords[j] = model->vert(face[j]);
                        screen_coords[j] = world2screen(world_coords[j]);
                        std::cout << screen_coords[j].z <<std::endl;
                        textures[j] = model->uv(i, j);
                }

                if (Light * NormalOfTriangle(world_coords) > 0)
                {
                        float La = 0.0f; // 环境光照

                        float Ld = std::max(0.f, Light.normalize() * NormalOfTriangle(world_coords));

                        Vec3f h = (Light + View).normalize();
                        float Ls = std::pow(std::max(0.f, h * NormalOfTriangle(world_coords)), 1);
                        float light_ins = Ld + Ls + La;
                        if (light_ins > 0)
                        {
                                // triangle(screen_coords, zbuffer, image, textures, model, (Ld + Ls + La));
                                triangle(screen_coords, zbuffer, image, textures, model, light_ins);
                        }
                }
        }
        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        image.write_tga_file("output.tga");
        delete model;
        return;
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

        makeModel(model, width, height);

        return 0;
}