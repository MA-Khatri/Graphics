#pragma once

#include "common.h"

#include "camera.h"
#include "interaction.h"
#include "shapes.h"
#include "material.h"
#include "utils.h"

namespace RT {

std::vector<unsigned char> RayTrace(Camera* camera, ShapesList* world);

} /* namespace RT */