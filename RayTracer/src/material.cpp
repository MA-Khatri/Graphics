#include "material.h"
#include "interaction.h"

namespace rt
{
/* ======================== */
/* ====== Lambertian ====== */
/* ======================== */

bool Lambertian::Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out, double& pdf) const
{
	Vec3 scatter_direction = RandomCosineDirection(interaction.normal);

	ray_out = interaction.transform.ModelToWorld(Ray(interaction.posn + Eps * interaction.normal, scatter_direction, ray_in.time));
	attenuation = texture->Value(interaction.u, interaction.v, ray_out.origin);
	pdf = glm::dot(interaction.normal, scatter_direction) / Pi;
	return true;
}


double Lambertian::ScatteringPDF(const Ray& ray_in, const Interaction& interaction, const Ray& ray_out) const
{
	return 1.0 / (2.0 * Pi);
}

/* =================== */
/* ====== Metal ====== */
/* =================== */

bool Metal::Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out, double& pdf) const
{
	Ray model_ray = interaction.transform.WorldToModel(ray_in);
	
	Vec3 reflected = Reflect(model_ray.direction, interaction.normal);
	reflected = glm::normalize(reflected);
	if (roughness > 0.0) reflected += roughness * RandomUnitVector();
	
	ray_out = interaction.transform.ModelToWorld(Ray(interaction.posn + Eps * interaction.normal, reflected, ray_in.time));
	
	attenuation = albedo;
	
	return (glm::dot(reflected, interaction.normal) > 0.0); /* Ignore if produced ray direction is within the object */
}

/* ======================== */
/* ====== Dielectric ====== */
/* ======================== */

bool Dielectric::Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out, double& pdf) const
{
	attenuation = Color(1.0, 1.0, 1.0);
	double eta_in_over_out = eta_in / eta_out;
	double refraction_index = interaction.front_face ? (1.0 / eta_in_over_out) : eta_in_over_out;

	Ray model_ray = interaction.transform.WorldToModel(ray_in);
	Vec3 unit_direction = glm::normalize(model_ray.direction);
	double cos_theta = std::fmin(glm::dot(-unit_direction, interaction.normal), 1.0);
	double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

	bool cannot_refract = refraction_index * sin_theta > 1.0;
	Vec3 direction;

	if (cannot_refract || Reflectance(cos_theta, refraction_index) > RandomDouble()) direction = Reflect(unit_direction, interaction.normal);
	else direction = Refract(unit_direction, interaction.normal, refraction_index);

	ray_out = interaction.transform.ModelToWorld(Ray(interaction.posn + Eps * direction, direction, ray_in.time));
	return true;
}

double Dielectric::Reflectance(double cosine, double refraction_index)
{
	double r0 = (1.0 - refraction_index) / (1.0 + refraction_index);
	r0 = r0 * r0;
	return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5.0);
}


/* ======================= */
/* ====== Isotropic ====== */
/* ======================= */

bool Isotropic::Scatter(const Ray& ray_in, const Interaction& interaction, Color& attenuation, Ray& ray_out, double& pdf) const
{
	/* Scatter the ray in a uniform random direction */
	ray_out = Ray(interaction.transform.model_to_world * Vec4(interaction.posn, 1.0), RandomUnitVector(), ray_in.time);
	attenuation = texture->Value(interaction.u, interaction.v, ray_out.origin);
	pdf = 1.0 / (4.0 * Pi); /* I.e., over surface of unit sphere */
	return true;
}


double Isotropic::ScatteringPDF(const Ray& ray_in, const Interaction& interaction, const Ray& ray_out) const
{
	return 1.0 / (4.0 * Pi);
}

/* =========================== */
/* ====== Diffuse Light ====== */
/* =========================== */

Color DiffuseLight::Emitted(const Ray& ray_in, const Interaction& interaction, double u, double v, const Point3& p) const
{
	if (!interaction.front_face) return Color(0.0, 0.0, 0.0); /* No light emitted from back face of light sources */
	return texture->Value(u, v, p);
}

}