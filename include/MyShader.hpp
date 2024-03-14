#pragma once
#ifndef __MYSHADER_H__
#define __MYSHADER_H__

#include <iostream>
#include <vector>
#include "geometry.hpp"
#include "Texture.hpp"
#include <optional>

namespace MyShader
{
    enum FragmentShaderClass
    {
        Base = 0,
        Deep = 1
    };

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
        std::optional<Texture *> specular_map;

        MyMtrix::Matrix uniform_M;
        MyMtrix::Matrix uniform_MIT;

        Vec3f lightDir;

        bool MSAA = false;
        int MSAA_sample = 4;
        int MSAA_index = 0;

        virtual FragmentShaderClass GetClassName()
        {
            return Base;
        }

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
        void set_sp_map(Texture *sp)
        {
            specular_map = sp;
        }

        void set_msaa(bool msaa)
        {
            MSAA = msaa;
        }

        void set_msaa_sample(int sample)
        {
            MSAA_sample = sample;
        }

        void set_msaa_index(int index)
        {
            MSAA_index = index;
        }
        ~FragmentShader(){};
    };

    class DeepFragmentShader : public FragmentShader
    {
    public:
        int width, height;
        std::vector<float> *depth_buffer;
        MyMtrix::Matrix uniform_Mshadow;

        DeepFragmentShader(const Vec3f &v, const Vec3f &c, const Vec3f &n, const Vec3f &n_, const Vec2f &u, Texture *t) : FragmentShader(v, c, n, n_, u, t)
        {
            width = t->width;
            height = t->height;
            depth_buffer = new std::vector<float>(t->width * t->height, -std::numeric_limits<float>::max());
            uniform_Mshadow = MyMtrix::Matrix();
        }
        DeepFragmentShader(const Vec3f &v, const Vec3f &c, const Vec3f &n, const Vec3f &n_, const Vec2f &u, Texture *t, Texture *nt, Vec3f l, std::vector<float> *db, MyMtrix::Matrix &mshadow) : FragmentShader(v, c, n, n_, u, t, nt, l)
        {
            depth_buffer = db;
            uniform_Mshadow = mshadow;
        }

        virtual FragmentShaderClass GetClassName() override
        {
            return Deep;
        }
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
    Vec4f vertex_shader(VertexShader *payload, MyMtrix::Matrix);
    Vec3f gouraud_shading(FragmentShader *payload);
    Vec3f normal_fragment_shader(FragmentShader *payload);
    Vec3f flat_shader(FragmentShader *payload);
    Vec3f phong_shading(FragmentShader *payload);
    Vec3f texture_shader(FragmentShader *payload);
    Vec3f Blinn_Phong(FragmentShader *payload);
    Vec3f depth_shading(FragmentShader *payload);
    Vec3f Blinn_Phong_With_ShadowMapping(MyShader::FragmentShader *payload);

}

#endif