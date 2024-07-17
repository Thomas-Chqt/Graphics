/*
 * ---------------------------------------------------
 * skybox.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/16 17:51:31
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec3 pos;

out vec3 uv;

layout (std140) uniform vpMatrix
{
    mat4 vpMatrix_data;
};

void main()
{
    gl_Position = (vpMatrix_data * vec4(pos, 1.0)).xyww;
    uv = pos;
}