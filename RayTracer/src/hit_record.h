#pragma once

#include "ray.h"
#include "transform.h"

#include <vector>
#include <memory>

namespace rt 
{

class Material;

class HitRecord
{
public:
	Point3 posn; /* Model space position */
	Vec3 normal; /* Model space normal */
	std::shared_ptr<Material> material;
	double t; /* position of hit along the ray */
	double u; /* uv coordinates of hit (for textures) */
	double v;
	bool front_face;

	/* Used to convert between model space and world space.
	All interaction calculations are done in model space then converted to world space. */
	Transform transform; 

public:
	HitRecord() {} /* No constructor is actually used for this class */

	void SetFaceNormal(const Vec3& ray_direction, const Vec3& outward_normal)
	{
		/* Sets the interaction normal vector */
		/* Note: the parameter `outward_normal` is assumed to have unit length */

		front_face = glm::dot(ray_direction, outward_normal) < 0.0;
		normal = front_face ? outward_normal : -outward_normal;
	}

};

} /* namespace rt */