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

    enum class Buffers
    {
        Color = 1,
        Depth = 2
    };

    class rasterizer
    {
    private:
        int width, height; // 屏幕宽高
        bool MSAA = false; // 是否开启多重采样
        const int MSAA_sample = 4; // 多重采样采样点数

        MyMtrix::Matrix modelMatrix;
        MyCamera::Camera* Camera;


        std::vector<Vec3f> frame_buffer;
        std::vector<float> depth_buffer;// z-buffer
        std::optional<std::vector<float>> depth_buffer_x4;

        std::optional<Texture*> texture;

		std::function<Vec3f(MyShader::FragmentShader)> fragmentShader;
		std::function<Vec3f(MyShader::VertexShader)> vertexShader;

        Buffers buffer;


    public:

        rasterizer(int w, int h,MyCamera::Camera* c):width(w), height(h),Camera(c)
        {
            frame_buffer.resize(w * h);
            depth_buffer.resize(w * h);

            std::fill(frame_buffer.begin(), frame_buffer.end(), Vec3f{-1, -1, -1});
            std::fill(depth_buffer.begin(), depth_buffer.end(),-std::numeric_limits<float>::max());
        
            if(MSAA)
            {
                depth_buffer_x4 = std::vector<float>(w * h * MSAA_sample, -std::numeric_limits<float>::max());
            }
        }

        void set_model(const MyMtrix::Matrix& m);

        int getindex(int x, int y);

        std::vector<Vec3f> getFrameBuffer()const {return frame_buffer;}

		void set_texture(Texture* tex);

		void clear(Buffers buf);

		void set_pixel(Vec2i& point, Vec3f& color);
        void set_pixel_super_sampling(Vec2i& point, Vec3f& color); 

		void set_vertexShader(std::function<Vec3f(MyShader::VertexShader)> vert_shader);
		void set_fragmentShader(std::function<Vec3f(MyShader::FragmentShader)> frag_shader);

		void draw(std::vector<Triangle>& TriangleList);

        void rasterizer_triangle(Triangle &t);

        ~rasterizer(){
            if(texture)
            {
                delete texture.value();
            }
        }
    };



}

#endif // PERJECTIVE_HPP