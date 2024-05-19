#shader vertex
#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_MVP;

out vec4 v_Normal;
out vec2 v_TexCoord;

void main()
{
    v_Normal = normal;
    v_TexCoord = texCoord;
    gl_Position = u_MVP * position;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec4 v_Normal;
in vec2 v_TexCoord;

void main()
{
    color = v_Normal;
};