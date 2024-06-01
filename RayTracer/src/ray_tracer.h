#pragma once

#include "common.h"

#include "cameras.h"
#include "renderer.h"
#include "interaction.h"
#include "hittable.h"
#include "material.h"
#include "bvh.h"
#include "utils.h"

/* This header file is what provides the interface for the ray tracer to other programs. */

namespace rt 
{

/* Pass in the scene and render with this camera */
std::vector<unsigned char> RayTrace(HittableList* world, Camera* camera)
{
	return Render(*world, *camera);
}

enum Scenes
{
	BasicMaterials,
	ScatteredSpheres,
	BouncingSpheres,
	Earth,
	PerlinSpheres,
};

/* Generate one of the default scenes */
HittableList GenerateScene(Scenes scene)
{
	HittableList world;

	switch (scene)
	{

	case BasicMaterials:
	{
		/* Ground sphere */
		auto material_ground = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1000.0), 1000.0, material_ground));

		/* Add in some custom larger spheres */
		auto material1 = std::make_shared<Dielectric>(1.5);
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, 1.0), 1.0, material1));

		auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
		world.Add(std::make_shared<Sphere>(Point3(-4.0, 0.0, 1.0), 1.0, material2));

		auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
		world.Add(std::make_shared<Sphere>(Point3(4.0, 0.0, 1.0), 1.0, material3));
	}

	case ScatteredSpheres:
	{
		/* Ground sphere */
		auto material_ground = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1000.0), 1000.0, material_ground));

		/* Add some random spheres */
		for (int a = -11; a < 11; a++)
		{
			for (int b = -11; b < 11; b++)
			{
				double choose_mat = RandomDouble();
				double choose_bounce = RandomDouble();
				Point3 center = Point3(a, b, 0.2) + Point3(0.4 * RandomInUnitDisk(), 0.0);

				/* Make sure these spheres do not intersect with the custom spheres */
				if (glm::length(center - Point3(4.0, 0.0, 1.0)) > 1.2 
					&& glm::length(center - Point3(0.0, 0.0, 1.0)) > 1.2 
					&& glm::length(center - Point3(-4.0, 0.0, 1.0)) > 1.2)
				{
					std::shared_ptr<Material> sphere_material;

					if (choose_mat < 0.6)
					{
						/* Diffuse */
						Color albedo = RandomVec3() * RandomVec3();
						sphere_material = std::make_shared<Lambertian>(albedo);

					}
					else if (choose_mat < 0.95)
					{
						/* Metal */
						Color albedo = RandomVec3(0.5, 1.0);
						double fuzz = RandomDouble(0, 0.5);
						sphere_material = std::make_shared<Metal>(albedo, fuzz);
					}
					else
					{
						/* Glass */
						sphere_material = std::make_shared<Dielectric>(1.5);
					}

					world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
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

	case BouncingSpheres:
	{
		/* Ground sphere with checker texture */
		auto checker_texture = std::make_shared<CheckerTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1000.0), 1000.0, std::make_shared<Lambertian>(checker_texture)));

		/* Add some random spheres */
		for (int a = -11; a < 11; a++)
		{
			for (int b = -11; b < 11; b++)
			{
				double choose_mat = RandomDouble();
				double choose_bounce = RandomDouble();
				Point3 center = Point3(a, b, 0.2) + Point3(0.4 * RandomInUnitDisk(), 0.0);

				/* Make sure these spheres do not intersect with the custom spheres */
				if (glm::length(center - Point3(4.0, 0.0, 1.0)) > 1.2
					&& glm::length(center - Point3(0.0, 0.0, 1.0)) > 1.2
					&& glm::length(center - Point3(-4.0, 0.0, 1.0)) > 1.2)
				{
					std::shared_ptr<Material> sphere_material;

					if (choose_mat < 0.6)
					{
						/* Diffuse */
						Color albedo = RandomVec3() * RandomVec3();
						sphere_material = std::make_shared<Lambertian>(albedo);

					}
					else if (choose_mat < 0.9)
					{
						/* Metal */
						Color albedo = RandomVec3(0.5, 1.0);
						double fuzz = RandomDouble(0, 0.5);
						sphere_material = std::make_shared<Metal>(albedo, fuzz);
					}
					else
					{
						/* Glass */
						sphere_material = std::make_shared<Dielectric>(1.5);
					}

					world.Add(std::make_shared<Sphere>(
						center,
						choose_bounce < 0.3 ? center + Vec3(0.0, 0.0, RandomDouble(0.0, 0.2)) : center,
						0.2,
						sphere_material)
					);
				}
			}
		}

		/* Add in some custom larger spheres */
		auto material1 = std::make_shared<Dielectric>(1.5);
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, 1.0), 1.0, material1));

		auto earth_texture = std::make_shared<ImageTexture>("earthmap.jpg");
		auto earth_surface = std::make_shared<Lambertian>(earth_texture);
		world.Add(std::make_shared<Sphere>(Point3(-4.0, 0.0, 1.0), 1.0, earth_surface));

		auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
		world.Add(std::make_shared<Sphere>(Point3(4.0, 0.0, 1.0), 1.0, material3));

		break;
	}

	case Earth:
	{
		auto earth_texture = std::make_shared<ImageTexture>("earthmap.jpg");
		auto earth_surface = std::make_shared<Lambertian>(earth_texture);
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, 0.0), 2.0, earth_surface));
		break;
	}

	case PerlinSpheres:
	{
		auto perlin_texture = std::make_shared<PerlinTexture>(4.0);
		auto turbulence_texture = std::make_shared<TurbulenceTexture>(4.0);
		auto marble_texture = std::make_shared<MarbleTexture>(4.0);
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1000.0), 1000.0, std::make_shared<Lambertian>(perlin_texture)));
		world.Add(std::make_shared<Sphere>(Point3(4.0, 4.0, 2.0), 2.0,std::make_shared<Lambertian>(turbulence_texture)));
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, 2.0), 2.0, std::make_shared<Lambertian>(marble_texture)));
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