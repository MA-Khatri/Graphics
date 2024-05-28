#pragma once

#include "interaction.h"
#include "aabb.h"

namespace rt 
{

class Hittable
{
public:
	virtual ~Hittable() = default;

	virtual bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const = 0;

	virtual AABB BoundingBox() const = 0;
};

class Sphere : public Hittable
{
public:
	Sphere(const Point3& center, double radius, std::shared_ptr<Material> material) 
		: center(center), radius(std::fmax(0.0, radius)), material(material), is_moving(false) 
	{
		auto rvec = Vec3(radius);
		bounding_box = AABB(center - rvec, center + rvec);
	}

	Sphere(const Point3& start, const Point3& stop, double radius, std::shared_ptr<Material> material)
		: center(start), radius(std::fmax(0.0, radius)), material(material), is_moving(true) 
	{
		motion_vector = stop - start;
		
		/* The bounding box for a moving sphere encapsulates the whole area which it can be in */
		auto rvec = Vec3(radius);
		AABB box1(start - rvec, start + rvec);
		AABB box2(stop - rvec, stop + rvec);
		bounding_box = AABB(box1, box2);
	}

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override
	{
		Point3 current_center = is_moving ? SphereCenter(ray.time) : center;
		Vec3 oc = current_center - ray.origin;
		double a = glm::length2(ray.direction);
		double h = glm::dot(ray.direction, oc);
		double c = glm::length2(oc) - radius * radius;

		double discriminant = h * h - a * c;
		if (discriminant < 0.0) return false;

		double sqrtd = std::sqrt(discriminant);

		/* Find the nearest root that lies in the acceptable range */
		double root = (h - sqrtd) / a;
		if (!ray_t.Surrounds(root))
		{
			root = (h + sqrtd) / a;
			if (!ray_t.Surrounds(root)) return false;
		}

		interaction.t = root;
		interaction.posn = ray.At(interaction.t);
		Vec3 outwardNormal = (interaction.posn - current_center) / radius;
		interaction.SetFaceNormal(ray, outwardNormal);
		interaction.material = material;

		return true;
	}

	AABB BoundingBox() const override { return bounding_box; }

private:
	Point3 center;
	double radius;
	std::shared_ptr<Material> material;
	bool is_moving;
	Vec3 motion_vector; /* The vector along which the sphere will move */
	AABB bounding_box;

	/* Return the center of the sphere at time t */
	Point3 SphereCenter(double time) const
	{
		/* Linearly interpolate between the sphere center and the end of the motion_vector */
		return center + time * motion_vector;
	}
};


class HittableList : public Hittable
{
public:
	std::vector<std::shared_ptr<Hittable>> objects;

	HittableList() {};
	HittableList(std::shared_ptr<Hittable> hittable) { Add(hittable); };

	void Clear() { objects.clear(); };

	void Add(std::shared_ptr<Hittable> hittable)
	{
		objects.push_back(hittable);
		bounding_box = AABB(bounding_box, hittable->BoundingBox());
	}

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override
	{
		Interaction tempInteract;
		bool hitAnything = false;
		double closestSoFar = ray_t.max;

		for (const auto& hittable : objects)
		{
			if (hittable->Hit(ray, Interval(ray_t.min, closestSoFar), tempInteract))
			{
				hitAnything = true;
				closestSoFar = tempInteract.t;
				interaction = tempInteract;
			}
		}

		return hitAnything;
	}

	AABB BoundingBox() const override { return bounding_box; }

private:
	AABB bounding_box;

};

} /* namespace rt */