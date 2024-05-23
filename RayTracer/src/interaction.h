#pragma once

#include "ray.h"

#include <vector>

namespace RayTracer {

class Material;

class Interaction
{
public:
	Point3 posn;
	Vec3 normal;
	double t = 0.0;
	shared_ptr<Material> material;
	bool front_face = false;

	void SetFaceNormal(const Ray& ray, const Vec3& outward_normal)
	{
		/* Sets the interaction normal vector */
		/* Note: the parameter `outward_normal` is assumed to have unit length */

		front_face = Dot(ray.direction, outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}

};

} /* namespace RayTracer */