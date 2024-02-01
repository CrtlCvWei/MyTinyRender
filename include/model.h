#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.hpp"
#include "tgaimage.hpp"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<Vec3i>> faces_; // FACE[0] 顶点索引 / FACE[1] 纹理坐标索引 / FACE[2] 法线向量索引
	std::vector<Vec3f> norms_; // 发现向量索引集合
	std::vector<Vec2f> uv_; // 纹理坐标索引集合
	TGAImage diffuseMap_;
	void loadTexture(std::string filename, const char* suffix, TGAImage& image);
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
	Vec2i uv(int iface, int nvert);
	TGAColor diffuse(Vec2i uv);
};

#endif //__MODEL_H__