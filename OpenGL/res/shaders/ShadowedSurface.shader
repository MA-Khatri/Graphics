#shader vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_ModelNormal;
uniform mat4 u_MatrixShadow;

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec4 v_LightViewPosition;


void main()
{
	gl_Position = u_MVP * vec4(a_Position, 1);
	v_LightViewPosition = u_MatrixShadow * u_Model * vec4(a_Position, 1);
	v_Normal = normalize((u_ModelNormal * vec4(a_Normal, 0)).xyz);
	v_TexCoord = a_TexCoord;
	v_Position = (u_Model * vec4(a_Position, 1)).xyz;
};


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

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec4 v_LightViewPosition;


vec4 directionalLight()
{
	// ambient lighting
	float ambient = 0.2f;

	// diffuse lighting
	vec3 normal = normalize(v_Normal);
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

vec4 pointLight()
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
	vec3 normal = normalize(v_Normal);
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

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.50f;
	float innerCone = 0.85f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(v_Normal);
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
	vec3 lightDir = normalize(u_LightPosition - v_Position);
	float ldotn = dot(lightDir, v_Normal);
	
	if (u_LightMode == 0)
		FragColor = directionalLight();
	else if (u_LightMode == 1)
		FragColor = pointLight();
	else if (u_LightMode == 2)
		FragColor = spotLight();
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