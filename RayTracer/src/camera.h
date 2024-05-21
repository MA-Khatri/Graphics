#pragma once

#include "shapes.h"

#include <algorithm>
#include <execution>

namespace RayTracer {

class Camera
{
public:

	unsigned int image_width = 100;
	unsigned int image_height = 100;
	unsigned int samples_per_pixel = 10;

	std::vector<unsigned char> Render(const Shape& world)
	{
		Initialize();



		/* Main (parallelized) ray tracing loop */
		std::vector<unsigned char> image(image_width * image_height * 3);
		std::for_each(std::execution::par, vertical_iter.begin(), vertical_iter.end(), [this, &world, &image](unsigned int j) {
			std::for_each(std::execution::par, horizontal_iter.begin(), horizontal_iter.end(), [this, &world, &image, j](unsigned int i) {
				
				//Color pixel_color(0.0, 0.0, 0.0);

				//for (unsigned int sample = 0; sample < samples_per_pixel; sample++)
				//{
				//	/* TODO */
				//}
				auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
				auto ray_direction = pixel_center - camera_center;
				Ray ray(camera_center, ray_direction);

				Color pixel_color = RayColor(ray, world);
				auto r = pixel_color.x();
				auto g = pixel_color.y();
				auto b = pixel_color.z();

				unsigned int pixel = 3 * (j * image_width + i); /* index to start of pixel */
				static const Interval intensity(0.000, 0.999);
				image[pixel + 0] = (unsigned char)(256 * intensity.Clamp(r));
				image[pixel + 1] = (unsigned char)(256 * intensity.Clamp(g));
				image[pixel + 2] = (unsigned char)(256 * intensity.Clamp(b));
			});
		});

		return image;
	}

private:
	double aspect_ratio = 1.0; /* Ratio of image width over image height */
	Point3 camera_center; /* I.e., origin of the camera */
	Point3 pixel00_loc; /* Location of pixel (0, 0) */
	Vec3 pixel_delta_u; /* Horizontal per-pixel offset */
	Vec3 pixel_delta_v; /* Vertical per-pixel offset */

	std::vector<unsigned int> horizontal_iter; /* Horizontal iterator used for std::for_each */
	std::vector<unsigned int> vertical_iter; /* Vertical iterator used for std::for_each */

	void Initialize()
	{
		/* Set up iterators for std::foreach */
		horizontal_iter = std::vector<unsigned int>(image_width);
		vertical_iter = std::vector<unsigned int>(image_height);
		for (unsigned int i = 0; i < image_width; i++) horizontal_iter[i] = i;
		for (unsigned int i = 0; i < image_height; i++) vertical_iter[i] = i;

		/* Determine aspect ratio of image -- Note: not currently used */
		aspect_ratio = double(image_width) / double(image_height);

		/* Set camera origin to world origin (for now) */
		camera_center = Point3(0.0, 0.0, 0.0);

		/* Determine viewport dimensions */
		double focal_length = 1.0;
		double viewport_height = 2.0;
		double viewport_width = viewport_height * aspect_ratio;

		/* Calculate the vectors across the horizontal and down the vertical viewport edges. */
		auto viewport_u = Vec3(viewport_width, 0.0, 0.0);
		auto viewport_v = Vec3(0.0, -viewport_height, 0.0);

		/* Calculate the horizontal and vertical delta vectors from pixel to pixel. */
		pixel_delta_u = viewport_u / double(image_width);
		pixel_delta_v = viewport_v / double(image_height);

		/* Calculate the location of the upper left pixel. */
		auto viewport_upper_left = camera_center - Vec3(0.0, 0.0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
	}

	Color RayColor(const Ray& ray, const Shape& world) const
	{
		Interaction interaction;
		if (world.Intersect(ray, Interval(0.0, infinity), interaction))
		{
			return 0.5 * (interaction.normal + Color(1.0, 1.0, 1.0));
		}

		/* Default sky background */
		Vec3 unit_direction = Normalize(ray.direction);
		auto a = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
	}
};

} /* namespace RayTracer */