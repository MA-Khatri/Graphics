#shader vertex
#version 460 core

layout(location = 0) in vec4 a_Position;
layout(location = 2) in vec2 a_texCoord;

out vec2 v_texCoord;

void main()
{
    v_texCoord = a_texCoord;
    gl_Position = a_Position;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D u_Diffuse0;

in vec2 v_texCoord;

void main()
{
    color = texture(u_Diffuse0, v_texCoord);
};