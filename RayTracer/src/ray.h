#pragma once

#include "math.h"

namespace rt 
{

class Ray 
{
public:
	Ray() : origin(Point3(0.0)), direction(Vec3(0.0)), time(0.0) {}
	Ray(const Point3& origin, const Vec3& direction) : origin(origin), direction(direction), time(0.0) {}
	Ray(const Point3& origin, const Vec3& direction, double time) : origin(origin), direction(direction), time(time) {}


	/* Return the position at a distance t along the ray */
	inline Point3 At(double t) const 
	{
		return origin + t * direction;
	}

	/* Return the position at a distance t along the ray which is transformed by the provided matrix */
	inline Point3 At(double t, Mat4 transform) const
	{
		return Point3(transform * Vec4(origin, 1.0) + t * transform * Vec4(direction, 0.0));
	}

public:
	Point3 origin;
	Vec3 direction;
	double time;
};

} /* namespace rt */