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
std::vector<unsigned char> RayTrace(Scene* scene, Camera* camera)
{
	return Render(*scene, *camera);
}

enum Scenes
{
	BasicMaterials,
	ScatteredSpheres,
	BouncingSpheres,
	Earth,
	PerlinSpheres,
	Parallelograms,
	CornellBox,
};

/* Generate one of the default scenes */
Scene GenerateScene(Scenes scene)
{
	HittableList world;
	Texture* sky = new SolidColor(0.7, 0.8, 1.0);

	switch (scene)
	{

	case BasicMaterials:
	{
		/* Ground plane with checker texture */
		auto checker_texture = std::make_shared<CheckerTexture>(1.0/100.0, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
		Transform tg;
		tg.Scale(100.0);
		world.Add(std::make_shared<Parallelogram>(tg, std::make_shared<Lambertian>(checker_texture)));
		//world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1000.0), 1000.0, std::make_shared<Lambertian>(checker_texture)));

		/* Add in some custom larger spheres */
		auto material1 = std::make_shared<Lambertian>(std::make_shared<TurbulenceTexture>(4.0));
		Transform t1;
		t1.Translate(0.0, -6.0, 2.0);
		t1.Rotate(45.0, Vec3(0.0, 0.0, 1.0));
		t1.Scale(2.0, 4.0, 2.0);
		world.Add(std::make_shared<Sphere>(t1, material1));

		auto material2 = std::make_shared<Dielectric>(1.5);
		Transform t2;
		t2.Translate(0.0, 0.0, 4.0);
		t2.Scale(3.0, 2.0, 3.0);
		world.Add(std::make_shared<Sphere>(t2, material2));

		auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
		Transform t3;
		t3.Translate(0.0, 5.0, 4.0);
		t3.Rotate(35.0, Vec3(0.0, 1.0, 1.0));
		t3.Scale(4.0, 2.0, 4.0);
		world.Add(std::make_shared<Sphere>(t3, material3));

		//auto light = std::make_shared<DiffuseLight>(Color(10.0));
		//auto s4 = std::make_shared<Sphere>(light);
		//s4->transform.Translate(0.0, 0.0, 12.0);
		//s4->transform.Scale(3.0, 3.0, 1.0);
		//world.Add(s4);

		Transform box_t;
		box_t.Translate(3.0, 4.0, 3.0);
		box_t.Rotate(30.0, Vec3(0.0, 1.0, 1.0));
		box_t.Scale(3.0);
		world.Add(std::make_shared<HittableList>(Box(box_t, material3)));

		sky = new ImageTexture("overcast_soil_puresky_4k.hdr");

		break;
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
						choose_bounce < 0.3 ? center + Vec3(0.0, 0.0, RandomDouble(0.0, 2.0)) : center,
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

		auto diffuse_light = std::make_shared<DiffuseLight>(Color(10.0, 10.0, 10.0));
		world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, 8.0), 2.0, diffuse_light));

		sky = new ImageTexture("overcast_soil_puresky_4k.hdr");

		break;
	}

	case Parallelograms:
	{
		/* Materials */ 
		auto left_red = std::make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
		auto back_green = std::make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
		auto right_blue = std::make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
		auto upper_orange = std::make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
		auto lower_teal = std::make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

		/* Parallelograms */
		world.Add(std::make_shared<Parallelogram>(Point3(-3.0, -2.0, 5.0), Vec3(0.0, 0.0, -4.0), Vec3(0.0, 4.0, 0.0), left_red));
		world.Add(std::make_shared<Parallelogram>(Point3(-2.0, -2.0, 0.0), Vec3(4.0, 0.0, 0.0), Vec3(0.0, 4.0, 0.0), back_green));
		world.Add(std::make_shared<Parallelogram>(Point3(3.0, -2.0, 1.0), Vec3(0.0, 0.0, 4.0), Vec3(0.0, 4.0, 0.0), right_blue));
		world.Add(std::make_shared<Parallelogram>(Point3(-2.0, 3.0, 1.0), Vec3(4.0, 0.0, 0.0), Vec3(0.0, 0.0, 4.0), upper_orange));
		world.Add(std::make_shared<Parallelogram>(Point3(-2.0, -3.0, 5.0), Vec3(4.0, 0.0, 0.0), Vec3(0.0, 0.0, -4.0), lower_teal));

		break;
	}

	case CornellBox:
	{
		/* Black background */
		sky = new SolidColor(0.0, 0.0, 0.0);

		/* Materials */
		auto red     = std::make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
		auto white   = std::make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
		auto green   = std::make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
		auto light   = std::make_shared<DiffuseLight>(Color(15.0, 15.0, 15.0));
		auto glass   = std::make_shared<Dielectric>(1.5);
		auto bubble  = std::make_shared<Dielectric>(0.666666);
		auto checker = std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(2.5, Color(0.1, 0.1, 0.4), Color(0.73, 0.73, 0.73)));

		/* Cornell box */
		world.Add(std::make_shared<Parallelogram>(Point3(5.0, -5.0, 0.0), Vec3(-10.0, 0.0, 0.0), Vec3(0.0, 0.0, 10.0), green)); /* left */
		world.Add(std::make_shared<Parallelogram>(Point3(5.0, 5.0, 0.0), Vec3(-10.0, 0.0, 0.0), Vec3(0.0, 0.0, 10.0), red)); /* right */
		world.Add(std::make_shared<Parallelogram>(Point3(5.0, -5.0, 0.0), Vec3(-10.0, 0.0, 0.0), Vec3(0.0, 10.0, 0.0), checker)); /* bottom */
		world.Add(std::make_shared<Parallelogram>(Point3(5.0, -5.0, 10.0), Vec3(-10.0, 0.0, 0.0), Vec3(0.0, 10.0, 0.0), white)); /* top */
		world.Add(std::make_shared<Parallelogram>(Point3(-5.0, -5.0, 0.0), Vec3(0.0, 10.0, 0.0), Vec3(0.0, 0.0, 10.0), white)); /* back */
		world.Add(std::make_shared<Parallelogram>(Point3(-1.5, -1.5, 10.0-Eps), Vec3(3.0, 0.0, 0.0), Vec3(0.0, 3.0, 0.0), light)); /* light */

		/* Cornell box contents */
		//auto material2 = std::make_shared<Dielectric>(1.5);
		//Transform t2;
		//t2.Translate(0.0, 0.0, 4.0);
		//t2.Scale(3.0, 3.0, 1.0);
		//world.Add(std::make_shared<Sphere>(t2, material2));

		//auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
		//Transform t3;
		//t3.Translate(-2.0, 0.0, 4.0);
		//t3.Rotate(35.0, Vec3(0.0, 1.0, 1.0));
		//t3.Scale(4.0, 2.0, 4.0);
		//world.Add(std::make_shared<Sphere>(t3, material3));

		Transform box_t;
		box_t.Translate(0.0, 0.0, 3.0);
		box_t.Rotate(30.0, Vec3(0.0, 1.0, 1.0));
		box_t.Scale(3.0);
		world.Add(std::make_shared<HittableList>(Box(box_t, glass)));

		break;
	}

	default:
	{
		break;
	}

	}

	/* Construct BVH */
	world = HittableList(std::make_shared<BVH_Node>(world));

	return Scene(world, sky);
}

} /* namespace rt */