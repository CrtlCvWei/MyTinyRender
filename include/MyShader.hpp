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
        Vec3f normal_fragment;
        Vec2f uv;
        Texture *tex;
        Texture *normal_tex;

        MyMtrix::Matrix uniform_M;
        MyMtrix::Matrix uniform_MIT;

        Vec3f lightDir;

        FragmentShader() : vex(Vec3f()), color(Vec3f()), normal(Vec3f()), normal_fragment(Vec3f()), uv(Vec2f()), tex(nullptr), lightDir(Vec3f(0, 0, 1)), uniform_MIT(MyMtrix::Matrix()){};
        FragmentShader(const Vec3f &v, const Vec3f &c, const Vec3f &n, const Vec3f &n_, const Vec2f &u, Texture *t) : vex(v), color(c), normal(n), normal_fragment(n_), uv(u), tex(t), normal_tex(nullptr), lightDir(Vec3f(0, 0, 1)), uniform_MIT(MyMtrix::Matrix()){};
        FragmentShader(const Vec3f &v, const Vec3f &c, const Vec3f &n, const Vec3f &n_, const Vec2f &u, Texture *t, Vec3f l) : vex(v), color(c), normal(n), normal_fragment(n_), uv(u), tex(t), normal_tex(nullptr), lightDir(l), uniform_MIT(MyMtrix::Matrix()){};
        FragmentShader(const Vec3f &v, const Vec3f &c, const Vec3f &n, const Vec3f &n_, const Vec2f &u, Texture *t, Texture *nt, Vec3f l) : vex(v), color(c), normal(n), normal_fragment(n_), uv(u), tex(t), normal_tex(nt), lightDir(l), uniform_MIT(MyMtrix::Matrix()){};

        void set_M(MyMtrix::Matrix m)
        {
            uniform_M = m;
        }
        void set_MIT(MyMtrix::Matrix m)
        {
            uniform_MIT = m;
        }

        ~FragmentShader(){};
    };

    struct VertexShader
    {
    public:
        Vec4f position;
        MyMtrix::Matrix uniform_M;
        MyMtrix::Matrix ViewPort;
        VertexShader(Vec4f p, MyMtrix::Matrix pv, MyMtrix::Matrix vp) : position(p), uniform_M(pv), ViewPort(vp){};
    };

    // Some Shader Function
    Vec4f vertex_shader(MyShader::VertexShader &payload, MyMtrix::Matrix);
    Vec3f gouraud_shading(FragmentShader &payload);
    Vec3f normal_fragment_shader(MyShader::FragmentShader &payload);
    Vec3f flat_shader(MyShader::FragmentShader &payload);
    Vec3f phong_shading(FragmentShader &payload);
    Vec3f texture_shader(MyShader::FragmentShader &payload);

    Vec3f Blinn_Phong(FragmentShader &payload);

}

#endif