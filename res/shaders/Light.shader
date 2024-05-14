#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_MVP;

out vec4 VertPosn;
out vec3 VertNormal;

void main()
{
    VertPosn = position;
    VertNormal = normal;

    gl_Position = u_MVP * position;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 VertNormal;
in vec4 VertPosn;

uniform vec4 u_Color;

uniform vec3 u_lightPosn;
uniform vec3 u_lightColor;

uniform vec4 u_cameraPosn;

void main()
{
    float ambient = 0.1f;
    float specularLight = 0.25f;

    vec3 normal = normalize(VertNormal);

    vec3 lightDirection = normalize(u_lightPosn - VertPosn.xyz);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    vec3 viewDirection = normalize(u_cameraPosn.xyz - VertPosn.xyz);
    vec3 reflectionDirection = reflect(lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 100.0);
    float specular = specAmount * specularLight;
    //float specular = 0.0f;

    color = u_Color * vec4(u_lightColor * (diffuse + ambient + specular), 1.0f);
};