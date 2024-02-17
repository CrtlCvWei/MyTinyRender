#include "MyCamera.hpp"
#include <cassert>

#define PI 3.14159265358979323846

MyMtrix::Matrix MyCamera::Camera::MVMatrix()
{
        return projectionMatrix * viewMatrix;
}

MyMtrix::Matrix MyCamera::Camera::ViewPort(int x, int y, int w, int h, float depth)
{
        // ref from OpenGL
        MyMtrix::Matrix m = MyMtrix::Matrix::identity(4);
        m[0][3] = x + w * 0.5;
        m[1][3] = y + h * 0.5;
        m[2][3] = depth * 0.5;
        m[0][0] = w * 0.5;
        m[1][1] = h * 0.5;
        m[2][2] = depth * 0.5;

        return m;
}

MyMtrix::Matrix MyCamera::Camera::LookAt(Vec3f eye, Vec3f center, Vec3f up)
{
        // View Matrix
        Vec3f v = (eye - center).normalize(); // 
        Vec3f u = (up ^ v).normalize();
        Vec3f w = (v ^ u).normalize(); // 修正叉乘顺序并归一化
        MyMtrix::Matrix R = MyMtrix::Matrix::identity(4);
        MyMtrix::Matrix Tr = MyMtrix::Matrix::identity(4);
        for (int i = 0; i < 3; i++)
        {
                R[0][i] = u[i];
                R[1][i] = w[i];
                R[2][i] = v[i];
                Tr[i][3] = -center[i];
        }

        return R * Tr;
}

MyMtrix::Matrix MyCamera::Camera::Perspective(float fovy, float aspect, float zNear, float zFar)
{
        assert(zNear > 0);
        assert(zFar > 0);

        zFar = -zFar;
        zNear = -zNear;

        float t = std::abs(std::atan(fovy * PI / 180 / 2) * zNear); // 计算 top
        float b = -t;                                      // 计算 bottom
        float r = t * aspect;                              // 计算 right
        float l = -r;                                      // 计算 left

       

        MyMtrix::Matrix m = MyMtrix::Matrix::identity(4);

        // // X_{c} = m[0][0] * X_{e} + m[0][2] * Z_{e}
        // m[0][0] = 2 * zNear / (r - l);
        // m[0][2] = (r + l) / (r - l);
        // // Y_{c} = m[1][1] * Y_{e} + m[1][2] * Z_{e}
        // m[1][1] = 2 * zNear / (t - b);
        // m[1][2] = (t + b) / (t - b);
        // // Z_{c} = m[2][2] * Z_{e} + m[2][3]
        // // m[2][2] = -(zFar + zNear) / (zFar - zNear);
        // m[2][3] = -2 * zFar * zNear / (zFar - zNear);




        m[3][2] = -1 / eye_position.z;
        m[3][3] = 1  ;

        return m;
}
