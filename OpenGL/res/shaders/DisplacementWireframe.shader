#shader vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_ModelNormal;

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1);
};


#shader tcs
#version 460 core

layout (vertices = 4) out;

void main()
{
    float level = 100.0;
    
    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
    gl_TessLevelOuter[3] = level;
    
    gl_TessLevelInner[0] = level;
    gl_TessLevelInner[1] = level;
    
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}


#shader tes
#version 460 core

layout (quads, equal_spacing, ccw) in;

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
{
    vec4 a = mix(v0, v1, gl_TessCoord.x);
    vec4 b = mix(v3, v2, gl_TessCoord.x);
    return mix(a, b, gl_TessCoord.y);
}

void main()
{
    gl_Position = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
}


#shader geometry
#version 460 core 

layout ( triangles ) in;
layout ( line_strip, max_vertices = 4 ) out;

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(-cross(a, b));
}

void main()
{
    vec3 normal = GetNormal();
    float offset = 0.01;
    
    
    gl_Position = gl_in[0].gl_Position + offset * vec4(normal, 0);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + offset * vec4(normal, 0);
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + offset * vec4(normal, 0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + offset * vec4(normal, 0);
    EmitVertex();
    
    EndPrimitive(); // not neccessary here, just here for future reference
}


#shader fragment
#version 460 core

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(1, 0, 1, 1);
}