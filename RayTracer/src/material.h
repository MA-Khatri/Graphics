#pragma once

#include "common.h"
#include "ray.h"
#include "texture.h"

namespace rt 
{

/* Forward declaration of the HitRecord and PDF classes */
class HitRecord;
class PDF;

class ScatterRecord
{
public:
	Color attenuation;
	std::shared_ptr<PDF> pdf_ptr;
	bool skip_pdf;
	Ray skip_pdf_ray;
};

class Material
{
public:
	virtual ~Material() = default;

	virtual Color Emitted(const Ray& ray_in, const HitRecord& hrec) const
	{
		return Color(0.0, 0.0, 0.0);
	}

	/* Update ray_out with the appropriate scatter function for this material */
	virtual bool Scatter(const Ray& ray_in, const HitRecord& hrec, ScatterRecord& srec) const
	{
		return false;
	}

	/* Sample a probability distribution */
	virtual double ScatteringPDF(const Ray& ray_in, const HitRecord& hrec, const Ray& ray_out) const
	{
		return 0.0;
	}
};


class Lambertian : public Material
{
public:
	Lambertian(const Color& albedo) : texture(std::make_shared<SolidColor>(albedo)) {}
	Lambertian(std::shared_ptr<Texture> texture) : texture(texture) {}

	bool Scatter(const Ray& ray_in, const HitRecord& hrec, ScatterRecord& srec) const override;

	double ScatteringPDF(const Ray& ray_in, const HitRecord& hrec, const Ray& ray_out) const override;

private:
	std::shared_ptr<Texture> texture;
};


class Metal : public Material
{
public:
	Metal(const Color& albedo, double roughness) : albedo(albedo), roughness(roughness < 1.0 ? roughness : 1.0) {}

	bool Scatter(const Ray& ray_in, const HitRecord& hrec, ScatterRecord& srec) const override;

private:
	Color albedo;
	double roughness; /* "Fuzziness" of the reflection -- 0 = perfect reflection, 1 = diffuse */
};


class Dielectric : public Material
{
public:
	Dielectric(double eta_out, double eta_in) : eta_out(eta_out), eta_in(eta_in) {}
	Dielectric(double eta_in_over_out) : eta_out(1.0), eta_in(eta_in_over_out) {}

	bool Scatter(const Ray& ray_in, const HitRecord& hrec, ScatterRecord& srec) const override;

private:
	double eta_out; /* Refractive index of the enclosing media */
	double eta_in; /* Refractive index of the material */

	/* Use Schlick's approximation to model reflectance */
	static double Reflectance(double cosine, double refraction_index);
};


class Isotropic : public Material
{
public:
	Isotropic(const Color& albedo) : texture(std::make_shared<SolidColor>(albedo)) {}
	Isotropic(std::shared_ptr<Texture> texture) : texture(texture) {}

	bool Scatter(const Ray& ray_in, const HitRecord& hrec, ScatterRecord& srec) const override;

	double ScatteringPDF(const Ray& ray_in, const HitRecord& hrec, const Ray& ray_out) const override;

private:
	std::shared_ptr<Texture> texture;
};


class DiffuseLight : public Material
{
public:
	DiffuseLight(std::shared_ptr<Texture> texture) : texture(texture) {}
	DiffuseLight(const Color& emit) : texture(std::make_shared<SolidColor>(emit)) {}

	Color Emitted(const Ray& ray_in, const HitRecord& hrec) const override;

private:
	std::shared_ptr<Texture> texture;
};

} /* namespace rt */
