#pragma once

#include "common.h"
#include "ray.h"

namespace rt 
{


class Interaction;


class Material
{
public:
	virtual ~Material() = default;

	virtual bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const
	{
		return false;
	}
};


class Lambertian : public Material
{
public:
	Lambertian(const Color& albedo) : albedo(albedo) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override
	{
		Vec3 scatter_direction = interaction.normal + RandomUnitVector();

		/* Catch degenerate scatter direction */
		if (NearZero(scatter_direction)) scatter_direction = interaction.normal;

		ray_out = Ray(interaction.posn, scatter_direction);
		attenuation = albedo;
		return true;
	}

private:
	Color albedo;
};


class Metal : public Material
{
public:
	Metal(const Color& albedo, float fuzz) : albedo(albedo), fuzz(fuzz < 1.0f ? fuzz : 1.0f) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override
	{
		Vec3 reflected = Reflect(ray_in.direction, interaction.normal);
		reflected = glm::normalize(reflected) + (fuzz * RandomUnitVector());
		ray_out = Ray(interaction.posn, reflected);
		attenuation = albedo;
		return (glm::dot(ray_out.direction, interaction.normal) > 0.0f); /* Ignore if produced ray direction is within the object */
	}

private:
	Color albedo;
	float fuzz;
};


class Dielectric : public Material
{
public:
	Dielectric(float eta_out, float eta_in) : eta_out(eta_out), eta_in(eta_in) {}
	Dielectric(float eta_in_over_out) : eta_out(1.0f), eta_in(eta_in_over_out) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override
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

		ray_out = Ray(interaction.posn, direction);
		return true;
	}

private:
	float eta_out; /* Refractive index of the enclosing media */
	float eta_in; /* Refractive index of the material */

	static float Reflectance(float cosine, float refraction_index)
	{
		/* Use Schlick's approximation to model reflectance */
		float r0 = (1.0f - refraction_index) / (1.0f + refraction_index);
		r0 = r0 * r0;
		return r0 + (1.0f - r0) * std::pow((1.0f - cosine), 5.0f);
	}
};

} /* namespace rt */
