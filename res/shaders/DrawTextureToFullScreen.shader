#shader vertex
#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 UV;

void main()
{
    UV = texCoord;
    gl_Position = position;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D u_screen;

in vec2 UV;

void main()
{
    color = texture(u_screen, UV);
};