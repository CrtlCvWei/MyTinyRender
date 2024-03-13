#pragma once
#ifndef __MYCAMERA_H__
#define __MYCAMERA_H__

#include "geometry.hpp"
#include "Triangle.hpp"
#include <optional>

namespace MyCamera
{
    enum CameraType
    {
        Perspe = 0,
        Orth = 1
    };
    class Camera
    {
    private:
        CameraType type;
        Vec3f eye_position;
        Vec3f center;
        Vec3f Up;

        float fovy;   // 视野角度
        float aspect; // 宽高比
        float zNear;  // 近裁剪面
        float zFar;   // 远裁剪面

        MyMtrix::Matrix viewMatrix;
        MyMtrix::Matrix projectionMatrix;
        MyMtrix::Matrix MIT;
        std::optional<MyMtrix::Matrix> uniform_Mshadow; // mapping from shadow buffer space to screen space

    protected:
        MyMtrix::Matrix LookAt(Vec3f eye, Vec3f center, Vec3f up); // 视图矩阵

        MyMtrix::Matrix Perspective(float fovy, float aspect, float zNear, float zFar); // 透视投影矩阵

    public:
        Camera() : eye_position(Vec3f(0, 0, -1.f)), center(Vec3f(0, 0, 0)), Up(Vec3f(0, 1, 1)),
                   fovy(45.0f), aspect(1.0f), zNear(0.1f), zFar(100.0f)
        {
            viewMatrix = LookAt(eye_position, center, Up);
            projectionMatrix = Perspective(fovy, aspect, zNear, zFar);
            MIT = (projectionMatrix * viewMatrix).inverse().transpose();
            type = Perspe;
        }

        Camera(const Vec3f &eye, const Vec3f &c, const Vec3f &u) : eye_position(eye), center(c), Up(u),
                                                                   fovy(45.0f), aspect(1.0f), zNear(0.1f), zFar(100.0f), type(Perspe)
        {
            viewMatrix = LookAt(eye_position, center, Up);
            projectionMatrix = Perspective(fovy, aspect, zNear, zFar);
            MIT = (projectionMatrix * viewMatrix).inverse().transpose();
        }

        Camera(const Vec3f &eye, const Vec3f &c, const Vec3f &u, const float fovy, const float aspect, const float zNear, const float zFar) : eye_position(eye), center(c), Up(u), 
        fovy(fovy), aspect(aspect), zNear(zNear), zFar(zFar), type(Perspe)
        {
            viewMatrix = LookAt(eye_position, center, Up);
            projectionMatrix = Perspective(fovy, aspect, zNear, zFar);
            MIT = (projectionMatrix * viewMatrix).inverse().transpose();
        }
        Camera(const Vec3f &eye, const Vec3f &c, const Vec3f &u, const float fovy, const float aspect, const float zNear, const float zFar, CameraType ct) 
        : Camera(eye, c, u, fovy, aspect, zNear, zFar)
        {
            type = ct;
            projectionMatrix = Perspective(fovy, aspect, zNear, zFar);
            MIT = (projectionMatrix * viewMatrix).inverse().transpose();
        }

        MyMtrix::Matrix getViewMatrix() const { return viewMatrix; }

        MyMtrix::Matrix getProjectionMatrix() const { return projectionMatrix; }

        Vec3f Geteye_position() const { return eye_position; }

        Vec3f Getcenter() const { return center; }

        Vec3f GetUp() const { return Up; }

        float Getfovy() const { return fovy; }
        float Getaspect() const { return aspect; }
        float GetNear() const { return zNear; }
        float GetFar() const { return zFar; }

        bool HasMshadow() const { return uniform_Mshadow.has_value();}
        void SetMshadow(MyMtrix::Matrix& m)
        {
            uniform_Mshadow = m;
        }

        MyMtrix::Matrix GetMVMatrix() ; // ModelView Matrix

        MyMtrix::Matrix GetMITMatrix() const; // 逆转置矩阵

        MyMtrix::Matrix ViewPort(int x, int y, int w, int h, float depth = 255.f); // 3D场景坐标转换为屏幕坐标

        MyMtrix::Matrix GetMshadow() const;
    };

    // Vec3f ProjectTriangle(Triangle &tri);

} // namespace MyCamera

#endif