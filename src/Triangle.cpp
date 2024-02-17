#include "Triangle.hpp"
#include "LineDraw.hpp"

Triangle::Triangle() : pts{Vec4f(), Vec4f(), Vec4f()},
                       normals{Vec3f(0., 0., 0.), Vec3f(0., 0., 0.), Vec3f(0., 0., 0.)},
                       uv{Vec2f(), Vec2f(), Vec2f()},
                       color{TGAColor(), TGAColor(), TGAColor()}
{
}

Triangle::Triangle(Triangle &t)
{
    for (size_t i = 0; i < 3; ++i)
    {
        pts[i] = t.pts[i];
        normals[i] = t.normals[i];
        uv[i] = t.uv[i];
        color[i] = t.color[i];
    }
}

Triangle::Triangle(Vec4f *pts, Vec3f *normals, Vec2f *uv, TGAColor *color) : pts{pts[0], pts[1], pts[2]},
                                                                             normals{normals[0], normals[1], normals[2]},
                                                                             uv{uv[0], uv[1], uv[2]},
                                                                             color{color[0], color[1], color[2]}
{
}

std::array<Vec3f, 3> Triangle::to_vec3()
{
    std::array<Vec3f, 3> res;
    for (size_t i = 0; i < 3; ++i)
    {
        if (pts[i].w != 0.0f)
        {
            // std::cerr << "Error: the point is not in the homogeneous coordinate system\n";
            res[i] = Vec3f(pts[i].x / pts[i].w, pts[i].y / pts[i].w, pts[i].z / pts[i].w);
        }
        else
            res[i] = Vec3f(pts[i].x, pts[i].y, pts[i].z);
    }
    return res;
}

void triangle_OldSchool(Vec2i &v1, Vec2i &v2, Vec2i &v3, TGAImage &image, const TGAColor color)
{
    Vec2i temp = Vec2i();
    if (v1.v > v2.v)
    {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }
    if (v2.v > v3.v)
    {
        temp = v2;
        v2 = v3;
        v3 = temp;
    }
    if (v1.v > v3.v)
    {
        temp = v1;
        v1 = v3;
        v3 = temp;
    }
    // now sort by y: v1 < v2 < v3

    int longbroundary = v3.v - v1.v;
    int shortbroundary = v2.v - v1.v + 1;
    int middlebroundary = v3.v - v2.v + 1;
    Vec2i A = Vec2i();
    Vec2i B = Vec2i();
    float alpha, belta;
    for (int y = v1.v; y <= v3.v; ++y)
    {
        A.v = B.v = y;
        alpha = (y - v1.v) / (float)(longbroundary);
        A.u = v1.u + (v3.u - v1.u) * alpha;
        if (y <= v2.v)
        {
            belta = (y - v1.v) / (float)(shortbroundary);
            B.u = v1.u + (v2.u - v1.u) * belta;
        }
        else
        {
            belta = (y - v2.v) / (float)(middlebroundary);
            B.u = v2.u + (v3.u - v2.u) * belta;
        }

        Drawline2D(A, B, image, color);
    }
}

Vec3f barycentric(Vec2i *pts, const Vec2i &P)
{
    // P(x,y) pts->[A,B,C]
    // use cross dot

    // [_u,_v,_z]
    Vec3f uvVector = Vec3f(pts[1].u - pts[0].u, pts[2].u - pts[0].u, pts[0].u - P.u) ^ Vec3f(pts[1].v - pts[0].v, pts[2].v - pts[0].v, pts[0].v - P.v);

    float u = uvVector.x / uvVector.z; // REAL U
    float v = uvVector.y / uvVector.z; // REAL V
    return Vec3f(1 - u - v, u, v);
}

