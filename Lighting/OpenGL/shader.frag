#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
 
uniform sampler2D firstTexture;
uniform sampler2D secondTexture;
uniform float texturesProportion;

void main()
{
    FragColor = mix(texture(firstTexture, TexCoord), texture(secondTexture, TexCoord), 0.5);
}