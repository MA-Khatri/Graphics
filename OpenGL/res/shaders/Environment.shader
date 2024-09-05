#shader vertex
#version 460 core

layout(location = 0) in vec4 a_Position;

out vec3 v_SampleDir;

uniform mat4 u_MVP; // not used
uniform mat4 u_VP; // view-projection matrix of camera

void main()
{
    v_SampleDir = (a_Position * u_VP).xyz;
    gl_Position = a_Position;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec3 v_SampleDir;

uniform samplerCube u_CubeMap0;

void main()
{
    color = texture(u_CubeMap0, v_SampleDir);
};