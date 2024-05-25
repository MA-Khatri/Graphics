#pragma once

#include "common.h"

#include "camera.h"
#include "interaction.h"
#include "shapes.h"
#include "material.h"
#include "utils.h"

/* This header file is what provides the interface for the ray tracer to other programs. */

namespace rt 
{

std::vector<unsigned char> RayTrace(Camera* camera, ShapesList* world)
{
	/* Pass in the scene and render with this camera */
	return camera->Render(*world);
}

} /* namespace rt */