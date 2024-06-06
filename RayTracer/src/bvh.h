#pragma once

#include "common.h"
#include "aabb.h"
#include "hittable.h"

#include <algorithm>

namespace rt
{

class BVH_Node : public Hittable
{
public:
	BVH_Node(HittableList list) : BVH_Node(list.objects, 0, list.objects.size()) 
	{
		/* Note: this constructor creates an implicit copy of the BVH_Node that only
		exists for the lifespan of the constructor. But this is ok since we only need to 
		store the resulting BVH. */

	}

	BVH_Node(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
	{
		/* Build a bounding box that spans all the source objects */
		bounding_box = AABB(/*Interval(+Inf, -Inf), Interval(+Inf, -Inf), Interval(+Inf, -Inf)*/);
		for (size_t object_index = start; object_index < end; object_index++)
		{
			bounding_box = AABB(bounding_box, objects[object_index]->BoundingBox());
		}

		/* Split along the longest axis */
		int axis = bounding_box.LongestAxis();

		auto comparator = (axis == 0) ? BoxXCompare : ((axis == 1) ? BoxYCompare : BoxZCompare);

		size_t object_span = end - start;

		if (object_span == 1)
		{
			/* If there is only 1 object remaining, assign it to both children*/
			left = right = objects[start];
		}
		else if (object_span == 2)
		{
			/* If there are two objects remaining, assign one to each child */
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			/* Sort the objects along the determined longest axis, then 
			assign the first half to the first child, second half to the other */
			std::sort(objects.begin() + start, objects.begin() + end, comparator);

			/* Recursively create the remaining nodes */
			auto mid = start + object_span / 2;
			left = std::make_shared<BVH_Node>(objects, start, mid);
			right = std::make_shared<BVH_Node>(objects, mid, end);
		}
	}

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override
	{
		if (!bounding_box.Hit(ray, ray_t)) return false;

		bool hit_left = left->Hit(ray, ray_t, interaction);
		bool hit_right = right->Hit(ray, Interval(ray_t.min, hit_left ? interaction.t : ray_t.max), interaction);

		return hit_left || hit_right;
	}

private:
	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	AABB bounding_box;

private:
	static bool BoxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis_index)
	{
		Interval a_axis_interval = a->BoundingBox().AxisInterval(axis_index);
		Interval b_axis_interval = b->BoundingBox().AxisInterval(axis_index);
		return a_axis_interval.min < b_axis_interval.min;
	}

	static bool BoxXCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
	{
		return BoxCompare(a, b, 0);
	}

	static bool BoxYCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
	{
		return BoxCompare(a, b, 1);
	}

	static bool BoxZCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
	{
		return BoxCompare(a, b, 2);
	}
};

}