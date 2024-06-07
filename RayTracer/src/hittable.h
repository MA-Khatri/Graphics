#pragma once

#include "interaction.h"
#include "aabb.h"
#include "texture.h"
#include "transform.h"

namespace rt 
{

class Hittable
{
public:
	virtual ~Hittable() = default;

	/* Handle ray-object interaction */
	virtual bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const = 0;

	/* Return this object's axis aligned bounding box in world space coordinates */
	inline AABB BoundingBox() const { return bounding_box; }

public:
	/* Store the transformation matrices for this object */
	Transform transform;

protected:
	/* The axis aligned bounding box which tightly encloses the world space dimensions of the object */
	AABB bounding_box;
};

class Sphere : public Hittable
{
public:
	/* Manually set the transform when generating a sphere */
	Sphere(const Transform& t_transform, std::shared_ptr<Material> material);

	/* Standard sphere generation with center and radius. This sets up the initial transforms for you */
	Sphere(const Vec3& center, double radius, std::shared_ptr<Material> material);

	/* For motion blur spheres, provide a start and stopping position and radius */
	Sphere(const Point3& start, const Point3& stop, double radius, std::shared_ptr<Material> material);

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override;

private:
	std::shared_ptr<Material> material;
	Vec3 motion_vector;


private:
	/* Return the center of the sphere (in model space) at time t */
	Point3 SphereCenter(double time) const;

	/* Set the UV coordinates of the sphere based on the hit point on a sphere of radius 1 */
	static void GetSphereUV(const Point3& p, double& u, double& v);

	void SetBoundingBox();
};


class Parallelogram : public Hittable
{
public:
	/* Construct a parallelogram using an origin Q and two vectors u, v that define its sides */
	Parallelogram(const Point3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> material);

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override;

private:
	Point3 Q;
	Vec3 u, v, w;
	std::shared_ptr<Material> material;
	Vec3 normal;
	double D;

private:
	/* Given the hit point in plane coordinates, return false if it is outside the primitive.
	Otherwise, set the UV coordinates of the hit and return true. */
	virtual bool IsInterior(double a, double b, Interaction& interaction) const; /* why virtual? */

	/* Compute the bounding box encapsulating all four vertices */
	virtual void SetBoundingBox(); /* why virtual? */
};


class HittableList : public Hittable
{
public:
	std::vector<std::shared_ptr<Hittable>> objects;

public:
	HittableList() {}
	HittableList(std::shared_ptr<Hittable> hittable);

	void Clear();

	void Add(std::shared_ptr<Hittable> hittable);

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override;
};


/* Compound shapes */

/* Returns a 3D box that contains the two provided opposite vertices 'a' and 'b' */
std::shared_ptr<HittableList> Box(const Point3& a, const Point3& b, std::shared_ptr<Material> material);

/* Returns a 1x1x1 box centered at the origin with the provided material. */
std::shared_ptr<HittableList> Box(std::shared_ptr<Material> material);

} /* namespace rt */