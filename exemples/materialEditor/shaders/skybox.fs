/*
 * ---------------------------------------------------
 * skybox.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/16 17:51:57
 * ---------------------------------------------------
 */

#version 410 core

in vec3 uv;

out vec4 fragmentColor;

uniform samplerCube skyboxTexture;

void main()
{
    fragmentColor = texture(skyboxTexture, uv);
}