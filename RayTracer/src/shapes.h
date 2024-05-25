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
	Sphere(const Point3& center, float radius, std::shared_ptr<Material> material) 
		: center(center), radius(fmax(0.0f, radius)), material(material) {}

	bool Intersect(const Ray& ray, Interval ray_t, Interaction& interaction) const override
	{
		Vec3 oc = center - ray.origin;
		float a = ray.direction.LengthSquared();
		float h = Dot(ray.direction, oc);
		float c = oc.LengthSquared() - radius * radius;

		float discriminant = h * h - a * c;
		if (discriminant < 0.0f) return false;

		float sqrtd = std::sqrt(discriminant);

		/* Find the nearest root that lies in the acceptable range */
		float root = (h - sqrtd) / a;
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
	float radius;
	std::shared_ptr<Material> material;
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
		float closestSoFar = ray_t.max;

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