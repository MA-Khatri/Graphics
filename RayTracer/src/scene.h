#pragma once

#include "hittable.h"
#include "texture.h"

namespace rt
{

class Scene
{
public:
	Scene(HittableList& world, Texture* sky) : world(world), sky(sky) {}

	Color SampleSky(const Ray& ray) const
	{
		/* Convert the ray's direction to UV coordinates */
		double u = 0.5 * (1.0 + (std::atan2(ray.direction.y, ray.direction.x) * InvPi));
		double v = std::atan2(glm::length(Vec2(ray.direction.x, ray.direction.y)), ray.direction.z) * InvPi;

		return sky->Value(u, v, ray.origin); /* Note the ray.origin is not used for most sky textures... Maybe later? */
	}

	inline HittableList World() const { return world; }

private:
	HittableList world;
	Texture* sky;
};

}