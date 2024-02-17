# pragma once
#include "Texture.hpp"

// Vec3f Texture::getColorBilinear(float u, float v)
// {

//     /*回头再写一遍*/

//     // 防止出现 uv 越界错误
//     if (u < 0) u = 0;
//     if (u > 1) u = 1;
//     if (v < 0) v = 0;
//     if (v > 1) v = 1;
//     // 由于在读取图片的时候已经翻转了图片，所以这里不需要再次翻转
//     float x = u * (width - 1);
//     float y = v * (height - 1);
//     int x0 = (int)x;
//     int y0 = (int)y;
//     int x1 = x0 + 1;
//     int y1 = y0 + 1;
//     float u_ratio = x - x0;
//     float v_ratio = y - y0;
//     float u_opposite = 1 - u_ratio;
//     float v_opposite = 1 - v_ratio;
//     TGAColor c00 = image_data.get(x0, y0);
//     TGAColor c01 = image_data.get(x0, y1);
//     TGAColor c10 = image_data.get(x1, y0);
//     TGAColor c11 = image_data.get(x1, y1);
//     Vec3f color = Vec3f(
//         c00.r * u_opposite * v_opposite + c01.r * u_opposite * v_ratio + c10.r * u_ratio * v_opposite + c11.r * u_ratio * v_ratio,
//         c00.g * u_opposite * v_opposite + c01.g * u_opposite * v_ratio + c10.g * u_ratio * v_opposite + c11.g * u_ratio * v_ratio,
//         c00.b * u_opposite * v_opposite + c01.b * u_opposite * v_ratio + c10.b * u_ratio * v_opposite + c11.b * u_ratio * v_ratio
//     );
//     return color;
// }