#pragma once

#include "interval.h"
#include "common.h"

namespace rt
{

/* Axis aligned bounding box class for bounding volume hierarchies */
class AABB
{
public:
	AABB() {} /* The default AABB is empty, since intervals are empty by default */

	AABB(const Interval& x, const Interval& y, const Interval& z) : x(x), y(y), z(z) {}

	AABB(const Point3& a, const Point3& b)
	{
		/* Treat the two points `a`, and `b` as extrema for the bounding box */
		x = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
		y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
		z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
	}

	AABB(const AABB& box0, const AABB& box1)
	{
		x = Interval(box0.x, box1.x);
		y = Interval(box0.y, box1.y);
		z = Interval(box0.z, box1.z);
	}

	/* Returns the interval of the AABB along the axes which can be indexed by an unsigned int */
	const Interval& AxisInterval(unsigned int n) const
	{
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	bool Hit(const Ray& ray, Interval ray_t) const
	{
		const Point3& ray_origin = ray.origin;
		const Vec3& ray_direction = ray.direction;

		for (unsigned int axis = 0; axis < 3; axis++)
		{
			const Interval& ax = AxisInterval(axis);
			const double adinv = 1.0 / ray_direction[axis];

			double t0 = (ax.min - ray_origin[axis]) * adinv;
			double t1 = (ax.max - ray_origin[axis]) * adinv;

			if (t0 < t1)
			{
				if (t0 > ray_t.min) ray_t.min = t0;
				if (t1 < ray_t.max) ray_t.max = t1;
			}
			else
			{
				if (t1 > ray_t.min) ray_t.min = t1;
				if (t0 < ray_t.max) ray_t.max = t0;
			}

			if (ray_t.max <= ray_t.min) return false;
		}

		return true;
	}

	/* Returns the index of the longest axis of the bounding box */
	int LongestAxis() const
	{
		if (x.Size() > y.Size()) return x.Size() > z.Size() ? 0 : 2;
		else return y.Size() > z.Size() ? 1 : 2;
	}

public:
	Interval x, y, z;
};

}