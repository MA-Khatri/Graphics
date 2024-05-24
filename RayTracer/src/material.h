#pragma once

#include "common.h"
#include "ray.h"

namespace rt {


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
	Metal(const Color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override
	{
		Vec3 reflected = Reflect(ray_in.direction, interaction.normal);
		reflected = Normalize(reflected) + (fuzz * RandomUnitVector());
		ray_out = Ray(interaction.posn, reflected);
		attenuation = albedo;
		return (Dot(ray_out.direction, interaction.normal) > 0); /* Ignore if produced ray direction is within the object */
	}

private:
	Color albedo;
	double fuzz;
};


class Dielectric : public Material
{
public:
	Dielectric(double eta_out, double eta_in) : eta_out(eta_out), eta_in(eta_in) {}
	Dielectric(double eta_in_over_out) : eta_out(1.0), eta_in(eta_in_over_out) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override
	{
		attenuation = Color(1.0, 1.0, 1.0);
		double eta_in_over_out = eta_in / eta_out;
		double refraction_index = interaction.front_face ? (1.0 / eta_in_over_out) : eta_in_over_out;

		Vec3 unit_direction = Normalize(ray_in.direction);
		double cos_theta = std::fmin(Dot(-unit_direction, interaction.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = refraction_index * sin_theta > 1.0;
		Vec3 direction;

		if (cannot_refract || Reflectance(cos_theta, refraction_index) > RandomDouble()) direction = Reflect(unit_direction, interaction.normal);
		else direction = Refract(unit_direction, interaction.normal, refraction_index);

		ray_out = Ray(interaction.posn, direction);
		return true;
	}

private:
	double eta_out; /* Refractive index of the enclosing media */
	double eta_in; /* Refractive index of the material */

	static double Reflectance(double cosine, double refraction_index)
	{
		/* Use Schlick's approximation to model reflectance */
		double r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};

} /* namespace RT */
