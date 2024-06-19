/*
 * ---------------------------------------------------
 * ambiantLight.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/15 16:26:09
 * ---------------------------------------------------
 */

#version 410 core

in vec2 texCoord;

out vec4 fragmentColor;

uniform sampler2D u_texture;
uniform vec4 u_ambiantColor;
uniform float u_ambiantIntensity;

void main()
{
    vec4 pixelColor = texture(u_texture, texCoord);
    fragmentColor = vec4(pixelColor.xyz * u_ambiantColor.xyz * u_ambiantIntensity, pixelColor.w);
}
