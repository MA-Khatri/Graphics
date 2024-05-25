#pragma once

#include "math.h"

namespace rt 
{

class Ray 
{
public:
	Ray() {}

	Ray(const Point3& origin, const Vec3& direction) : origin(origin), direction(direction) {}

	Point3 At(double t) const {
		return origin + t * direction;
	}

public:
	Point3 origin;
	Vec3 direction;
};

} /* namespace rt */