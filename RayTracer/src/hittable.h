#pragma once

#include "interaction.h"
#include "aabb.h"
#include "texture.h"

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


class Parallelogram : public Hittable
{
public:
	/* Construct a parallelogram using an origin Q and two vectors u, v that define its sides */
	Parallelogram(const Point3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> material);

	/* Compute the bounding box encapsulating all four vertices */
	virtual void SetBoundingBox();

	AABB BoundingBox() const override;

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override;


	/* Given the hit point in plane coordinates, return false if it is outside the primitive. 
	Otherwise, set the UV coordinates of the hit and return true. */
	virtual bool IsInterior(double a, double b, Interaction& interaction) const;

private:
	Point3 Q;
	Vec3 u, v, w;
	std::shared_ptr<Material> material;
	AABB bounding_box;
	Vec3 normal;
	double D;
};


class HittableList : public Hittable
{
public:
	std::vector<std::shared_ptr<Hittable>> objects;

public:
	HittableList();
	HittableList(std::shared_ptr<Hittable> hittable);

	void Clear();

	void Add(std::shared_ptr<Hittable> hittable);

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override;

	AABB BoundingBox() const override;

private:
	AABB bounding_box;

};


/* Compound shapes */

/* Returns a 3D box that contains the two provided opposite vertices 'a' and 'b' */
std::shared_ptr<HittableList> Box(const Point3& a, const Point3& b, std::shared_ptr<Material> material);

/* Returns a 1x1x1 box centered at the origin with the provided material. */
std::shared_ptr<HittableList> Box(std::shared_ptr<Material> material);

} /* namespace rt */