#include "LineDraw.hpp"
#include <iostream>
#include "geometry.hpp"

void Drawline2D_old(const Vector2f &pointA, const Vector2f &pointB, TGAImage &image, const TGAColor color)
{
    // interp from A to B
    /*
    x = x0 + s*  (x1 - x0 )
    y = y0 + s*  (y1 - y0 )
    */

    // two slow
    float x0, y0, x1, y1;
    x0 = pointA.x();
    x1 = pointB.x();
    y0 = pointA.y();
    y1 = pointB.y();

    bool transpose = false;
    if (std::abs(pointB.y() - pointA.y()) > std::abs((pointB.x() - pointA.x())))
    {
        x0 = pointA.y();
        x1 = pointB.y();
        y0 = pointA.x();
        y1 = pointB.x();
        transpose = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (float x = x0; x <= x1; ++x)
    {
        float t = (x - x0) / (float)(x1 - x0);
        float y = y0 * (1. - t) + y1 * t;
        if (transpose)
            if (!image.set(y, x, color))
                std::cout << "Failed to draw the point: (" << x << " , " << y << ") in image!" << std::endl;
            else if (!image.set(x, y, color))
                std::cout << "Failed to draw the point: (" << x << " , " << y << ") in image!" << std::endl;
    }
};

void Drawline2D(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor color)
{
    // Bresenham Draw Lines

    bool transpose = false;
    if (std::abs(x1 - x0) < std::abs(y1 - y0))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        transpose = true;
    }
    if (x1 < x0)
    {
        // make x1 always > x0
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dy = y1 - y0;             // maybe < 0 !
    int dx = x1 - x0;             // >0
    int slope = std::abs(2 * dy); // >0
    int up = 2 * dx;
    int error = 0;
    int y = y0;
    
    for (int x = x0; x <= x1; ++x)
    {
        if (transpose)
        {
            if (!image.set(y, x, color))
                std::cout << "Failed to draw the point: (" << y << " , " << x << ") in image!" << std::endl;
        }
            else
            {
                if (!image.set(x, y, color))
                    std::cout << "Failed to draw the point: (" << x << " , " << y << ") in image!" << std::endl;
            }
        //
        error += slope;
        if (error > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error -= 2 * dx;
        }
    }
};


void Drawline2D(const Vec2i &v1, const Vec2i &v2, TGAImage &image, const TGAColor color)
{
        // Bresenham Draw Lines
    int x0 = v1.u ,y0 = v1.v , x1 = v2.u , y1 = v2.v;

    Drawline2D(x0,y0,x1,y1,image,color);
  
}