#pragma once

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/epsilon.hpp"

/* Custom wrapper over glm */

namespace rt
{
	/* ============================== */
	/* === Accessor redefinitions === */
	/* ============================== */

	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	using Point3 = glm::vec3;
	using Color = glm::vec3;
	using Vec4 = glm::vec4;
	using Mat4 = glm::mat4;


	/* ================== */
	/* === Vec3 Utils === */
	/* ================== */

	inline bool Equal(Vec3 u, Vec3 v)
	{
		return glm::all(glm::equal(u, v));
	}

	inline bool NearZero(Vec3 v)
	{
		return glm::all(glm::epsilonEqual(v, Vec3(0.0f), Eps));
	}

	inline Vec3 RandomVec3()
	{
		return Vec3(RandomFloat(), RandomFloat(), RandomFloat());
	}

	inline Vec3 RandomVec3(float min, float max)
	{
		return Vec3(RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max));
	}

	inline Vec2 RandomInUnitDisk()
	{
		while (true)
		{
			Vec2 p = Vec2(RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f));
			if (glm::length2(p) < 1) return p;
		}
	}

	inline Vec3 RandomInUnitSphere()
	{
		while (true)
		{
			auto p = RandomVec3(-1, 1);
			if (glm::length2(p) < 1) return p;
		}
	}

	inline Vec3 RandomUnitVector()
	{
		return glm::normalize(RandomInUnitSphere());
	}

	inline Vec3 RandomOnHemisphere(const Vec3& normal)
	{
		Vec3 on_unit_sphere = RandomUnitVector();

		/* In the same hemisphere as the normal? */
		if (glm::dot(on_unit_sphere, normal) > 0.0) return on_unit_sphere;
		else return -on_unit_sphere;
	}

	inline Vec3 Reflect(const Vec3& v, const Vec3& n)
	{
		return v - 2 * glm::dot(v, n) * n;
	}

	inline Vec3 Refract(const Vec3& incident_dir, const Vec3& normal, double eta_in_over_out)
	{
		double cos_theta = std::fmin(glm::dot(-incident_dir, normal), 1.0);
		Vec3 r_out_perp = eta_in_over_out * (incident_dir + cos_theta * normal);
		Vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - glm::length2(r_out_perp))) * normal;
		return r_out_perp + r_out_parallel;
	}

}