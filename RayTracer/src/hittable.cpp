#include "hittable.h"

#include "OBJ-Loader.h"

namespace rt
{
/* ===================== */
/* ====== Spheres ====== */
/* ===================== */

Sphere::Sphere(const Transform& t_transform, std::shared_ptr<Material> material)
	: material(material), motion_vector(Vec3(0.0)) 
{
	transform = t_transform;
	SetBoundingBox();
}

Sphere::Sphere(const Transform& t_transform, const Vec3& motion_vector, std::shared_ptr<Material> material)
	: material(material), motion_vector(motion_vector)
{
	transform = t_transform;
	SetBoundingBox();
}

Sphere::Sphere(const Vec3& center, double radius, std::shared_ptr<Material> material)
	: material(material), motion_vector(Vec3(0.0))
{
	transform.Translate(center);
	transform.Scale(radius);

	SetBoundingBox();
}

Sphere::Sphere(const Point3& start, const Point3& stop, double radius, std::shared_ptr<Material> material)
	: material(material), motion_vector(stop - start)
{
	transform.Translate(start);
	transform.Scale(radius);

	SetBoundingBox();
}

bool Sphere::Hit(const Ray& ray, Interval ray_t, HitRecord& hrec) const
{
	Ray model_ray = transform.WorldToModel(ray);

	Point3 current_center = SphereCenter(ray.time); /* In model coords */
	Vec3 oc = current_center - model_ray.origin;
	double a = glm::length2(model_ray.direction);
	double h = glm::dot(model_ray.direction, oc);
	double c = glm::length2(oc) - 1.0;

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

	hrec.t = root;
	hrec.posn = model_ray.At(hrec.t); /* store model space posn */


	Vec3 outward_normal = model_ray.At(hrec.t) - current_center; /* model space normal vector */
	GetSphereUV(outward_normal, hrec.u, hrec.v); /* Set UV coords in model space */

	hrec.SetFaceNormal(model_ray.direction, outward_normal); /* set model space normal */

	hrec.material = material;
	hrec.transform = transform;

	return true;
}


double Sphere::PDF_Value(const Point3& origin, const Vec3& direction) const
{
	/* Note: this method only works for stationary spheres! */

	HitRecord hrec;
	auto world_space_ray = Ray(origin, direction);
	if (!this->Hit(world_space_ray, Interval(Eps, Inf), hrec)) return 0.0;

	auto model_space_ray = transform.WorldToModel(world_space_ray);
	double cos_theta_max = std::sqrt(1.0 + 1.0 / glm::length2(model_space_ray.origin));
	double solid_angle = 2.0 * Pi * (1.0 - cos_theta_max);

	return 1.0 / solid_angle;
}


Vec3 Sphere::Random(const Point3& origin) const
{
	/* TODO */
	return Vec3(0.0, 0.0, 1.0);

}


Point3 Sphere::SphereCenter(double time) const
{
	/* Linearly interpolate between the sphere center and the end of the motion_vector */
	return time * motion_vector;
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

void Sphere::SetBoundingBox()
{
	/* Find the transformed bounds of all corners of the box with corners [-1,-1,-1] to [1,1,1]*/
	Vec3 rvec1 = transform.model_to_world * Vec4(-1.0, -1.0, -1.0, 1.0);
	Vec3 rvec2 = transform.model_to_world * Vec4(1.0, -1.0, -1.0, 1.0);
	Vec3 rvec3 = transform.model_to_world * Vec4(1.0, 1.0, -1.0, 1.0);
	Vec3 rvec4 = transform.model_to_world * Vec4(-1.0, 1.0, -1.0, 1.0);

	Vec3 rvec5 = transform.model_to_world * Vec4(-1.0, -1.0, 1.0, 1.0);
	Vec3 rvec6 = transform.model_to_world * Vec4(1.0, -1.0, 1.0, 1.0);
	Vec3 rvec7 = transform.model_to_world * Vec4(1.0, 1.0, 1.0, 1.0);
	Vec3 rvec8 = transform.model_to_world * Vec4(-1.0, 1.0, 1.0, 1.0);

	/* Create a bounding box that encompasses all transformed bounds */
	AABB bounding_box_1 = AABB(AABB(AABB(rvec1, rvec2), AABB(rvec3, rvec4)), 
							   AABB(AABB(rvec5, rvec6), AABB(rvec7, rvec8)));

	/* Do this *again* for the stop point */
	AABB bounding_box_2;
	if (glm::length(motion_vector) > 0.0)
	{
		Vec4 mv = Vec4(motion_vector, 0.0);
		rvec1 = transform.model_to_world * (mv + Vec4(-1.0, -1.0, -1.0, 1.0));
		rvec2 = transform.model_to_world * (mv + Vec4(1.0, -1.0, -1.0, 1.0));
		rvec3 = transform.model_to_world * (mv + Vec4(1.0, 1.0, -1.0, 1.0));
		rvec4 = transform.model_to_world * (mv + Vec4(-1.0, 1.0, -1.0, 1.0));

		rvec5 = transform.model_to_world * (mv + Vec4(-1.0, -1.0, 1.0, 1.0));
		rvec6 = transform.model_to_world * (mv + Vec4(1.0, -1.0, 1.0, 1.0));
		rvec7 = transform.model_to_world * (mv + Vec4(1.0, 1.0, 1.0, 1.0));
		rvec8 = transform.model_to_world * (mv + Vec4(-1.0, 1.0, 1.0, 1.0));

		/* Create a bounding box that encompasses all transformed bounds */
		bounding_box_2 = AABB(AABB(AABB(rvec1, rvec2), AABB(rvec3, rvec4)),
							  AABB(AABB(rvec5, rvec6), AABB(rvec7, rvec8)));
	}

	bounding_box = AABB(bounding_box_1, bounding_box_2);
}


/* ============================ */
/* ====== Parallelograms ====== */
/* ============================ */

Parallelogram::Parallelogram(const Point3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> material)
	: Q(Q), u(u), v(v), material(material)
{
	Vec3 n = glm::cross(u, v);
	area = glm::length(n);
	normal = glm::normalize(n);

	/* In the equation for a plane (Ax + By + Cz = D), (A,B,C) is the normal vector. We can then solve for D 
	by picking a point (x,y,z) on the plane. We know Q is on the plane so D = n dot Q */
	D = glm::dot(normal, Q);

	w = n / glm::dot(n, n);

	SetBoundingBox();
}


Parallelogram::Parallelogram(const Transform& t_transform, std::shared_ptr<Material> material)
	: Q(Point3(-0.5, -0.5, 0.0)), u(Vec3(1.0, 0.0, 0.0)), v(Vec3(0.0, 1.0, 0.0)), 
	w(Vec3(0.0, 0.0, 1.0)), normal(Vec3(0.0, 0.0, 1.0)), material(material)
{
	transform = t_transform;
	D = glm::dot(normal, Q);

	SetBoundingBox();
}

void Parallelogram::SetBoundingBox()
{
	/* Transform the parallelogram origin and direction vectors to world space */
	Vec3 Q_w = transform.model_to_world * Vec4(Q, 1.0);
	Vec3 u_w = transform.model_to_world * Vec4(u, 0.0);
	Vec3 v_w = transform.model_to_world * Vec4(v, 0.0);

	AABB bbox_diagonal1 = AABB(Q_w, Q_w+ u_w + v_w);
	AABB bbox_diagonal2 = AABB(Q_w+ u_w, Q_w+ v_w);
	bounding_box = AABB(bbox_diagonal1, bbox_diagonal2);
}

bool Parallelogram::Hit(const Ray& ray, Interval ray_t, HitRecord& hrec) const
{
	/* If our ray is defined as R = P + td, the intersection with the plane becomes
	n dot (P + td) = D. Solving for t, we get t = (D - n dot P) / (n dot d) */
	
	Ray model_ray = transform.WorldToModel(ray);

	double denominator = glm::dot(normal, model_ray.direction);

	/* No hit if the ray is parallel to the plane */
	if (std::fabs(denominator) < Eps) return false;

	/* Return false if the hit point parameter t is outside the ray interval */
	double t = (D - glm::dot(normal, model_ray.origin)) / denominator;
	if (!ray_t.Contains(t)) return false;

	/* Determine if the hit point lies within the bounds of the parallelogram using the planar coordinates */
	Point3 intersection = model_ray.At(t);
	Vec3 planar_hitpoint_vector = intersection - Q;
	double alpha = glm::dot(w, glm::cross(planar_hitpoint_vector, v));
	double beta = glm::dot(w, glm::cross(u, planar_hitpoint_vector));
	if (!IsInterior(alpha, beta, hrec)) return false;

	/* Ray hits within the plane bounds... update hrec */
	hrec.t = t;
	hrec.posn = intersection;
	hrec.material = material;
	hrec.SetFaceNormal(model_ray.direction, normal);
	hrec.transform = transform;

	return true;
}


double Parallelogram::PDF_Value(const Point3& origin, const Vec3& direction) const
{
	/* Assume input origin and direction are in world space! */

	HitRecord hrec;
	if (!this->Hit(Ray(origin, direction), Interval(Eps, Inf), hrec))
	{
		return 0.0;
	}

	double distance_squared = hrec.t * hrec.t * glm::length2(direction);
	Vec3 world_normal = hrec.transform.model_to_world * Vec4(hrec.normal, 0.0); /* Note: hrec.transform is the same as this->transform here */
	double cosine = std::fabs(glm::dot(direction, world_normal)) / glm::length(direction);

	return distance_squared / (cosine * area);
}


rt::Vec3 Parallelogram::Random(const Point3& origin) const
{
	/* Note: assume origin is provided in world space. 
	Q, u, v are in model space and need to be converted to world space! */
	Point3 world_space_Q = transform.model_to_world * Vec4(Q, 1.0);
	Vec3 world_space_u = transform.model_to_world * Vec4(u, 0.0);
	Vec3 world_space_v = transform.model_to_world * Vec4(v, 0.0);

	/* Pick a random point on this parallelogram */
	Vec3 p = world_space_Q + (RandomDouble() * world_space_u) + (RandomDouble() * world_space_v);

	/* Return a vector to a random point on this parallelogram in world space */
	return p - origin;
}


bool Parallelogram::IsInterior(double a, double b, HitRecord& hrec) const
{
	Interval unit_interval = Interval(0.0, 1.0);

	if (!unit_interval.Contains(a) || !unit_interval.Contains(b)) return false;

	hrec.u = a;
	hrec.v = b;
	return true;
}


/* ======================= */
/* ====== Triangles ====== */
/* ======================= */

Triangle::Triangle(const Transform& t_transform, const Point3& v0p, const Point3& v1p, const Point3& v2p, std::shared_ptr<Material> material)
	: v0p(v0p), v1p(v1p), v2p(v2p), material(material)
{
	transform = t_transform;

	/* Edge vectors and normal in model space */
	e01 = v1p - v0p;
	e02 = v2p - v0p;

	Vec3 normal = glm::normalize(glm::cross(e01, e02));
	v0n = normal;
	v1n = normal;
	v2n = normal;

	SetBoundingBox();
}


Triangle::Triangle(const Transform& t_transform, const objl::Vertex& v0, const objl::Vertex& v1, const objl::Vertex& v2, std::shared_ptr<Material> material)
	: material(material)
{
	transform = t_transform;

	v0p = Point3(v0.Position.X, v0.Position.Y, v0.Position.Z);
	v1p = Point3(v1.Position.X, v1.Position.Y, v1.Position.Z);
	v2p = Point3(v2.Position.X, v2.Position.Y, v2.Position.Z);

	e01 = v1p - v0p;
	e02 = v2p - v0p;

	v0n = Vec3(v0.Normal.X, v0.Normal.Y, v0.Normal.Z);
	v1n = Vec3(v1.Normal.X, v1.Normal.Y, v1.Normal.Z);
	v2n = Vec3(v2.Normal.X, v2.Normal.Y, v2.Normal.Z);

	/* if normals are not provided, compute them manually */
	if (NearZero(v0n) || NearZero(v1n) || NearZero(v2n))
	{
		Vec3 normal = glm::normalize(glm::cross(e01, e02));
		v0n = normal;
		v1n = normal;
		v2n = normal;
	}

	SetBoundingBox();
}


bool Triangle::Hit(const Ray& ray, Interval ray_t, HitRecord& hrec) const
{
	/* Moller-Trumbore intersection algorithm */
	/* https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm */

	Ray model_ray = transform.WorldToModel(ray);

	Vec3 ray_cross_e02 = glm::cross(model_ray.direction, e02);
	double det = glm::dot(e01, ray_cross_e02);

	/* No hit if the ray is parallel to the triangle */
	if (std::fabs(det) < Eps) return false;

	/* Is the ray within the bounds of the triangle? */
	double inv_det = 1.0 / det;
	Vec3 s = model_ray.origin - v0p;
	double u = inv_det * glm::dot(s, ray_cross_e02);
	if (u < 0.0 || u > 1.0) return false;

	Vec3 s_cross_e01 = glm::cross(s, e01);
	double v = inv_det * glm::dot(model_ray.direction, s_cross_e01);
	if (v < 0.0 || u + v > 1.0) return false;

	/* The ray is within the triangle, test the intersection point */
	double t = inv_det * glm::dot(e02, s_cross_e01);
	if (t < Eps) return false;
	
	hrec.t = t;
	hrec.posn = model_ray.At(t);
	hrec.material = material;
	Vec3 normal = ComputeInterpolatedNormal(u, v);
	hrec.SetFaceNormal(model_ray.direction, normal);
	hrec.transform = transform;

	hrec.u = u;
	hrec.v = v;

	return true;
}

void Triangle::SetBoundingBox()
{
	/* Transform triangle vertices to world space */
	Point3 w_v0p = transform.model_to_world * Vec4(v0p, 1.0);
	Point3 w_v1p = transform.model_to_world * Vec4(v1p, 1.0);
	Point3 w_v2p = transform.model_to_world * Vec4(v2p, 1.0);

	/* Create a bounding box enclosing world space bounds of the triangle's vertices */
	bounding_box = AABB(AABB(w_v0p, w_v1p), AABB(w_v0p, w_v2p));
}


Vec3 Triangle::ComputeInterpolatedNormal(double u, double v) const
{
	return v0n + u * (v1n - v0n) + v * (v2n - v0n);
}

/* ============================== */
/* ====== Constant Mediums ====== */
/* ============================== */

ConstantMedium::ConstantMedium(std::shared_ptr<Hittable> boundary, double density, std::shared_ptr<Texture> texture)
	: boundary(boundary), neg_inv_density(-1.0 / density), phase_function(std::make_shared<Isotropic>(texture))
{
	SetBoundingBox();
}

ConstantMedium::ConstantMedium(std::shared_ptr<Hittable> boundary, double density, const Color& albedo)
	: boundary(boundary), neg_inv_density(-1.0 / density), phase_function(std::make_shared<Isotropic>(albedo))
{
	SetBoundingBox();
}

bool ConstantMedium::Hit(const Ray& ray, Interval ray_t, HitRecord& hrec) const
{
	/* We need to determine the entry and exit points of the ray along the boundary */
	HitRecord hrec1, hrec2;

	/* If the ray does not intersect with the boundary at all, return */
	if (!boundary->Hit(ray, Interval(-Inf, Inf), hrec1)) return false;

	/* If the ray does not *exit*  the boundary, return */
	if (!boundary->Hit(ray, Interval(hrec1.t + Eps, Inf), hrec2)) return false;

	/* Bounds check the entry and exit points */
	if (hrec1.t < ray_t.min) hrec1.t = ray_t.min;
	if (hrec2.t > ray_t.max) hrec2.t = ray_t.max;
	if (hrec1.t >= hrec2.t) return false;
	if (hrec1.t < 0.0) hrec1.t = 0.0;

	/* Determine at what point inside the bounds the ray will scatter (or if it will pass through) */
	Ray model_ray = boundary->transform.WorldToModel(ray);
	double ray_length = glm::length(model_ray.direction);
	double distance_inside_boundary = (hrec2.t - hrec1.t) * ray_length;
	double hit_distance = neg_inv_density * std::log(RandomDouble());

	if (hit_distance > distance_inside_boundary) return false;

	hrec.t = hrec1.t + hit_distance / ray_length;
	hrec.posn = model_ray.At(hrec.t);
	hrec.material = phase_function;
	hrec.transform = boundary->transform;

	/* arbitrary... */
	hrec.normal = Vec3(0.0, 0.0, 1.0);
	hrec.front_face = true;

	return true;
}

void ConstantMedium::SetBoundingBox()
{
	bounding_box = boundary->BoundingBox();
}

/* =========================== */
/* ====== Hittable List ====== */
/* =========================== */

HittableList::HittableList(std::shared_ptr<Hittable> hittable)
{
	Add(hittable);
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

bool HittableList::Hit(const Ray& ray, Interval ray_t, HitRecord& hrec) const
{
	HitRecord temp_hrec;
	bool hit_anything = false;
	double closest_so_far = ray_t.max;

	for (const auto& hittable : objects)
	{
		if (hittable->Hit(ray, Interval(ray_t.min, closest_so_far), temp_hrec))
		{
			hit_anything = true;
			closest_so_far = temp_hrec.t;
			hrec = temp_hrec;
		}
	}

	return hit_anything;
}


double HittableList::PDF_Value(const Point3& origin, const Vec3& direction) const
{
	double inv_length = 1.0 / (double)objects.size();
	double value = 0.0;

	for (const auto& object : objects)
	{
		value += inv_length * object->PDF_Value(origin, direction);
	}

	return value;
}


Vec3 HittableList::Random(const Point3& origin) const
{
	int index = (int)(RandomDouble() * (int)objects.size());
	return objects[index]->Random(origin);
}


/* ============================= */
/* ====== Compound Shapes ====== */
/* ============================= */

/* === Boxes (cubes) === */
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

std::shared_ptr<rt::HittableList> Box(const Transform& t_transform, std::shared_ptr<Material> material)
{
	auto sides = std::make_shared<HittableList>();

	sides->Add(std::make_shared<Parallelogram>(Transform(t_transform.model_to_world * glm::translate(Vec3(0.0, 0.0, 0.5))), material)); /* top */
	sides->Add(std::make_shared<Parallelogram>(Transform(t_transform.model_to_world * glm::translate(Vec3(0.0, 0.0, -0.5)) * glm::rotate(Pi, Vec3(1.0, 0.0, 0.0))), material)); /* bottom */
	sides->Add(std::make_shared<Parallelogram>(Transform(t_transform.model_to_world * glm::translate(Vec3(0.0, -0.5, 0.0)) * glm::rotate(Pi / 2.0, Vec3(1.0, 0.0, 0.0))), material)); /* left */
	sides->Add(std::make_shared<Parallelogram>(Transform(t_transform.model_to_world * glm::translate(Vec3(0.0, 0.5, 0.0)) * glm::rotate(-Pi / 2.0, Vec3(1.0, 0.0, 0.0))), material)); /* right */
	sides->Add(std::make_shared<Parallelogram>(Transform(t_transform.model_to_world * glm::translate(Vec3(-0.5, 0.0, 0.0)) * glm::rotate(-Pi / 2.0, Vec3(0.0, 1.0, 0.0))), material)); /* back */
	sides->Add(std::make_shared<Parallelogram>(Transform(t_transform.model_to_world * glm::translate(Vec3(0.5, 0.0, 0.0)) * glm::rotate(Pi / 2.0, Vec3(0.0, 1.0, 0.0))), material)); /* front */

	return sides;
}

/* === Triangle Meshes === */
HittableList LoadMesh(const Transform& t_transform, const std::string& filepath, std::shared_ptr<Material> material)
{
	/* Store mesh as a hittable list */
	HittableList hittable_mesh;

	/* Load mesh triangle vertices from file */
	objl::Loader loader;
	bool loadout = loader.LoadFile("../RayTracer/res/meshes/" + filepath);
	if (!loadout)
	{
		std::cout << "[rt::LoadMesh] ERROR! Failed to load mesh file '" << filepath << "'" << std::endl;
		return hittable_mesh;
	}

	if (loader.LoadedMeshes.size() > 1)
	{
		std::cout << "[rt::LoadMesh] WARNING: More than 1 mesh found in file '" << filepath << "'. Loading only the first mesh!" << std::endl;
	}

	objl::Mesh mesh = loader.LoadedMeshes[0];

	/* Iterate through all indices and create triangles with corresponding vertices */
	for (int i = 0; i < mesh.Indices.size(); i += 3)
	{
		auto v0 = mesh.Vertices[mesh.Indices[i + 0]];
		auto v1 = mesh.Vertices[mesh.Indices[i + 1]];
		auto v2 = mesh.Vertices[mesh.Indices[i + 2]];
		hittable_mesh.Add(std::make_shared<Triangle>(t_transform, v0, v1, v2, material));
	}

	return hittable_mesh;
}

}