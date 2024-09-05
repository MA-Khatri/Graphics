#shader vertex
#version 460 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_ModelNormal;
uniform mat4 u_MatrixShadow;

out vec4 v_LightViewPosition;
out vec4 v_Normal;
out vec2 v_TexCoord;
out vec3 v_Posn;


void main()
{
    gl_Position = u_MVP * a_Position;
    v_LightViewPosition = u_MatrixShadow * u_Model * a_Position;
    v_Normal = normalize(u_ModelNormal * a_Normal);
    v_TexCoord = a_TexCoord;
    v_Posn = (u_Model * a_Position).xyz;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

uniform vec3 u_CameraPosition;
uniform vec3 u_LightPosition;

uniform sampler2DShadow u_ShadowMap0;

in vec4 v_LightViewPosition;
in vec4 v_Normal;
in vec2 v_TexCoord;
in vec3 v_Posn;

void main()
{
    vec3 lightDir = normalize(u_LightPosition - v_Posn);
    float lc = 0.1 + 0.9 * clamp(dot(lightDir, v_Normal.xyz), 0, 1);
    color = vec4(vec3(lc), 1);
    
    vec3 projLightViewCoords = v_LightViewPosition.xyz / v_LightViewPosition.w;
    //float currentDepth = projLightViewCoords.z;
    //float closestDepth = texture(u_ShadowMap0, projLightViewCoords.xy).r;
    //float bias = clamp(0.005 * tan(acos(dot(v_Normal.xyz, lightDir))), 0.0, 0.01); // light-angle dependent bias
    //float shadowScale = currentDepth - bias < closestDepth ? 1 : 0.3; // the inequality should be reversed...?
    
    float shadowScale = textureProj(u_ShadowMap0, v_LightViewPosition);
    
    if (projLightViewCoords.z > 1.0) shadowScale = 1.0; // if outside view frustum, no shadow
    
    color *= vec4(vec3(shadowScale), 1.0);
};