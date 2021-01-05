#shader fragment
#version 430 core

layout(location = 0) out vec4 color;

smooth in vec2 v_texCoord;

uniform sampler2D u_texture;

void main()
{
    vec4 texColor = texture(u_texture, v_texCoord);
    color = texColor;//vec4(1.0f, 1.0f, 1.0f, 1.0f);
}