#pragma once

#include "ray.h"

#include <vector>
#include <memory>

namespace rt 
{

class Material;

class Interaction
{
public:
	Point3 posn;
	Vec3 normal;
	std::shared_ptr<Material> material;
	double t;
	double u;
	double v;
	bool front_face;

	Interaction() {} /* No constructor is actually used for this class */

	void SetFaceNormal(const Vec3& ray_direction, const Vec3& outward_normal)
	{
		/* Sets the interaction normal vector */
		/* Note: the parameter `outward_normal` is assumed to have unit length */

		front_face = glm::dot(ray_direction, outward_normal) < 0.0;
		normal = front_face ? outward_normal : -outward_normal;
	}

};

} /* namespace rt */