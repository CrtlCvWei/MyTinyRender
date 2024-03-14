#pragma once
#ifndef _RASTERIZER_HPP
#define _RASTERIZER_HPP

#include "geometry.hpp"
#include <optional>
#include "Texture.hpp"
#include "Triangle.hpp"
#include "MyShader.hpp"
#include <functional>
#include "MyCamera.hpp"

namespace rst
{

    enum RenderMode
    {
        ForShadowMapping = 0,
        ForNormalMapping = 1,
    };

    enum class Buffers
    {
        Color = 1,
        Depth = 2
    };

    class rasterizer
    {
    private:
        int width, height;         // 屏幕宽高
        Vec3f lightDir;            // 平行光方向
        bool MSAA = false;         // 是否开启多重采样
        const int MSAA_sample = 4; // 多重采样采样点数

        MyMtrix::Matrix modelMatrix;
        MyCamera::Camera *Camera;
        std::optional<MyCamera::Camera *> DeepthCamera;

        std::vector<Vec3f> frame_buffer;
        std::vector<float> depth_buffer;                     // z-buffer
        std::optional<std::vector<float>> depth_buffer_msaa; // MSAA
        std::optional<std::vector<Vec3f>> frame_buffer_msaa; // MSAA

        std::optional<Texture *> texture;
        std::optional<Texture *> normal_map;
        std::optional<Texture *> specular_map;

        std::optional<std::vector<float> *> depth_buffer_shadow; // for shadow mapping

        std::function<Vec3f(MyShader::FragmentShader *)> fragmentShader;
        std::function<Vec4f(MyShader::VertexShader *, MyMtrix::Matrix)> vertexShader;

        Buffers buffer;

    protected:
        void rasterizer_preshadow(Triangle &t, std::vector<float> *dbs);
        void Scanbbox(Triangle &t, Vec3f &pts_norm, RenderMode mode);
        inline void fragment_shading(Triangle &t,Vec3f &pts_norm,const Vec3f bar,RenderMode mode,const Vec3f& point,const int i = 0);
        Vec3f getColor_msaa(std::vector<Vec3f> &color,const int head_idx);

    public:
        rasterizer(int w, int h, MyCamera::Camera *c) : width(w), height(h), Camera(c), lightDir(Vec3f(0, 0, 1))
        {
            frame_buffer.resize(w * h);
            depth_buffer.resize(w * h);

            std::fill(frame_buffer.begin(), frame_buffer.end(), Vec3f{-1, -1, -1});
            std::fill(depth_buffer.begin(), depth_buffer.end(), -std::numeric_limits<float>::max());

            if (MSAA)
            {
                depth_buffer_msaa = std::vector<float>(w * h * MSAA_sample, -std::numeric_limits<float>::max());
            }
        }

        rasterizer(int w, int h, MyCamera::Camera *c, Vec3f l) : width(w), height(h), Camera(c), lightDir(l)
        {
            frame_buffer.resize(w * h);
            depth_buffer.resize(w * h);

            std::fill(frame_buffer.begin(), frame_buffer.end(), Vec3f{-1, -1, -1});
            std::fill(depth_buffer.begin(), depth_buffer.end(), -std::numeric_limits<float>::max());

            if (MSAA)
            {
                depth_buffer_msaa = std::vector<float>(w * h * MSAA_sample, -std::numeric_limits<float>::max());
            }
        }

        rasterizer(int w, int h, MyCamera::Camera *c, Vec3f l, std::vector<float> *dbs) : rasterizer(w, h, c, l)
        {
            depth_buffer_shadow = dbs;
        }

        void set_model(const MyMtrix::Matrix &m);

        int getindex(int x, int y);
        int getindex_msaa(int x, int y);
        Vec3f GetLightDir() const { return lightDir; }

        std::vector<Vec3f> getFrameBuffer() const { return frame_buffer; }

        void set_texture(Texture *tex);

        void set_normal_map(Texture *nmp);

        void set_specular_map(Texture *sp);

        void set_depth_buffer(std::vector<float> *db);

        void clear(Buffers buf);

        bool set_pixel(Vec2i &point, Vec3f &color);
        void set_pixel_super_sampling(Vec2i &point, Vec3f &color);

        void set_vertexShader(std::function<Vec4f(MyShader::VertexShader *, MyMtrix::Matrix modelMatrix)> vert_shader);
        void set_fragmentShader(std::function<Vec3f(MyShader::FragmentShader *)> frag_shader);

        void draw(std::vector<Triangle> &TriangleList);

        void rasterizer_triangle(Triangle &t);

        void SetMASS(bool flag)
        {
            MSAA = flag;
            if (MSAA)
            {
                frame_buffer_msaa = std::vector<Vec3f>(width * height * MSAA_sample, Vec3f(0.0f, 0.0f, 0.0f));
                depth_buffer_msaa = std::vector<float>(width * height * MSAA_sample, -std::numeric_limits<float>::max());
                if(depth_buffer_shadow.has_value())
                {
                    depth_buffer_shadow.value()->resize(width * height * MSAA_sample, -std::numeric_limits<float>::max());
                }
            }
        }

        ~rasterizer()
        {
            if (texture)
                delete texture.value();
            if (depth_buffer_shadow)
                delete depth_buffer_shadow.value();
            if (normal_map)
                delete normal_map.value();
            if (specular_map)
                delete specular_map.value();
        }
    };

}

#endif // PERJECTIVE_HPP
