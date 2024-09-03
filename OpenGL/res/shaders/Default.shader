#shader vertex
#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_ModelNormal;
uniform vec3 u_LightPosition;

out vec4 v_Normal;
out vec2 v_TexCoord;
out vec3 v_Posn;
out vec3 v_LightPosition;

void main()
{
    v_Normal = normalize(u_ModelNormal * normal);
    v_TexCoord = texCoord;
    v_Posn = (u_Model * position).xyz;
    v_LightPosition = u_LightPosition;
    gl_Position = u_MVP * position;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec4 v_Normal;
in vec2 v_TexCoord;
in vec3 v_Posn;
in vec3 v_LightPosition;

void main()
{
    vec3 lightDir = normalize(v_LightPosition - v_Posn);
    float lc = 0.1 + 0.9 * clamp(dot(lightDir, v_Normal.xyz), 0, 1);
    color = vec4(vec3(lc), 1);
};