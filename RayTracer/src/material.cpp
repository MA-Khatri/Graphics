#include "material.h"
#include "hit_record.h"
#include "pdf.h"

namespace rt
{
/* ======================== */
/* ====== Lambertian ====== */
/* ======================== */

bool Lambertian::Scatter(const Ray& ray_in, const HitRecord& hrec, ScatterRecord& srec) const
{
	srec.attenuation = texture->Value(hrec.u, hrec.v, hrec.posn);
	srec.pdf_ptr = std::make_shared<CosinePDF>(hrec.normal);
	srec.skip_pdf = false;
	return true;
}


double Lambertian::ScatteringPDF(const Ray& ray_in, const HitRecord& hrec, const Ray& ray_out) const
{
	Vec3 world_normal = glm::normalize(hrec.transform.GetWorldNormal(hrec.normal));
	double cosine = glm::dot(world_normal, glm::normalize(ray_out.direction));
	return cosine < 0.0 ? 0.0 : cosine / Pi;
}

/* =================== */
/* ====== Metal ====== */
/* =================== */

bool Metal::Scatter(const Ray& ray_in, const HitRecord& hrec, ScatterRecord& srec) const
{
	Ray model_ray = hrec.transform.WorldToModel(ray_in);
	
	Vec3 reflected = Reflect(model_ray.direction, hrec.normal);
	reflected = glm::normalize(reflected);
	if (roughness > 0.0) reflected += roughness * RandomUnitVector();
	
	srec.attenuation = albedo;
	srec.pdf_ptr = nullptr;
	srec.skip_pdf = true;
	srec.skip_pdf_ray = hrec.transform.ModelToWorld(Ray(hrec.posn + Eps * hrec.normal, reflected, ray_in.time));
		
	return true;
}

/* ======================== */
/* ====== Dielectric ====== */
/* ======================== */

bool Dielectric::Scatter(const Ray& ray_in, const HitRecord& hrec, ScatterRecord& srec) const
{
	srec.attenuation = Color(1.0, 1.0, 1.0);
	srec.pdf_ptr = nullptr;
	srec.skip_pdf = true;

	double eta_in_over_out = eta_in / eta_out;
	double refraction_index = hrec.front_face ? (1.0 / eta_in_over_out) : eta_in_over_out;

	Ray model_ray = hrec.transform.WorldToModel(ray_in);
	Vec3 unit_direction = glm::normalize(model_ray.direction);
	double cos_theta = std::fmin(glm::dot(-unit_direction, hrec.normal), 1.0);
	double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

	bool cannot_refract = refraction_index * sin_theta > 1.0;
	Vec3 direction;

	if (cannot_refract || Reflectance(cos_theta, refraction_index) > RandomDouble()) direction = Reflect(unit_direction, hrec.normal);
	else direction = Refract(unit_direction, hrec.normal, refraction_index);

	srec.skip_pdf_ray = hrec.transform.ModelToWorld(Ray(hrec.posn + Eps * direction, direction, ray_in.time));
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

bool Isotropic::Scatter(const Ray& ray_in, const HitRecord& hrec, ScatterRecord& srec) const
{
	srec.attenuation = texture->Value(hrec.u, hrec.v, hrec.posn);
	srec.pdf_ptr = std::make_shared<SpherePDF>();
	srec.skip_pdf = false;
	return true;
}


double Isotropic::ScatteringPDF(const Ray& ray_in, const HitRecord& hrec, const Ray& ray_out) const
{
	return 1.0 / (4.0 * Pi);
}

/* =========================== */
/* ====== Diffuse Light ====== */
/* =========================== */

Color DiffuseLight::Emitted(const Ray& ray_in, const HitRecord& hrec) const
{
	if (!hrec.front_face) return Color(0.0, 0.0, 0.0); /* No light emitted from back face of light sources */
	return texture->Value(hrec.u, hrec.v, hrec.posn);
}

}