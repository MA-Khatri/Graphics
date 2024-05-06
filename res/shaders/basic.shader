#shader vertex
#version 330 core

layout(location = 0) in vec4 position; // OpenGL will convert the vec2 into a vec4 for us

void main()
{
   gl_Position = position;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
   color = u_Color;
};