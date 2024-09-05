#shader vertex
#version 460 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Normal;

out vec3 v_Normal;
out vec3 v_Position;

uniform mat4 u_MVP; 
uniform mat4 u_VP; // view-projection matrix of camera

void main()
{
    v_Normal = a_Normal.xyz;
    vec4 tPosn = u_MVP * a_Position;
    v_Position = tPosn.xyz;
    gl_Position = tPosn;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec3 v_Position;

uniform samplerCube u_CubeMap0;

uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPosition;

void main()
{
    vec3 eyeDir = normalize(u_CameraPosition - v_Position);
    vec3 reflectedDir = -reflect(eyeDir, v_Normal);
    color = texture(u_CubeMap0, mat3(1, 0, 0, 0, 0, 1, 0, -1, 0) * reflectedDir);
    //color = texture(u_CubeMap0, reflectedDir);
};