#pragma once

#include "ray.h"

#include <vector>


class Interaction
{
public:
	Point3 posn;
	Vec3 normal;
	double t;
	bool frontFace;

	void SetFaceNormal(const Ray& ray, const Vec3& outwardNormal)
	{
		/* Sets the interaction normal vector */
		/* Note: the parameter `outwardNormal` is assumed to have unit length */

		frontFace = Dot(ray.direction, outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}

};