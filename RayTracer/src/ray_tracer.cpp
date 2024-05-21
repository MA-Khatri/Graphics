#include "ray_tracer.h"

namespace RayTracer {

std::vector<unsigned char> RayTrace(unsigned int width, unsigned int height, float yfov)
{
	/* World setup */
	ShapesList world;
	world.Add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
	world.Add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));

	/* Camera */
	Camera cam;
	cam.image_width = width;
	cam.image_height = height;

	/* Pass in the scene and render with this camera */
	return cam.Render(world);
}

} /* namespace RayTracer */