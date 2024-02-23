#include "MyShader.hpp"

#include <algorithm> // Include the <algorithm> header for std::max


Vec4f MyShader::vertex_shader(MyShader::VertexShader &payload, MyMtrix::Matrix modelMatrix)
{
    auto temp =( payload.uniform_M * modelMatrix * MyMtrix::toMatrix(payload.position)).to_vec4();
    if(temp.w != 0.0f)
    {
        temp = temp / temp.w;
    }// NDC

    return(payload.ViewPort * MyMtrix::toMatrix(temp)).to_vec4();
}

Vec3f MyShader::normal_fragment_shader(MyShader::FragmentShader &payload)
{
    Vec3f return_color = (payload.normal.normalize() + Vec3f(1.0f, 1.0f, 1.0f)) / 2.f; // [0, 1]
    Vec3f result(return_color.x * 255, return_color.y * 255, return_color.z * 255);
    return result;
}

Vec3f MyShader::flat_shader(MyShader::FragmentShader &payload)
{   
    float ins = payload.normal_fragment.normalize() * payload.lightDir.normalize();
    ins = ins > 0 ? ins : 0;
    Vec3f return_color = payload.color * ins;
    return return_color;
}

Vec3f MyShader::gouraud_shading(FragmentShader &payload)
{
    float intensity = std::max(0.0f, payload.normal.normalize() * payload.lightDir.normalize());
    Vec3f return_color = payload.color * intensity;
    return return_color;
}

Vec3f MyShader::texture_shader(MyShader::FragmentShader &payload)
{
    auto intensity = std::max(0.0f, payload.normal.normalize() * payload.lightDir.normalize());
    auto tex = payload.tex->getColor(payload.uv.x, payload.uv.y);
    auto return_color = Vec3f(tex.r, tex.g, tex.b) * intensity;
    return return_color;
}

Vec3f MyShader::Blinn_Phong(MyShader::FragmentShader &payload)
{
    auto tex = payload.tex->getColor(payload.uv.x, payload.uv.y); 
    auto return_color = Vec3f(tex.r, tex.g, tex.b);

    auto temp_normal = payload.normal_tex->getColor(payload.uv.x, payload.uv.y);
    auto n_vec4 = (payload.uniform_MIT * MyMtrix::toMatrix(Vec4f(temp_normal.r / 255. * 2 - 1,temp_normal.g  / 255. * 2 -1,temp_normal.b  / 255. * 2 -1,0))).to_vec4(); // 
    if(n_vec4.w != 0.0f && n_vec4.w != 1.0f)
    {
        n_vec4 = n_vec4 / n_vec4.w;
    }
    auto n = Vec3f(n_vec4.x, n_vec4.y, n_vec4.z).normalize();

    auto l_vec4 = ( payload.uniform_M *  MyMtrix::toHomoCoordinate(payload.lightDir,1)).to_vec4();
    if(l_vec4.w != 0.0f && l_vec4.w != 1.0f)
    {
        l_vec4 = l_vec4 / l_vec4.w;
    }
    auto l = Vec3f(l_vec4.x, l_vec4.y, l_vec4.z).normalize();

    // diffuse
    auto ins_diffuse = std::max(0.0f, n * l);
    // specular
    auto h = (l + n).normalize();
    auto ins_sp = std::max(0.f, static_cast<float>(std::pow(n * h, 50)) );
    // if(ins_sp > 0.)
    //     std::cout << ins_sp << std::endl;
    // ambient
    auto ins_ambient = 5.f;

    for(size_t i = 0 ; i < 3 ; ++i)
    {
        return_color[i] = std::min(255.f, return_color[i] * (ins_diffuse + ins_sp * 0) + ins_ambient );
    }

    return return_color;
}