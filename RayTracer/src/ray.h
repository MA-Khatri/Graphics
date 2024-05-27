#pragma once

#include "math.h"

namespace rt 
{

class Ray 
{
public:
	Ray() {}
	Ray(const Point3& origin, const Vec3& direction) : origin(origin), direction(direction), time(0) {}
	Ray(const Point3& origin, const Vec3& direction, float time) : origin(origin), direction(direction), time(time) {}


	/* Return the position at a distance t along the ray */
	inline Point3 At(double t) const 
	{
		return origin + t * direction;
	}

public:
	Point3 origin;
	Vec3 direction;
	float time;
};

} /* namespace rt */