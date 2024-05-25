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
	float t = 0.0f;
	std::shared_ptr<Material> material;
	bool front_face = false;

	Interaction() {} /* No constructor is actually used for this class */

	void SetFaceNormal(const Ray& ray, const Vec3& outward_normal)
	{
		/* Sets the interaction normal vector */
		/* Note: the parameter `outward_normal` is assumed to have unit length */

		front_face = glm::dot(ray.direction, outward_normal) < 0.0f;
		normal = front_face ? outward_normal : -outward_normal;
	}

};

} /* namespace rt */