Vec3f barycentric(Vec3f *pts, const Vec3f &P)
{
    // SAME
    // A - pts[0] B - pts[1] C-pts[2]

    Vec3f A = pts[0], B = pts[1], C = pts[2];
    float denominator = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);

    if (std::abs(denominator) > 1e-6)
    {
        float alpha = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denominator;
        float beta = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denominator;

        return Vec3f(alpha, beta, 1 - alpha - beta);
    }
    else
    {
        // 处理分母为零的情况，例如返回一个特殊值
        return Vec3f(-1, -1, -1); // 或者其他适当的值
    }
}

bool isInTriangle(Vec2i *pts, const Vec2i &P)
{
    Vec3f coefficient = barycentric(pts, P);
    return (coefficient.x >= -0.1 && coefficient.y >= -0.1 && coefficient.z >= -0.1);
}

bool isInTriangle(Vec3f *pts, const Vec3f &P)
{
    Vec3f coefficient = barycentric(pts, P);
    return (coefficient.x >= -0. && coefficient.y >= -0. && coefficient.z >= -0.);
}

void triangle(Vec2i *pts, TGAImage &image, const TGAColor color)
{
    // step1 get boundary box

    Vec2i bboxMin(image.get_width() - 1, image.get_height() - 1);
    Vec2i bboxMax(0, 0);
    bboxMin.x = std::min({bboxMin.x, pts[0].x, pts[1].x, pts[2].x}); // 计算三角形的包围盒
    bboxMin.y = std::min({bboxMin.y, pts[0].y, pts[1].y, pts[2].y});
    bboxMax.x = std::max({bboxMax.x, pts[0].x, pts[1].x, pts[2].x});
    bboxMax.y = std::max({bboxMax.y, pts[0].y, pts[1].y, pts[2].y});

    // loop set
    for (int x = bboxMin.x; x <= bboxMax.x; ++x)
    {
        for (int y = bboxMin.y; y <= bboxMax.y; ++y)
        {
            if (isInTriangle(pts, Vec2i(x, y)))
            {
                image.set(x, y, color);
            }
        }
    }
}

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, Vec2i *texture, Model *model, const float light_ins)
{
    Vec2i bboxMin(image.get_width() - 1, image.get_height() - 1);
    Vec2i bboxMax(0, 0);

    bboxMin.x = std::min({bboxMin.x, static_cast<int>(pts[0].x), static_cast<int>(pts[1].x), static_cast<int>(pts[2].x)});
    bboxMin.y = std::min({bboxMin.y, static_cast<int>(pts[0].y), static_cast<int>(pts[1].y), static_cast<int>(pts[2].y)});
    bboxMax.x = std::max({bboxMax.x, static_cast<int>(pts[0].x), static_cast<int>(pts[1].x), static_cast<int>(pts[2].x)});
    bboxMax.y = std::max({bboxMax.y, static_cast<int>(pts[0].y), static_cast<int>(pts[1].y), static_cast<int>(pts[2].y)});

    for (int x = bboxMin.x; x <= bboxMax.x; ++x)
    {
        for (int y = bboxMin.y; y <= bboxMax.y; ++y)
        {
            Vec3f P(x, y, 0);
            if (isInTriangle(pts, P))
            {
                Vec3f coefficient = barycentric(pts, P);
                P.z = coefficient.x * pts[0].z + coefficient.y * pts[1].z + coefficient.z * pts[2].z; // z_interpolation < 0
                if (P.z > zbuffer[static_cast<int>(x + image.get_width() * y)])
                {
                    zbuffer[static_cast<int>(x + image.get_width() * y)] = P.z; // update
                    Vec2i P_uv = coefficient.x * texture[0] + coefficient.y * texture[1] + coefficient.z * texture[2];

                    TGAColor color = TGAColor(255, 255, 255, 255) * light_ins;
                    color.a = 255.;
                    image.set(P.x, P.y, color);
                }
            }
        }
    }
}

Vec3f NormalOfTriangle(Vec3f *pts)
{

    // 计算两个边向量
    Vec3f edge1 = pts[1] - pts[0];
    Vec3f edge2 = pts[2] - pts[0];

    return (edge2 ^ edge1).normalize();
}
