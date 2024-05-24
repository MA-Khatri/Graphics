#include "ray_tracer.h"

namespace RT {

std::vector<unsigned char> RayTrace(Camera* camera, ShapesList* world)
{
	/* Pass in the scene and render with this camera */
	return camera->Render(*world);
}

} /* namespace RT */