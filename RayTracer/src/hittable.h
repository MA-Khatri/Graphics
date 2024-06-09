#pragma once

#include "interaction.h"
#include "aabb.h"
#include "texture.h"
#include "transform.h"
#include "material.h"

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

	/* Set transform and motion_vector */
	Sphere(const Transform& t_transform, const Vec3& motion_vector, std::shared_ptr<Material> material);

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

	/* Construct a unit quad centered at the origin with normal along +z transformed by the given transform */
	Parallelogram(const Transform& t_transform, std::shared_ptr<Material> material);

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



class Triangle : public Hittable
{
public:
	/* Construct a triangle (in model space!) using the positions of its three vertices.
	The normal is computed from the triangle vertices. */
	Triangle(const Transform& t_transform, const Point3& v0, const Point3& v1, const Point3& v2, std::shared_ptr<Material> material);

	///* TODO: Construct a triangle (in model space!) using three vertices returned by the obj loader. */
	//Triangle(const Transform& t_transform, const objl::Vertex& v0, const objl::Vertex& v1, const objl::Vertex& v2, std::shared_ptr<Material> material);

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override;

private:
	Point3 v0;
	Point3 v1;
	Point3 v2;
	std::shared_ptr<Material> material;

	Vec3 e01; /* Vector from v0 to v1 */
	Vec3 e02; /* Vector from v0 to v2 */
	Vec3 normal; /* Unit normal vector in model space */

private:
	void SetBoundingBox();
};



class ConstantMedium : public Hittable
{
public:
	ConstantMedium(std::shared_ptr<Hittable> boundary, double density, std::shared_ptr<Texture> texture);
	ConstantMedium(std::shared_ptr<Hittable> boundary, double density, const Color& albedo);

	bool Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const override;

private:
	std::shared_ptr<Hittable> boundary;
	double neg_inv_density;
	std::shared_ptr<Material> phase_function;

private:
	void SetBoundingBox();
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

/* Returns a unit cube centered at the origin with the provided material transformed with the provided transform. */
std::shared_ptr<HittableList> Box(const Transform& t_transform, std::shared_ptr<Material> material);

/* Load a triangle mesh */
HittableList LoadMesh(const Transform& t_transform, const std::string& filepath, std::shared_ptr<Material> material);

} /* namespace rt */