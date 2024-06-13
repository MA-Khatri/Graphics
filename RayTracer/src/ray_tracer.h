#pragma once

#include "common.h"

#include "cameras.h"
#include "renderer.h"
#include "hit_record.h"
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
	Showcase0,
	TriangleMesh,
};

/* Generate one of the default scenes */
Scene GenerateScene(Scenes scene)
{
	HittableList world;
	HittableList lights;
	Texture* sky = new SolidColor(0.7, 0.8, 1.0);

	switch (scene)
	{

	case BasicMaterials:
	{
		sky = new ImageTexture("overcast_soil_puresky_4k.hdr");

		/* Ground plane with checker texture */
		auto checker_texture = std::make_shared<CheckerTexture>(2.5, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
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

		auto material4 = std::make_shared<Metal>(Color(0.5, 0.6, 0.7), 0.0001);
		Transform t4;
		t4.Translate(-10.0, 0.0, 5.0);
		t4.Rotate(90.0, Vec3(0.0, 1.0, 0.0));
		t4.Scale(10.0, 20.0, 1.0);
		world.Add(std::make_shared<Parallelogram>(t4, material4));

		Transform t5;
		t5.Translate(0.0, -5.0, 8.0);
		t5.Rotate(45.0, Vec3(0.0, 1.0, 1.0));
		t5.Scale(4.0);
		world.Add(std::make_shared<ConstantMedium>(std::make_shared<HittableList>(Box(t5, material3)), 0.1, Color(0.0)));

		Transform t6;
		world.Add(std::make_shared<Triangle>(t6, Point3(0.0, -1.0, 1.0), Point3(0.0, 1.0, 1.0), Point3(1.0, 0.0, 1.0), material4));

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
		/* Background */
		//sky = new SolidColor(0.0, 0.0, 0.0);
		sky = new ImageTexture("overcast_soil_puresky_4k.hdr");

		/* Materials */
		auto red     = std::make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
		auto white   = std::make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
		auto green   = std::make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
		auto light   = std::make_shared<DiffuseLight>(Color(15.0, 15.0, 15.0));
		auto glass   = std::make_shared<Dielectric>(1.5);
		auto bubble  = std::make_shared<Dielectric>(0.666666);
		auto checker = std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(1.0, Color(0.1, 0.1, 0.4), Color(0.73, 0.73, 0.73)));

		/* Cornell box */
		Transform left_t;
		left_t.Translate(0.0, -5.0, 5.0);
		left_t.Rotate(-90.0, Vec3(1.0, 0.0, 0.0));
		left_t.Scale(10.0);
		world.Add(std::make_shared<Parallelogram>(left_t, green));

		Transform right_t;
		right_t.Translate(0.0, 5.0, 5.0);
		right_t.Rotate(90.0, Vec3(1.0, 0.0, 0.0));
		right_t.Scale(10.0);
		world.Add(std::make_shared<Parallelogram>(right_t, red));

		Transform bottom_t;
		bottom_t.Scale(10.0);
		world.Add(std::make_shared<Parallelogram>(bottom_t, white));

		Transform top_t;
		top_t.Translate(0.0, 0.0, 10.0);
		top_t.Rotate(180.0, Vec3(1.0, 0.0, 0.0));
		top_t.Scale(10.0);
		world.Add(std::make_shared<Parallelogram>(top_t, white));

		Transform back_t;
		back_t.Translate(-5.0, 0.0, 5.0);
		back_t.Rotate(-90.0, Vec3(0.0, 1.0, 0.0));
		back_t.Scale(10.0);
		world.Add(std::make_shared<Parallelogram>(back_t, checker));
		
		Transform light_t;
		light_t.Translate(0.0, 0.0, 10.0 - Eps);
		light_t.Rotate(180.0, Vec3(1.0, 0.0, 0.0));
		light_t.Scale(3.0);
		lights.Add(std::make_shared<Parallelogram>(light_t, light)); /* light */
		world.Add(std::make_shared<Parallelogram>(light_t, light));

		/* Cornell box contents */
		//auto material2 = std::make_shared<Dielectric>(1.5);
		//Transform t2;
		//t2.Translate(0.0, 0.0, 4.0);
		//t2.Scale(3.5);
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
		world.Add(std::make_shared<HittableList>(Box(box_t, red)));

		//Transform t;
		//t.Translate(0.0, 1.0, -1.0);
		//t.Rotate(120.0, Vec3(0.0, 0.0, 1.0));
		//t.Rotate(90.0, Vec3(1.0, 0.0, 0.0));
		//t.Scale(40.0);
		//auto bunny = LoadMesh(t, "stanford-bunny.obj", white);
		//world.Add(std::make_shared<BVH_Node>(bunny));

		//Transform t;
		//t.Rotate(120.0, Vec3(0.0, 0.0, 1.0));
		//t.Rotate(90.0, Vec3(1.0, 0.0, 0.0));
		//t.Scale(5.0);
		//auto mesh = LoadMesh(t, "dragon.obj", red);
		//world.Add(std::make_shared<BVH_Node>(mesh));

		break;
	}

	case Showcase0:
	{
		/* Black background */
		sky = new ImageTexture("milky_way.jpg");

		/* Single large diffuse overhead light */
		auto light_material = std::make_shared<DiffuseLight>(Color(7.0));
		Transform light_t;
		light_t.Translate(0.0, 0.0, 15.0);
		light_t.Scale(10.0);
		world.Add(std::make_shared<Parallelogram>(light_t, light_material));

		/* Fog throughout the scene */
		auto white_material = std::make_shared<Lambertian>(Color(0.73));
		Transform fog_t;
		fog_t.Scale(1000.0);
		world.Add(std::make_shared<ConstantMedium>(std::make_shared<HittableList>(Box(fog_t, white_material)), 0.0001, Color(0.1)));

		/* Create a 'ground' out of multiple staggered height boxes */
		HittableList ground;
		auto ground_material = std::make_shared<Lambertian>(Color(0.48, 0.83, 0.53));
		int boxes_per_side = 20;
		for (int i = 0; i < boxes_per_side; i++)
		{
			for (int j = 0; j < boxes_per_side; j++)
			{
				double w = 2.0; /* scale */

				/* Determine the origin of the box */
				double x = (-(boxes_per_side / 2) + i + 0.5) * w;
				double y = (-(boxes_per_side / 2) + j + 0.5) * w;
				double z = RandomDouble(-w / 4.0, w / 4.0);

				Transform t;
				t.Translate(x, y, z);
				t.Scale(w);

				ground.Add(Box(t, ground_material));
			}
		}
		/* Create a BVH of these boxes and add to the world */
		world.Add(std::make_shared<BVH_Node>(ground));

		/* Make a rotated 'box' of lambertian spheres */
		HittableList box_of_spheres;
		Transform bs_t;
		bs_t.Translate(0.0, 6.0, 9.0);
		bs_t.Rotate(45.0, Vec3(0.0, 1.0, 1.0));
		bs_t.Scale(2.5);
		int n_spheres = 1000;
		for (int i = 0; i < n_spheres; i++)
		{
			/* Transform for this circle */
			Transform s_t;
			s_t.Translate(RandomVec3(-1.0, 1.0));
			s_t.Scale(0.05);

			/* Combined transform */
			Transform bss_t(bs_t.model_to_world * s_t.model_to_world);

			box_of_spheres.Add(std::make_shared<Sphere>(bss_t, white_material));
		}
		world.Add(std::make_shared<BVH_Node>(box_of_spheres));

		/* Motion blur sphere */
		auto blur_material = std::make_shared<Lambertian>(Color(0.9, 0.4, 0.6));
		Transform blur_t;
		blur_t.Translate(2.0, 2.5, 2.0);
		blur_t.Scale(1.0);
		world.Add(std::make_shared<Sphere>(blur_t, Vec3(0.0, 1.0, 0.0), blur_material));

		/* Globe */
		auto globe_material = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("earthmap.jpg"));
		Transform globe_t;
		globe_t.Translate(-7.0, -7.0, 6.0);
		globe_t.Scale(3.0);
		world.Add(std::make_shared<Sphere>(globe_t, globe_material));

		/* Glass ball */
		auto glass_material = std::make_shared<Dielectric>(1.5);
		Transform glass_t;
		glass_t.Translate(4.0, -2.0, 3.5);
		glass_t.Scale(1.75);
		world.Add(std::make_shared<Sphere>(glass_t, glass_material));

		/* Metal spheroid */
		auto smooth_metal_material = std::make_shared<Metal>(Color(0.7, 0.5, 0.4), 0.0);
		Transform ms_t;
		ms_t.Translate(-4.0, 4.0, 4.0);
		ms_t.Rotate(30.0, Vec3(0.0, 0.0, 1.0));
		ms_t.Scale(5.0, 2.0, 2.0);
		world.Add(std::make_shared<Sphere>(ms_t, smooth_metal_material));

		/* Noise spheroid */
		auto turbulence_texture = std::make_shared<TurbulenceTexture>(100.0);
		auto turbulence_material = std::make_shared<Lambertian>(turbulence_texture);
		Transform mt_t;
		mt_t.Translate(3.0, 6.0, 3.0);
		mt_t.Scale(1.0, 1.0, 1.5);
		world.Add(std::make_shared<Sphere>(mt_t, turbulence_material));

		/* Fog cuboid */
		Transform c_t;
		c_t.Translate(-12.0, 0.0, 8.0);
		c_t.Rotate(-60.0, Vec3(0.0, 1.0, 1.0));
		c_t.Scale(4.0);
		world.Add(std::make_shared<ConstantMedium>(std::make_shared<HittableList>(Box(c_t, white_material)), 0.2, Color(0.73)));

		break;
	}

	case TriangleMesh:
	{
		sky = new ImageTexture("overcast_soil_puresky_4k.hdr");

		/* Materials */
		auto checker_texture = std::make_shared<CheckerTexture>(2.5, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
		auto checker_material = std::make_shared<Lambertian>(checker_texture);
		auto red = std::make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
		auto smooth_metal = std::make_shared<Metal>(Color(0.7, 0.5, 0.4), 0.0);
		auto rough_metal = std::make_shared<Metal>(Color(0.7, 0.5, 0.4), 0.1);
		auto glass = std::make_shared<Dielectric>(1.5);

		/* Ground plane */
		Transform tg;
		tg.Scale(100.0);
		world.Add(std::make_shared<Parallelogram>(tg, checker_material));

		/* Mesh */
		Transform t;

		//t.Rotate(90.0, Vec3(0.0, 0.0, 1.0));
		//t.Scale(6.0);
		//auto mesh = LoadMesh(t, "stanford-bunny-s.obj", glass);

		t.Rotate(120.0, Vec3(0.0, 0.0, 1.0));
		t.Rotate(90.0, Vec3(1.0, 0.0, 0.0));
		t.Scale(6.0);
		auto mesh = LoadMesh(t, "dragon.obj", glass);

		//t.Rotate(180.0, Vec3(0.0, 0.0, 1.0));
		//t.Scale(6.0);
		//auto mesh = LoadMesh(t, "low-poly-bunny.obj", red);
		
		world.Add(std::make_shared<BVH_Node>(mesh));

		break;
	}

	default:
	{
		break;
	}

	}

	/* Construct BVH */
	world = HittableList(std::make_shared<BVH_Node>(world));

	return Scene(world, lights, sky);
}

} /* namespace rt */