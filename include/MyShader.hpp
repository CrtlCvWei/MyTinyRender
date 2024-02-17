#pragma once
#ifndef __MYSHADER_H__
#define __MYSHADER_H__

#include <iostream>
#include <vector>
#include "geometry.hpp"
#include "Texture.hpp"

namespace MyShader
{

    class FragmentShader
    {

    public:
        Vec3f vex;
        Vec3f color;
        Vec3f normal;
        Vec2f uv;
        Texture *tex;
        
        FragmentShader() : vex(Vec3f()), color(Vec3f()), normal(Vec3f()), uv(Vec2f()), tex(nullptr){};
        FragmentShader(const Vec3f &v, const Vec3f &c, const Vec3f &n, const Vec2f &u, Texture *t) : vex(v), color(c), normal(n), uv(u), tex(t){};
        ~FragmentShader(){};
    };

    class VertexShader
    {
    public:
        Vec3f position;
    };
}

#endif