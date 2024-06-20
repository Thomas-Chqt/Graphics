/*
 * ---------------------------------------------------
 * phongLight.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/19 18:23:07
 * ---------------------------------------------------
 */

#version 410 core

in vec2 texCoord;
in vec3 normal;

out vec4 fragmentColor;

uniform sampler2D u_texture;
uniform vec4 u_diffuseColor;
uniform float u_diffuseIntensity;
uniform vec3 u_diffuseDirection;

void main()
{
    vec4 pixelColor = texture(u_texture, texCoord);
    float diffuseFactor = dot(normalize(normal), -u_diffuseDirection);
    fragmentColor = vec4(pixelColor.xyz * u_diffuseColor.xyz * diffuseFactor * u_diffuseIntensity, pixelColor.w);
}
