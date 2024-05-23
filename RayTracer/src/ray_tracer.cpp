#include "ray_tracer.h"

namespace RayTracer {

std::vector<unsigned char> RayTrace(Camera* camera)
{
	/* World setup */
	ShapesList world;

	auto material_default = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
	auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
	auto material_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
	auto material_left = make_shared<Metal>(Color(0.8, 0.8, 0.8));
	auto material_right = make_shared<Metal>(Color(0.8, 0.6, 0.2));

	world.Add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.Add(make_shared<Sphere>(Point3(0.0, 0.0, -1.2), 0.5, material_center));
	world.Add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.Add(make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, material_right));

	/* Pass in the scene and render with this camera */
	return camera->Render(world);
}

} /* namespace RayTracer */