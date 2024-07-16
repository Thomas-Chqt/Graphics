/*
 * ---------------------------------------------------
 * skybox.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/16 17:51:31
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec4 pos;

out vec3 uv;

layout (std140) uniform matrices
{
    mat4 vpMatrix;
};

void main()
{
    uv = pos.xyz;
    gl_Position = vpMatrix * vec4(pos.xyz, 1.0);
}