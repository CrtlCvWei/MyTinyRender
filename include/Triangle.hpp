#pragma once
#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "LineDraw.hpp"
#include "geometry.hpp"
#include "tgaimage.hpp"
#include "model.h"

// class TGAImage;
// struct TGAColor;

void triangle_OldSchool(Vec2i &v1, Vec2i &v2, Vec2i &v3, TGAImage &image, const TGAColor color);

// lesson 2
void triangle(Vec2i *pts, TGAImage &image, const TGAColor color);

// lesson 3 with z-buffer
void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, Vec2i *texture, Model *model,const float light_ins);

Vec3f barycentric(Vec2i *pts, const Vec2i &P);
Vec3f barycentric(Vec3f *pts, const Vec3f &P);

Vec3f NormalOfTriangle(Vec3f *pts);

bool isInTriangle(Vec2i *pts, const Vec2i &P);
bool isInTriangle(Vec3f *pts, const Vec3f &P);

Vec3f barycentric(Vec2i *pts, const Vec2i &P);

#endif
