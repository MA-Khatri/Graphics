#pragma once

#include "common.h"
#include "ray.h"

namespace rt 
{

/* Forward declaration of the Interaction class */
class Interaction;


class Material
{
public:
	virtual ~Material() = default;

	/* Update ray_out with the appropriate scatter function for this material */
	virtual bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const
	{
		return false;
	}
};


class Lambertian : public Material
{
public:
	Lambertian(const Color& albedo) : albedo(albedo) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override;

private:
	Color albedo;
};


class Metal : public Material
{
public:
	Metal(const Color& albedo, float roughness) : albedo(albedo), roughness(roughness < 1.0f ? roughness : 1.0f) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override;

private:
	Color albedo;
	float roughness; /* "Fuzziness" of the reflection -- 0 = perfect reflection, 1 = diffuse */
};


class Dielectric : public Material
{
public:
	Dielectric(float eta_out, float eta_in) : eta_out(eta_out), eta_in(eta_in) {}
	Dielectric(float eta_in_over_out) : eta_out(1.0f), eta_in(eta_in_over_out) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override;

private:
	float eta_out; /* Refractive index of the enclosing media */
	float eta_in; /* Refractive index of the material */

	/* Use Schlick's approximation to model reflectance */
	static float Reflectance(float cosine, float refraction_index);
};

} /* namespace rt */
