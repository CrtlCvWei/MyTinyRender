
#include <iostream>
#include "tgaimage.hpp"
#include "LineDraw.hpp"
#include "model.h"
#include "Triangle.hpp"

void makeModel(Model *model, int width, int height)
{
        TGAImage image = TGAImage(width, height, TGAImage::Format::RGB);
        for (int i = 0; i < model->nfaces(); i++)
        {
                std::vector<int> face = model->face(i);
                Vec2i screen_coords[3];
                Vec3f world_coords[3];
                Vec3f Light{1, 0, -1};
                Vec3f View{0, 0, -1};

                for (int j = 0; j < 3; j++)
                {
                        world_coords[j] = model->vert(face[j]);
                        screen_coords[j] = Vec2i((world_coords[j].x + 1.) * width / 2., (world_coords[j].y + 1.) * height / 2.);
                }
                // float La = 0.0f; // 环境光照


                float Ld = std::max(0.f, Light.normalize() * NormalOfTriangle(world_coords));

                Vec3f h = (Light + View).normalize();
                float Ls = std::max(0.f, h * NormalOfTriangle(world_coords));

                if((Ld+Ls)> 0)
                {
                  triangle(screen_coords, image, TGAColor((Ld+Ls)/2 * 255, (Ld+Ls)/2 * 255,(Ld+Ls)/2 * 255, 255));
                }
                        
                
        }
        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        image.write_tga_file("output.tga");
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

        makeModel(model, 800, 800);

        return 0;
}