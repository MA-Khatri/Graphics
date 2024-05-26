#pragma once

#include "common.h"

#include "cameras.h"
#include "renderer.h"
#include "interaction.h"
#include "shapes.h"
#include "material.h"
#include "utils.h"

/* This header file is what provides the interface for the ray tracer to other programs. */

namespace rt 
{

/* Pass in the scene and render with this camera */
std::vector<unsigned char> RayTrace(ShapesList* world, Camera* camera)
{
	return Render(*world, *camera);
}

} /* namespace rt */