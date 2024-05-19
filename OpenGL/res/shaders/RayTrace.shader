#shader vertex
#version 460 core

layout(location = 0) in vec4 position;

void main()
{
    gl_Position = position;
};


#shader fragment
#version 460 core

layout(location = 0) out vec4 FragColor;

uniform vec2 u_ViewportSize;
uniform mat4 u_CameraMatrix;

void main()
{
    vec4 color = vec4(0.075, 0.133, 0.173, 1.0);
    
    vec2 viewport_posn = (gl_FragCoord.xy - u_ViewportSize) / u_ViewportSize;
    
    float fov = 45.0;
    vec4 cam_o = vec4(-tan(fov / 2.0), 0.0, 0.0, 1.0);
    vec4 ray_o = vec4(0.0, viewport_posn, 1.0);
    vec4 ray_d = normalize(ray_o - cam_o);
    
    vec3 sphere_c = vec3(-2.0, 0.0, 0.0);
    float sphere_r = 1.0;
    
    vec3 o_c = ray_o.xyz - sphere_c;
    float b = dot(ray_d.xyz, o_c);
    float c = dot(o_c, o_c) - sphere_r * sphere_r;
    float intersectionState = b * b - c;
    vec3 intersection = ray_o.xyz + ray_d.xyz * (-b + sqrt(b * b - c));
    
    if (intersectionState >= 0.0)
    {
        color = vec4((normalize(intersection - sphere_c) + 1.0) / 2.0, 1.0);
    }
    
    FragColor = color;
    //FragColor = vec4(viewport_posn, 0.0, 1.0);
};