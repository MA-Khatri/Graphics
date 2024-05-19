#shader vertex
#version 460 core

layout(location = 0) in vec4 position; // OpenGL will convert the vec2 into a vec4 for us
layout(location = 1) in vec4 color;

uniform mat4 u_MVP;

out vec4 v_Color;

void main()
{
    gl_Position = u_MVP * position;
    v_Color = color;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec4 v_Color;

void main()
{
    color = v_Color;
};