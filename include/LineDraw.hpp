#pragma once
#ifndef DRAW_LINE_HPP
#define DRAW_LINE_HPP

#include <Eigen/Dense>
#include "tgaimage.hpp"
#include "geometry.hpp"

using namespace Eigen;

void Drawline2D_old(const Vector2f &pointA, const Vector2f &pointB, TGAImage &image, const TGAColor color);
void Drawline2D(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor color);
void Drawline2D(const Vec2i &v1, const Vec2i &v2, TGAImage &image, const TGAColor color);
//
// void Drawline3D(Vector3f pointA ,Vector3f pointB ,TGAImage &image, TGAColor & color);

#endif