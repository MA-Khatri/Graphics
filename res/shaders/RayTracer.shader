#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

void main()
{
    gl_Position = position;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec2 u_ViewportSize;

void main()
{
    float x = 10.0*mod(gl_FragCoord.x, 100.0)/u_ViewportSize.x;
    float y = 10.0*mod(gl_FragCoord.y, 100.0)/u_ViewportSize.y;
    color = vec4(x, y, 0.0f, 1.0f);
};