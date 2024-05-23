#pragma once

#include "interaction.h"

namespace RayTracer {

class Shape
{
public:
	virtual ~Shape() = default;

	virtual bool Intersect(const Ray& ray, Interval ray_t, Interaction& interaction) const = 0;
};

class Sphere : public Shape
{
public:
	Sphere(const Point3& center, double radius, shared_ptr<Material> material) 
		: center(center), radius(fmax(0, radius)), material(material) {}

	bool Intersect(const Ray& ray, Interval ray_t, Interaction& interaction) const override
	{
		Vec3 oc = center - ray.origin;
		auto a = ray.direction.LengthSquared();
		auto h = Dot(ray.direction, oc);
		auto c = oc.LengthSquared() - radius * radius;

		auto discriminant = h * h - a * c;
		if (discriminant < 0) return false;

		auto sqrtd = sqrt(discriminant);

		/* Find the nearest root that lies in the acceptable range */
		auto root = (h - sqrtd) / a;
		if (!ray_t.Surrounds(root))
		{
			root = (h + sqrtd) / a;
			if (!ray_t.Surrounds(root)) return false;
		}

		interaction.t = root;
		interaction.posn = ray.At(interaction.t);
		Vec3 outwardNormal = (interaction.posn - center) / radius;
		interaction.SetFaceNormal(ray, outwardNormal);
		interaction.material = material;

		return true;
	};

private:
	Point3 center;
	double radius;
	shared_ptr<Material> material;
};


class ShapesList : public Shape
{
public:
	std::vector<shared_ptr<Shape>> shapes;

	ShapesList() {};
	ShapesList(shared_ptr<Shape> shape) { Add(shape); };

	void Clear() { shapes.clear(); };

	void Add(shared_ptr<Shape> shape)
	{
		shapes.push_back(shape);
	}

	bool Intersect(const Ray& ray, Interval ray_t, Interaction& interaction) const override
	{
		Interaction tempInteract;
		bool hitAnything = false;
		auto closestSoFar = ray_t.max;

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

} /* namespace RayTracer */