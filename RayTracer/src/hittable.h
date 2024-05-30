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
	Sphere(const Point3& center, double radius, std::shared_ptr<Material> material);

	Sphere(const Point3& start, const Point3& stop, double radius, std::shared_ptr<Material> material);

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override;

	AABB BoundingBox() const override;

private:
	Point3 center;
	double radius;
	std::shared_ptr<Material> material;
	bool is_moving;
	Vec3 motion_vector; /* The vector along which the sphere will move */
	AABB bounding_box;

	/* Return the center of the sphere at time t */
	Point3 SphereCenter(double time) const;

	/* Set the UV coordinates of the sphere based on the hit point on a sphere of radius 1 */
	static void GetSphereUV(const Point3& p, double& u, double& v);
};


class HittableList : public Hittable
{
public:
	std::vector<std::shared_ptr<Hittable>> objects;

	HittableList();;
	HittableList(std::shared_ptr<Hittable> hittable);;

	void Clear();;

	void Add(std::shared_ptr<Hittable> hittable);

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override;

	AABB BoundingBox() const override;

private:
	AABB bounding_box;

};

} /* namespace rt */