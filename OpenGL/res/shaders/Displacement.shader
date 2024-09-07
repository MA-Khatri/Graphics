#shader vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

//uniform mat4 u_MVP;
//uniform mat4 u_Model;
//uniform mat4 u_ModelNormal;
//uniform mat4 u_MatrixShadow;

//out vec3 v_Position;
//out vec3 v_Normal;
//out vec2 v_TexCoord;
//out vec4 v_LightViewPosition;

out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(a_Position, 1);
	
	v_TexCoord = a_TexCoord;
};


#shader tcs
#version 460 core

layout (vertices = 4) out;

uniform float u_TessLevel;

in vec2 v_TexCoord[];

out vec2 TexCoord[];

void main()
{
	gl_TessLevelOuter[0] = u_TessLevel;
	gl_TessLevelOuter[1] = u_TessLevel;
	gl_TessLevelOuter[2] = u_TessLevel;
	gl_TessLevelOuter[3] = u_TessLevel;
	
	gl_TessLevelInner[0] = u_TessLevel;
	gl_TessLevelInner[1] = u_TessLevel;
	
	// Pass through attributes
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	TexCoord[gl_InvocationID] = v_TexCoord[gl_InvocationID];
}


#shader tes
#version 460 core

layout (quads, fractional_even_spacing, ccw) in;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_MatrixShadow;

uniform sampler2D u_DisplacementMap0;
uniform float u_Height;

in vec2 TexCoord[];

out vec3 v_Position;
out vec2 v_TexCoord;
out vec4 v_LightViewPosition;


vec4 interpolate(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
{
	vec4 a = mix(v0, v1, gl_TessCoord.x);
	vec4 b = mix(v3, v2, gl_TessCoord.x);
	return mix(a, b, gl_TessCoord.y);
}

vec2 interpolate(vec2 v0, vec2 v1, vec2 v2, vec2 v3)
{
	vec2 a = mix(v0, v1, gl_TessCoord.x);
	vec2 b = mix(v3, v2, gl_TessCoord.x);
	return mix(a, b, gl_TessCoord.y);
}

void main()
{
	vec2 texCoord = interpolate(TexCoord[0], TexCoord[1], TexCoord[2], TexCoord[3]);
	float displacement = texture(u_DisplacementMap0, texCoord).x;
	
	vec4 posn = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
	posn += vec4(0, 0, u_Height * displacement, 0);
	gl_Position = u_MVP * posn;
	
	// Other calculations to pass through...
	v_Position = (u_Model * posn).xyz;
	v_TexCoord = texCoord;
	v_LightViewPosition = u_MatrixShadow * u_Model * posn;

}


#shader fragment
#version 460 core

layout(location = 0) out vec4 FragColor;

uniform vec3 u_CameraPosition;
uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform int  u_LightMode; // Lighting mode: 0 = directional, 1 = point, 2 = spot/cone

uniform sampler2DShadow u_ShadowMap0;

uniform sampler2D u_Diffuse0;
uniform sampler2D u_Specular0;
uniform sampler2D u_NormalMap0;

in vec3 v_Position;
in vec2 v_TexCoord;
in vec4 v_LightViewPosition;


vec4 directionalLight(vec3 normal)
{
	// ambient lighting
	float ambient = 0.2f;

	// diffuse lighting
	vec3 lightDirection = normalize(u_LightPosition); // direction from light position to world origin 
	float diffuse = 0.8f * max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(u_CameraPosition - v_Position);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 100);
	float specular = specAmount * specularLight;

	return vec4((texture(u_Diffuse0, v_TexCoord) * (diffuse + ambient) + texture(u_Specular0, v_TexCoord).r * specular).rgb * u_LightColor, 1);
}

vec4 pointLight(vec3 normal)
{
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = u_LightPosition - v_Position;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 0.0; // original = 3.0
	float b = 0.1; // original = 0.7
	float inten = 1.0f / (a * (dist * dist) + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(u_CameraPosition - v_Position);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return vec4((texture(u_Diffuse0, v_TexCoord) * (diffuse * inten + ambient) + texture(u_Specular0, v_TexCoord).r * specular * inten).rgb * u_LightColor, 1);
}

vec4 spotLight(vec3 normal)
{
	// controls how big the area that is lit up is
	float outerCone = 0.50f;
	float innerCone = 0.85f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 lightDirection = normalize(u_LightPosition - v_Position);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(u_CameraPosition - v_Position);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the v_Position based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, 0.0f, -1.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return vec4((texture(u_Diffuse0, v_TexCoord) * (diffuse * inten + ambient) + texture(u_Specular0, v_TexCoord).r * specular * inten).rgb * u_LightColor, 1);
}


void main()
{
	// Obtain normal map in range [0,1]
	vec3 normal = texture(u_NormalMap0, v_TexCoord).xyz;
	
	// transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);
	
	vec3 lightDir = normalize(u_LightPosition - v_Position);
	float ldotn = dot(lightDir, normal);
	
	if (u_LightMode == 0)
		FragColor = directionalLight(normal);
	else if (u_LightMode == 1)
		FragColor = pointLight(normal);
	else if (u_LightMode == 2)
		FragColor = spotLight(normal);
	else
	{// Default shading
		float lc = 0.1 + 0.9 * clamp(ldotn, 0, 1);
		FragColor = vec4(vec3(lc), 1);
	}

	// Compute shadow contribution (shadowScale factor)
	float projLightViewZ = v_LightViewPosition.z / v_LightViewPosition.w;
	float shadowScale = 1.0; // if outside view frustum, no shadow
	if (projLightViewZ < 1.0 && ldotn > 0.0)
		shadowScale = textureProj(u_ShadowMap0, v_LightViewPosition);
	FragColor *= vec4(vec3(clamp(shadowScale, 0.5f, 1.0f)), 1);
};