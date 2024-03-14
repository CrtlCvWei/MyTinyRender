#include "MyShader.hpp"

#include <algorithm> // Include the <algorithm> header for std::max

Vec4f MyShader::vertex_shader(MyShader::VertexShader *payload, MyMtrix::Matrix modelMatrix)
{
    auto temp = (payload->uniform_M * modelMatrix * MyMtrix::toMatrix(payload->position)).to_vec4();
    if (temp.w != 0.0f)
    {
        temp = temp / temp.w;
    } // NDC
    return (payload->ViewPort * MyMtrix::toMatrix(temp)).to_vec4();
}

Vec3f MyShader::normal_fragment_shader(MyShader::FragmentShader *payload)
{

    Vec3f result = Vec3f(255.f, 255.f, 255.f) * payload->normal.norm(); //
    return result;
}

Vec3f MyShader::depth_shading(MyShader::FragmentShader *payload)
{
    if (payload->GetClassName() == MyShader::Deep)
    {
        auto dpayload = dynamic_cast<MyShader::DeepFragmentShader *>(payload);
        if (dpayload->depth_buffer)
        {
            int idx = static_cast<int>(payload->vex.x) + static_cast<int>(payload->vex.y) * dpayload->width;
            if(!dpayload->MSAA)
                dpayload->depth_buffer->at(idx) = payload->vex.z;
            else
                dpayload->depth_buffer->at(payload->MSAA_index + payload->MSAA_sample * idx) = payload->vex.z;
        }
    }
    float deep = std::clamp(payload->vex.z / 255.f, 0.f, 1.f);
    Vec3f return_color = Vec3f(255, 255, 255) * deep;
    return return_color;
}

Vec3f MyShader::flat_shader(MyShader::FragmentShader *payload)
{
    float ins = std::max(0.f, payload->normal_fragment.normalize() * payload->lightDir.normalize());
    Vec3f return_color = payload->color * ins;
    return return_color;
}

Vec3f MyShader::gouraud_shading(FragmentShader *payload)
{
    // 实际上应该在顶点着色器中计算光照，这里为了简化，直接在片元着色器中计算
    float intensity = std::max(0.0f, payload->normal.normalize() * payload->lightDir.normalize());
    Vec3f return_color = payload->color * intensity;
    return return_color;
}

Vec3f MyShader::texture_shader(MyShader::FragmentShader *payload)
{
    auto intensity = std::max(0.0f, payload->normal.normalize() * payload->lightDir.normalize());
    auto tex = payload->tex->getColor(payload->uv.x, payload->uv.y);
    auto return_color = Vec3f(tex.r, tex.g, tex.b) * intensity;
    return return_color;
}

Vec3f MyShader::Blinn_Phong(MyShader::FragmentShader *payload)
{
    float sp_p = 50.f;
    auto tex = payload->tex->getColor(payload->uv.x, payload->uv.y);
    auto return_color = Vec3f(tex.r, tex.g, tex.b);

    auto temp_normal = payload->normal_tex->getNorm(payload->uv.x, payload->uv.y);
    auto n_vec4 = (payload->uniform_MIT * MyMtrix::toMatrix(Vec4f(temp_normal.x, temp_normal.y, temp_normal.z, 0))).to_vec4(); //
    if (n_vec4.w != 0.0f && n_vec4.w != 1.0f)
    {
        n_vec4 = n_vec4 / n_vec4.w;
    }
    auto n = Vec3f(n_vec4.x, n_vec4.y, n_vec4.z).normalize();

    if (payload->specular_map.has_value())
    {
        sp_p = payload->specular_map.value()->getSpec(payload->uv.x, payload->uv.y);
    }

    auto l_vec4 = (payload->uniform_M * MyMtrix::toHomoCoordinate(payload->lightDir, 1)).to_vec4();
    if (l_vec4.w != 0.0f && l_vec4.w != 1.0f)
    {
        l_vec4 = l_vec4 / l_vec4.w;
    }
    auto l = Vec3f(l_vec4.x, l_vec4.y, l_vec4.z).normalize();

    // diffuse
    auto ins_diffuse = std::max(0.0f, n * l);
    // specular
    auto h = (l + n).normalize();
    auto ins_sp = std::max(0.f, static_cast<float>(std::pow(n * h, sp_p)));

    // ambient
    auto ins_ambient = 20.f;

    for (size_t i = 0; i < 3; ++i)
    {
        return_color[i] = std::min(255.f, return_color[i] * (1.2f * ins_diffuse + ins_sp * .6f) + ins_ambient);
    }

    return return_color;
}

Vec3f MyShader::Blinn_Phong_With_ShadowMapping(MyShader::FragmentShader *payload)
{
    float shadow = 0;
    
    MyShader::DeepFragmentShader *dpayload = dynamic_cast<MyShader::DeepFragmentShader *>(payload);
    MyMtrix::Matrix sb_p_matrix = dpayload->uniform_Mshadow * MyMtrix::toHomoCoordinate(payload->vex, 1.f);
    auto sb_vec4 = (sb_p_matrix / sb_p_matrix[3][0]).to_vec4();
    // index in the shadowbuffer array
    int idx = int(sb_vec4[0]) + dpayload->width * int(sb_vec4[1]) ;
    idx = payload->MSAA ?  payload->MSAA_sample * idx + payload->MSAA_index : idx;

    if (idx > dpayload->depth_buffer->size() - 1)
    {
        shadow = 1;
    }
    else
    {
        shadow = .3 + .7 * (dpayload->depth_buffer->at(idx) < sb_vec4[2] +43.34f); //  a magic number
    }
    auto return_color = MyShader::Blinn_Phong(payload);
    for (size_t i = 0; i < 3; ++i)
    {
        return_color[i] = std::clamp(return_color[i] * shadow, 0.f, 255.f);
    }
    return return_color;
}