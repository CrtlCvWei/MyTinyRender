#pragma once
#ifndef __MYCAMERA_H__
#define __MYCAMERA_H__

#include "geometry.hpp"
#include "Triangle.hpp"

namespace MyCamera
{
    class Camera
    {
    private:
        Vec3f eye_position;
        Vec3f center;
        Vec3f Up;

        float fovy;   // 视野角度
        float aspect; // 宽高比
        float zNear;  // 近裁剪面
        float zFar;   // 远裁剪面

        MyMtrix::Matrix viewMatrix;
        MyMtrix::Matrix projectionMatrix;

    protected:
        MyMtrix::Matrix LookAt(Vec3f eye, Vec3f center, Vec3f up); // 视图矩阵

        MyMtrix::Matrix Perspective(float fovy, float aspect, float zNear, float zFar); // 透视投影矩阵

    public:
        Camera() : eye_position(Vec3f(0, 0, -1.f)), center(Vec3f(0, 0, 0)), Up(Vec3f(0, 1, 1)),
                   fovy(45.0f), aspect(1.0f), zNear(0.1f), zFar(100.0f)
        {
            viewMatrix = LookAt(eye_position, center, Up);
            projectionMatrix = Perspective(fovy, aspect, zNear, zFar);
        }

        Camera(const Vec3f &eye, const Vec3f &c, const Vec3f &u) : eye_position(eye), center(c), Up(u),
                                                                   fovy(45.0f), aspect(1.0f), zNear(0.1f), zFar(100.0f)
        {
            viewMatrix = LookAt(eye_position, center, Up);
            projectionMatrix = Perspective(fovy, aspect, zNear, zFar);
        }

        Camera(const Vec3f &eye, const Vec3f &c, const Vec3f &u, const float fovy, const float aspect, const float zNear, const float zFar) : eye_position(eye), center(c), Up(u),
                                                                                                                                              fovy(fovy), aspect(aspect), zNear(zNear), zFar(zFar)
        {
            viewMatrix = LookAt(eye_position, center, Up);
            projectionMatrix = Perspective(fovy, aspect, zNear, zFar);
        }

        MyMtrix::Matrix getViewMatrix() const
        {
            return viewMatrix;
        }

        MyMtrix::Matrix getProjectionMatrix() const
        {
            return projectionMatrix;
        }

        MyMtrix::Matrix MVMatrix(); // ModelView Matrix

        MyMtrix::Matrix ViewPort(int x, int y, int w, int h, float depth = 255.f); // 3D场景坐标转换为屏幕坐标
    };

    // Vec3f ProjectTriangle(Triangle &tri);

} // namespace MyCamera

#endif