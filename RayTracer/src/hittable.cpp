#include "hittable.h"

namespace rt
{
/* ===================== */
/* ====== Spheres ====== */
/* ===================== */

Sphere::Sphere(const Point3& start, const Point3& stop, double radius, std::shared_ptr<Material> material) 
	: center(start), radius(std::fmax(0.0, radius)), material(material), is_moving(true)
{
	motion_vector = stop - start;

	/* The bounding box for a moving sphere encapsulates the whole area which it can be in */
	auto rvec = Vec3(radius);
	AABB box1(start - rvec, start + rvec);
	AABB box2(stop - rvec, stop + rvec);
	bounding_box = AABB(box1, box2);
}

Sphere::Sphere(const Point3& center, double radius, std::shared_ptr<Material> material) 
	: center(center), radius(std::fmax(0.0, radius)), material(material), is_moving(false)
{
	auto rvec = Vec3(radius);
	bounding_box = AABB(center - rvec, center + rvec);
}

bool Sphere::Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const
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
	Vec3 outward_normal = (interaction.posn - current_center) / radius;
	interaction.SetFaceNormal(ray, outward_normal);
	GetSphereUV(outward_normal, interaction.u, interaction.v);
	interaction.material = material;

	return true;
}

AABB Sphere::BoundingBox() const
{
	return bounding_box;
}

Point3 Sphere::SphereCenter(double time) const
{
	/* Linearly interpolate between the sphere center and the end of the motion_vector */
	return center + time * motion_vector;
}

void Sphere::GetSphereUV(const Point3& p, double& u, double& v)
{
	/* p: hit point on a sphere of radius 1, centered at the origin */
	/* u: returned value [0, 1] of angle around the z-axis from x = -1 */
	/* v: returned value [0, 1] of angle from z = -1 to z = +1 */

	double theta = std::acos(p.z);
	double phi = std::atan2(p.y, p.x) + Pi;

	u = phi / (2.0 * Pi);
	v = theta / Pi;
}


/* ============================ */
/* ====== Parallelograms ====== */
/* ============================ */

Parallelogram::Parallelogram(const Point3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> material)
	: Q(Q), u(u), v(v), material(material)
{
	Vec3 n = glm::cross(u, v);
	normal = glm::normalize(n);

	/* In the equation for a plane (Ax + By + Cz = D), (A,B,C) is the normal vector. We can then solve for D 
	by picking a point (x,y,z) on the plane. We know Q is on the plane so D = n dot Q */
	D = glm::dot(normal, Q);

	w = n / glm::dot(n, n);

	SetBoundingBox();
}

void Parallelogram::SetBoundingBox()
{
	AABB bbox_diagonal1 = AABB(Q, Q + u + v);
	AABB bbox_diagonal2 = AABB(Q + u, Q + v);
	bounding_box = AABB(bbox_diagonal1, bbox_diagonal2);
}

AABB Parallelogram::BoundingBox() const
{
	return bounding_box;
}

bool Parallelogram::Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const
{
	/* If our ray is defined as R = P + td, the intersection with the plane becomes
	n dot (P + td) = D. Solving for t, we get t = (D - n dot P) / (n dot d) */
	
	double denominator = glm::dot(normal, ray.direction);

	/* No hit if the ray is parallel to the plane */
	if (std::fabs(denominator) < Eps) return false;

	/* Return false if the hit point parameter t is outside the ray interval */
	double t = (D - glm::dot(normal, ray.origin)) / denominator;
	if (!ray_t.Contains(t)) return false;

	/* Determine if the hit point lies within the bounds of the parallelogram using the planar coordinates */
	Point3 intersection = ray.At(t);
	Vec3 planar_hitpoint_vector = intersection - Q;
	double alpha = glm::dot(w, glm::cross(planar_hitpoint_vector, v));
	double beta = glm::dot(w, glm::cross(u, planar_hitpoint_vector));
	if (!IsInterior(alpha, beta, interaction)) return false;

	/* Ray hits within the plane bounds... return interaction */
	interaction.t = t;
	interaction.posn = intersection;
	interaction.material = material;
	interaction.SetFaceNormal(ray, normal);

	return true;
}

bool Parallelogram::IsInterior(double a, double b, Interaction& interaction) const
{
	Interval unit_interval = Interval(0.0, 1.0);

	if (!unit_interval.Contains(a) || !unit_interval.Contains(b)) return false;

	interaction.u = a;
	interaction.v = b;
	return true;
}

/* =========================== */
/* ====== Hittable List ====== */
/* =========================== */

HittableList::HittableList(std::shared_ptr<Hittable> hittable)
{
	Add(hittable);
}

HittableList::HittableList()
{

}

void HittableList::Clear()
{
	objects.clear();
}

void HittableList::Add(std::shared_ptr<Hittable> hittable)
{
	objects.push_back(hittable);
	bounding_box = AABB(bounding_box, hittable->BoundingBox());
}

bool HittableList::Hit(const Ray& ray, Interval ray_t, Interaction& interaction) const
{
	Interaction temp_interact;
	bool hit_anything = false;
	double closest_so_far = ray_t.max;

	for (const auto& hittable : objects)
	{
		if (hittable->Hit(ray, Interval(ray_t.min, closest_so_far), temp_interact))
		{
			hit_anything = true;
			closest_so_far = temp_interact.t;
			interaction = temp_interact;
		}
	}

	return hit_anything;
}

AABB HittableList::BoundingBox() const
{
	return bounding_box;
}


/* ============================= */
/* ====== Compound Shapes ====== */
/* ============================= */

std::shared_ptr<rt::HittableList> Box(const Point3& a, const Point3& b, std::shared_ptr<Material> material)
{
	auto sides = std::make_shared<HittableList>();

	auto min = Point3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
	auto max = Point3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

	auto dx = Vec3(max.x - min.x, 0.0, 0.0);
	auto dy = Vec3(0.0, max.y - min.y, 0.0);
	auto dz = Vec3(0.0, 0.0, max.z - min.z);

	sides->Add(std::make_shared<Parallelogram>(Point3(min.x, min.y, max.z),  dx,  dy, material));
	sides->Add(std::make_shared<Parallelogram>(Point3(max.x, min.y, max.z), -dz,  dy, material));
	sides->Add(std::make_shared<Parallelogram>(Point3(max.x, min.y, min.z), -dx,  dy, material));
	sides->Add(std::make_shared<Parallelogram>(Point3(min.x, min.y, min.z),  dz,  dy, material));
	sides->Add(std::make_shared<Parallelogram>(Point3(min.x, max.y, max.z),  dx, -dz, material));
	sides->Add(std::make_shared<Parallelogram>(Point3(min.x, min.y, min.z),  dx,  dz, material));

	return sides;
}

std::shared_ptr<rt::HittableList> Box(std::shared_ptr<Material> material)
{
	return Box(Point3(-0.5, -0.5, -0.5), Point3(0.5, 0.5, 0.5), material);
}

}