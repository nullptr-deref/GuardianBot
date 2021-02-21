#version 430 core

out vec4 color;

smooth in vec2 v_texCoord;

uniform sampler2D u_texture;

void main()
{
    vec4 texColor = texture(u_texture, v_texCoord);
    color = texColor;
}