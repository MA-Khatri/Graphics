#shader vertex
#version 460 core

layout(location = 0) in vec4 position;

out vec3 dir;

uniform mat4 u_iVP; // inverse of view-projection matrix of camera

void main()
{
    dir = (position * u_iVP).xyz;
    gl_Position = position;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec3 dir;

uniform samplerCube u_CubeMap0;

void main()
{
    color = texture(u_CubeMap0, dir);
};