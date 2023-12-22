#include "Triangle.hpp"
#include "LineDraw.hpp"

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

bool isInTriangle(Vec2i *pts, const Vec2i &P)
{
    Vec3f coefficient = barycentric(pts, P);
    return (coefficient.x >= -0.1 && coefficient.y >= -0.1 && coefficient.z >= -0.1);
}

void triangle(Vec2i *pts, TGAImage &image, const TGAColor color)
{
    // step1 get boundary box

    Vec2i bboxMin(image.get_width() - 1, image.get_height() - 1);
    Vec2i bboxMax(0, 0);
    bboxMin.x = std::min({ bboxMin.x, pts[0].x, pts[1].x, pts[2].x });//计算三角形的包围盒
    bboxMin.y = std::min({ bboxMin.y, pts[0].y, pts[1].y, pts[2].y });
    bboxMax.x = std::max({ bboxMax.x, pts[0].x, pts[1].x, pts[2].x });
    bboxMax.y = std::max({ bboxMax.y, pts[0].y, pts[1].y, pts[2].y });

    // int x_min = std::min(pts[2].x, std::min(pts[0].x, pts[1].x));
    // int x_max = std::max(pts[2].x, std::max(pts[0].x, pts[1].x));
    // int y_min = std::min(pts[2].y, std::min(pts[0].y, pts[1].y));
    // int y_max = std::max(pts[2].y, std::max(pts[0].y, pts[1].y));

    // loop set
    for (int x =bboxMin.x; x <= bboxMax.x; ++x)
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


Vec3f NormalOfTriangle(Vec3f *pts)
{
    Vec3f centroid = (pts[0]+pts[1]+pts[2]) /  3.f;

    // 计算两个边向量
    Vec3f edge1 = pts[1] - pts[0];
    Vec3f edge2 = pts[2] - pts[0];
    
    return (edge2 ^edge1).normalize();
}
   