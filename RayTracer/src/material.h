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
	Metal(const Color& albedo, double roughness) : albedo(albedo), roughness(roughness < 1.0 ? roughness : 1.0) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override;

private:
	Color albedo;
	double roughness; /* "Fuzziness" of the reflection -- 0 = perfect reflection, 1 = diffuse */
};


class Dielectric : public Material
{
public:
	Dielectric(double eta_out, double eta_in) : eta_out(eta_out), eta_in(eta_in) {}
	Dielectric(double eta_in_over_out) : eta_out(1.0), eta_in(eta_in_over_out) {}

	bool Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out) const override;

private:
	double eta_out; /* Refractive index of the enclosing media */
	double eta_in; /* Refractive index of the material */

	/* Use Schlick's approximation to model reflectance */
	static double Reflectance(double cosine, double refraction_index);
};

} /* namespace rt */
