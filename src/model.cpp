#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"
#include "tgaimage.hpp"

Model::Model(const char *filename) : tex(nullptr),nverts_(0),nfaces_(0)
{

    std::vector<Vec4f> verts_;
	std::vector<std::vector<Vec3i>> faces_; // FACE[0] 顶点索引 / FACE[1] 纹理坐标索引 / FACE[2] 法线向量索引
	std::vector<Vec3f> norms_; // 法线向量索引集合
	std::vector<Vec2f> uv_; // 纹理坐标索引集合

    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec4f v;
            for (int i = 0; i < 3; i++)
                iss >> v[i];
            v[3] = 1.0f;
            verts_.push_back(v);
        }
        else if (!line.compare(0, 3, "vt "))
        {
            iss >> trash >> trash;
            Vec2f uv;
            for (int i = 0; i < 2; i++)
                iss >> uv[i];
            uv_.push_back(uv);
        }
        else if (!line.compare(0, 3, "vn "))
        {
            iss >> trash >> trash;
            Vec3f normal;
            for (int i = 0; i < 3; i++)
                iss >> normal[i];
            norms_.push_back(normal);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<Vec3i> f;
            Vec3i tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2])
            {
                for (int i = 0; i < 3; i++)
                    tmp[i]--;
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    
    triangles.resize(faces_.size());
    triangles.clear();

    for(size_t i = 0 ; i < faces_.size(); ++i)
    {
        std::vector<Vec3i> face = faces_[i];

        
        Vec4f world_coords[3];
        Vec3f normal[3];
        Vec2f uv[3];
        TGAColor c[3];

        for(int j = 0; j < 3; j++)
        {

            world_coords[j] = verts_[face[j].x];
            uv[j] = uv_[face[j].y];
            normal[j] = norms_[face[j].z];
            c[j] = TGAColor(255, 255, 255, 255);
        }
        triangles.push_back(Triangle(world_coords,normal,uv,c));
    }

    //record the number of vertices and faces
    nverts_ = static_cast<int>(verts_.size()) ;
    nfaces_ =static_cast<int>(faces_.size());
    
    // load texture
    loadTexture(filename, "_diffuse.tga");
}

Model::~Model()
{
}

int Model::nverts()
{
    return nverts_;
}

int Model::nfaces()
{
    return nfaces_;
}

void Model::loadTexture(std::string filename, const char *suffix)
{
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");

    if (dot != std::string::npos)
    {
        texfile = texfile.substr(0, dot) + std::string(suffix);
        tex = new Texture(texfile);
        std::cerr << "texture file " << texfile << " loading " << ((tex) ? "ok" : "failed") << std::endl;
    }
}

// Vec2i Model::uv(int iface, int nvert)
// {
//     int idx = faces_[iface][nvert][1];
//     return Vec2i(uv_[idx].x * tex->width, uv_[idx].y * tex->height);
// }