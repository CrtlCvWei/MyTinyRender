#pragma once
#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.hpp"
#include "tgaimage.hpp"
#include "Texture.hpp"
#include "Triangle.hpp"


class Triangle;

class Model {
private:
	
	int nverts_, nfaces_;

	Texture* tex;

	std::vector<Triangle> triangles;

	void loadTexture(std::string filename, const char* suffix);
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	std::vector<Triangle> getTriangles() { return triangles; }
	// Vec3f vert(int i);
	// std::vector<int> face(int idx);
	// Vec2i uv(int iface, int nvert);

};

#endif //__MODEL_H__