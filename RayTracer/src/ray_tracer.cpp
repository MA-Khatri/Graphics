#include "ray_tracer.h"

namespace RayTracer {

std::vector<unsigned char> RayTrace(Camera* camera)
{
	/* World setup */
	ShapesList world;
	world.Add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
	world.Add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));

	/* Pass in the scene and render with this camera */
	return camera->Render(world);
}

} /* namespace RayTracer */