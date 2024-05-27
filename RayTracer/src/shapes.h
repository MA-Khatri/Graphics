#pragma once

#include "interaction.h"

namespace rt 
{

class Shape
{
public:
	virtual ~Shape() = default;

	virtual bool Intersect(const Ray& ray, Interval ray_t, Interaction& interaction) const = 0;
};

class Sphere : public Shape
{
public:
	Sphere(const Point3& center, double radius, std::shared_ptr<Material> material) 
		: center(center), radius(std::fmax(0.0, radius)), material(material), is_moving(false) {}

	Sphere(const Point3& start, const Point3& stop, double radius, std::shared_ptr<Material> material)
		: center(start), radius(std::fmax(0.0, radius)), material(material), is_moving(true) 
	{
		motion_vector = stop - start;
	}

	bool Intersect(const Ray& ray, Interval ray_t, Interaction& interaction) const override
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

private:
	Point3 center;
	double radius;
	std::shared_ptr<Material> material;
	bool is_moving;
	Vec3 motion_vector; /* The vector along which the sphere will move */

	/* Return the center of the sphere at time t */
	Point3 SphereCenter(double time) const
	{
		/* Linearly interpolate between the sphere center and the end of the motion_vector */
		return center + time * motion_vector;
	}
};


class ShapesList : public Shape
{
public:
	std::vector<std::shared_ptr<Shape>> shapes;

	ShapesList() {};
	ShapesList(std::shared_ptr<Shape> shape) { Add(shape); };

	void Clear() { shapes.clear(); };

	void Add(std::shared_ptr<Shape> shape)
	{
		shapes.push_back(shape);
	}

	bool Intersect(const Ray& ray, Interval ray_t, Interaction& interaction) const override
	{
		Interaction tempInteract;
		bool hitAnything = false;
		double closestSoFar = ray_t.max;

		for (const auto& shape : shapes)
		{
			if (shape->Intersect(ray, Interval(ray_t.min, closestSoFar), tempInteract))
			{
				hitAnything = true;
				closestSoFar = tempInteract.t;
				interaction = tempInteract;
			}
		}

		return hitAnything;
	}

};

} /* namespace rt */