//
// Created by LEI XU on 4/27/19.
//
#pragma once
#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H

#include "iostream"
#include "tgaimage.hpp"
#include "geometry.hpp"

class Texture
{
private:
    TGAImage image_data;

public:
    int width, height;

    Texture() : width(0), height(0), image_data(TGAImage()){};
    Texture(std::string &name)
    {
        image_data.read_tga_file(name.c_str()); // cover string to char*
        image_data.flip_vertically();

        width = image_data.get_width();
        height = image_data.get_height();
    }

    TGAColor getColor(float u, float v) { return image_data.get(u, v); }
    TGAColor getColor(Vec2f uv) { return image_data.get(uv.u, uv.v); }

    Vec3f getColorBilinear(float u, float v);
};
#endif // RASTERIZER_TEXTURE_H
