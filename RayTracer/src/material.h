#pragma once

#include "common.h"
#include "ray.h"

namespace RayTracer {

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
		if (scatter_direction.NearZero()) scatter_direction = interaction.normal;

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
	Metal(const Color& albedo) : albedo(albedo) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override
	{
		Vec3 reflected = Reflect(ray_in.direction, interaction.normal);
		ray_out = Ray(interaction.posn, reflected);
		attenuation = albedo;
		return true;
	}

private:
	Color albedo;
};

} /* namespace RayTracer */
