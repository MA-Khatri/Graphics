#pragma once

#include "interaction.h"

class Shape
{
public:
	virtual ~Shape() = default;

	virtual bool Intersect(const Ray& ray, double tMin, double tMax, Interaction& interaction) const = 0;
};

class Sphere : public Shape
{
public:
	Sphere(const Point3& center, double radius) : center(center), radius(fmax(0, radius)) {};

	bool Intersect(const Ray& ray, double tMin, double tMax, Interaction& interaction) const override
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
		if (root <= tMin || tMax <= root)
		{
			root = (h + sqrtd) / a;
			if (root <= tMin || tMax <= root) return false;
		}

		interaction.t = root;
		interaction.posn = ray.At(interaction.t);
		Vec3 outwardNormal = (interaction.posn - center) / radius;
		interaction.SetFaceNormal(ray, outwardNormal);

		return true;
	};

public:
	Point3 center;
	double radius;
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

	bool Intersect(const Ray& ray, double tMin, double tMax, Interaction& interaction) const override
	{
		Interaction tempInteract;
		bool hitAnything = false;
		auto closestSoFar = tMax;

		for (const auto& shape : shapes)
		{
			if (shape->Intersect(ray, tMin, closestSoFar, tempInteract))
			{
				hitAnything = true;
				closestSoFar = tempInteract.t;
				interaction = tempInteract;
			}
		}

		return hitAnything;
	}

};