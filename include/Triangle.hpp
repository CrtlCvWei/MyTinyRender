#pragma once
#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "LineDraw.hpp"
#include "geometry.hpp"
#include "tgaimage.hpp"

// class TGAImage;
// struct TGAColor;

void triangle_OldSchool(Vec2i &v1, Vec2i &v2, Vec2i &v3, TGAImage &image, const TGAColor color);

void triangle(Vec2i *pts,TGAImage &image, const TGAColor color);

Vec3f barycentric(Vec2i *pts, const Vec2i &P);

Vec3f NormalOfTriangle(Vec3f *pts);

bool isInTriangle(Vec2i *pts, const Vec2i &P);

Vec3f barycentric(Vec2i *pts, const Vec2i &P);

#endif
