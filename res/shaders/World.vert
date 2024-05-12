#version 330 core

layout(location = 0) in vec4 position; // OpenGL will convert the vec2 into a vec4 for us
layout(location = 1) in vec4 color;

uniform mat4 u_VP;

out vec4 v_Color;

void main()
{
    gl_Position = u_VP * position;
    v_Color = color;
};