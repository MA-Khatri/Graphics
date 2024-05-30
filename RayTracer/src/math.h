#pragma once

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/epsilon.hpp"

#include "common.h"

/* Custom wrapper over glm */

namespace rt
{

/* ============================== */
/* === Accessor redefinitions === */
/* ============================== */

using Vec2 = glm::dvec2;
using Point2 = glm::dvec2;
using Vec3 = glm::dvec3;
using Point3 = glm::dvec3;
using Color = glm::dvec3;
using Vec4 = glm::dvec4;
using Mat4 = glm::dmat4;


/* ================= */
/* === GLM Utils === */
/* ================= */

/* Returns true if all element-wise components of provided VecXs are equal */
template <typename T>
inline bool Equal(T u, T v)
{
	return glm::all(glm::equal(u, v));
}

/* Returns true if all components of VecX are close to 0 */
template <typename T>
inline bool NearZero(T v)
{
	return glm::all(glm::epsilonEqual(v, T(0.0), Eps));
}

/* Generates a Vec3 with all components between 0.0 and 1.0 */
inline Vec3 RandomVec3()
{
	return Vec3(RandomDouble(), RandomDouble(), RandomDouble());
}

/* Generates a Vec3 with all components between provided min and max values */
inline Vec3 RandomVec3(float min, float max)
{
	return Vec3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
}

/* Generates a Vec2 with length < 1.0 */
inline Vec2 RandomInUnitDisk()
{
	while (true)
	{
		Vec2 p = Vec2(RandomDouble(-1.0, 1.0), RandomDouble(-1.0, 1.0));
		if (glm::length2(p) < 1.0) return p;
	}
}

/* Generate a random Vec3 with a length < 1.0 */
inline Vec3 RandomInUnitSphere()
{
	while (true)
	{
		auto p = RandomVec3(-1.0, 1.0);
		if (glm::length2(p) < 1) return p;
	}
}

/* Generate a random Vec3 with unit length */
inline Vec3 RandomUnitVector()
{
	return glm::normalize(RandomInUnitSphere());
}

/* Generates a random unit Vec3 that lies in the same hemisphere as the provided normal */
inline Vec3 RandomOnHemisphere(const Vec3& normal)
{
	Vec3 on_unit_sphere = RandomUnitVector();

	/* In the same hemisphere as the normal? */
	if (glm::dot(on_unit_sphere, normal) > 0.0) return on_unit_sphere;
	else return -on_unit_sphere;
}


/* Returns a random point in the [-0.5, -0.5] to [0.5, 0.5] unit square */
inline Point2 SampleSquare()
{
	return Point2(RandomDouble() - 0.5, RandomDouble() - 0.5);
}

/* Reflect provided Vec3 along the provided normal */
inline Vec3 Reflect(const Vec3& v, const Vec3& n)
{
	return v - 2.0 * glm::dot(v, n) * n;
}

/* Refract the (Vec3) incident ray direction along the provided normal using the provided index of refraction ratio */
inline Vec3 Refract(const Vec3& incident_dir, const Vec3& normal, double eta_in_over_out)
{
	double cos_theta = std::fmin(glm::dot(-incident_dir, normal), 1.0);
	Vec3 r_out_perp = eta_in_over_out * (incident_dir + cos_theta * normal);
	Vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - glm::length2(r_out_perp))) * normal;
	return r_out_perp + r_out_parallel;
}

}