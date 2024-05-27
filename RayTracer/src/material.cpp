#include "material.h"
#include "interaction.h"

namespace rt
{

bool Lambertian::Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const
{
	Vec3 scatter_direction = interaction.normal + RandomUnitVector();

	/* Catch degenerate scatter direction */
	if (NearZero(scatter_direction)) scatter_direction = interaction.normal;

	ray_out = Ray(interaction.posn, scatter_direction, ray_in.time);
	attenuation = albedo;
	return true;
}



bool Metal::Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const
{
	Vec3 reflected = Reflect(ray_in.direction, interaction.normal);
	reflected = glm::normalize(reflected) + (roughness * RandomUnitVector());
	ray_out = Ray(interaction.posn, reflected, ray_in.time);
	attenuation = albedo;
	return (glm::dot(ray_out.direction, interaction.normal) > 0.0f); /* Ignore if produced ray direction is within the object */
}

bool Dielectric::Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const
{
	attenuation = Color(1.0f, 1.0f, 1.0f);
	float eta_in_over_out = eta_in / eta_out;
	float refraction_index = interaction.front_face ? (1.0f / eta_in_over_out) : eta_in_over_out;

	Vec3 unit_direction = glm::normalize(ray_in.direction);
	float cos_theta = std::fmin(glm::dot(-unit_direction, interaction.normal), 1.0f);
	float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);

	bool cannot_refract = refraction_index * sin_theta > 1.0f;
	Vec3 direction;

	if (cannot_refract || Reflectance(cos_theta, refraction_index) > RandomFloat()) direction = Reflect(unit_direction, interaction.normal);
	else direction = Refract(unit_direction, interaction.normal, refraction_index);

	ray_out = Ray(interaction.posn, direction, ray_in.time);
	return true;
}

float Dielectric::Reflectance(float cosine, float refraction_index)
{
	float r0 = (1.0f - refraction_index) / (1.0f + refraction_index);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * std::pow((1.0f - cosine), 5.0f);
}

}