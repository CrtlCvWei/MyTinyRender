#pragma once
#ifndef _TRIANGLE_HPP
#define _TRIANGLE_HPP

#include "LineDraw.hpp"
#include "geometry.hpp"
#include "tgaimage.hpp"
#include "model.h"

class Model;

class Triangle
{
protected:
    Vec4f pts[3];
    Vec3f normals[3];
    Vec2f uv[3];
    TGAColor color[3];
    // TGAImage &image;

public:
    Triangle();
    Triangle(Triangle &t);
    Triangle(Vec4f *pts, Vec3f *normals, Vec2f *uv, TGAColor *color);
    
    Vec4f& a() { return pts[0]; }
    Vec4f& b() { return pts[1]; }
    Vec4f& c() { return pts[2]; }

    // Get functions for normals
    Vec3f getNormal(int index) const { return normals[index]; }
    Vec3f* getNormals() { return normals; }

    // Get functions for uv
    Vec2f getUV(int index) const { return uv[index]; }
    Vec2f* getUVs() { return uv; }

    // Get functions for color
    TGAColor getColor(int index) const { return color[index]; }
    TGAColor* getColors() { return color; }


    void setVert(int ind, Vec4f &v) {pts[ind] = v;}

    std::array<Vec4f,3> vert() {return {pts[0], pts[1], pts[2]};}


    std::array<Vec3f,3> to_vec3();

};


void triangle_OldSchool(Vec2i &v1, Vec2i &v2, Vec2i &v3, TGAImage &image, const TGAColor color);

// lesson 2
void triangle(Vec2i *pts, TGAImage &image, const TGAColor color);

Vec3f barycentric(Vec2i *pts, const Vec2i &P);
Vec3f barycentric(Vec3f *pts, const Vec3f &P);

Vec3f NormalOfTriangle(Vec3f *pts);

bool isInTriangle(Vec2i *pts, const Vec2i &P);
bool isInTriangle(Vec3f *pts, const Vec3f &P);

// lesson 3 with z-buffer
void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, Vec2i *texture, Model *model, const float light_ins);
#endif
