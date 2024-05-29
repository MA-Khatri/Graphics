#pragma once

#include "common.h"

#include "cameras.h"
#include "renderer.h"
#include "interaction.h"
#include "hittable.h"
#include "material.h"
#include "utils.h"
#include "bvh.h"

/* This header file is what provides the interface for the ray tracer to other programs. */

namespace rt 
{

/* Pass in the scene and render with this camera */
std::vector<unsigned char> RayTrace(HittableList* world, Camera* camera)
{
	return Render(*world, *camera);
}

/* Generate one of the default scenes */
HittableList GenerateScene(unsigned int i)
{
	HittableList world;

	switch (i)
	{

	case 1:
	{
		/* Ground sphere */
		auto material_ground = std::make_shared<Lambertian>(Color(0.5f, 0.5f, 0.5f));
		world.Add(std::make_shared<Sphere>(Point3(0.0f, 0.0f, -1000.0f), 1000.0f, material_ground));

		/* Add in some custom larger spheres */
		auto material1 = std::make_shared<Dielectric>(1.5);
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, 1.0), 1.0, material1));

		auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
		world.Add(std::make_shared<Sphere>(Point3(-4.0, 0.0, 1.0), 1.0, material2));

		auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
		world.Add(std::make_shared<Sphere>(Point3(4.0, 0.0, 1.0), 1.0, material3));
	}

	case 2:
	{
		/* Ground sphere */
		auto material_ground = std::make_shared<Lambertian>(Color(0.5f, 0.5f, 0.5f));
		world.Add(std::make_shared<Sphere>(Point3(0.0f, 0.0f, -1000.0f), 1000.0f, material_ground));

		/* Add some random spheres */
		for (int a = -5; a < 5; a++)
		{
			for (int b = -5; b < 5; b++)
			{
				double choose_mat = RandomDouble();
				Point3 center(a + 0.9 * RandomDouble(), b + 0.9 * RandomDouble(), 0.2);

				/* Make sure these spheres do not intersect with the custom spheres */
				if ((center - Point3(4.0, 0.0, 1.0)).length() > 1.2 
					&& (center - Point3(0.0, 0.0, 1.0)).length() > 1.2 
					&& (center - Point3(-4.0, 0.0, 1.0)).length() > 1.2)
				{
					std::shared_ptr<Material> sphere_material;

					if (choose_mat < 0.8)
					{
						/* Diffuse */
						Color albedo = RandomVec3() * RandomVec3();
						sphere_material = std::make_shared<Lambertian>(albedo);
						world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
					}
					else if (0.8 <= choose_mat < 0.95)
					{
						/* Metal */
						Color albedo = RandomVec3(0.5, 1.0);
						double fuzz = RandomDouble(0, 0.5);
						sphere_material = std::make_shared<Metal>(albedo, fuzz);
						world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
					}
					else
					{
						/* Glass */
						sphere_material = std::make_shared<Dielectric>(1.5);
						world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
					}
				}
			}
		}

		/* Add in some custom larger spheres */
		auto material1 = std::make_shared<Dielectric>(1.5);
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, 1.0), 1.0, material1));

		auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
		world.Add(std::make_shared<Sphere>(Point3(-4.0, 0.0, 1.0), 1.0, material2));

		auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
		world.Add(std::make_shared<Sphere>(Point3(4.0, 0.0, 1.0), 1.0, material3));

		break;
	}

	default:
	{
		break;
	}

	}

	/* Construct BVH */
	world = HittableList(std::make_shared<BVH_Node>(world));

	return world;
}

} /* namespace rt */