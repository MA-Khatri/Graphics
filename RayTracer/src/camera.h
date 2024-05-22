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

	std::vector<unsigned char> Render(const Shape& world)
	{
		/* Initialize the image that will be returned */
		std::vector<unsigned char> image(image_width * image_height * 3);

#define MULTI false
#if MULTI
		/* Main (parallelized) ray tracing loop */
		std::for_each(std::execution::par, vertical_iter.begin(), vertical_iter.end(), [this, &world, &image](unsigned int j) {
			std::for_each(std::execution::par, horizontal_iter.begin(), horizontal_iter.end(), [this, &world, &image, j](unsigned int i) {
				
				/* Determine the index to start of this pixel */
				unsigned int pixel = 3 * (j * image_width + i);

				/* Create a ray with a random offset within the pixel */
				Ray ray = GetRay(i, j);

				/* Trace ray and determine new color */
				Color pixel_color = RayColor(ray, world);
				auto r = pixel_color.x();
				auto g = pixel_color.y();
				auto b = pixel_color.z();

				/* Extract accumulated color */
				double cr = image_accumulator[pixel + 0];
				double cg = image_accumulator[pixel + 1];
				double cb = image_accumulator[pixel + 2];

				/* Determine the new accumulated color */
				r = (r + current_samples * cr) / (current_samples + 1);
				g = (g + current_samples * cg) / (current_samples + 1);
				b = (b + current_samples * cb) / (current_samples + 1);

				/* Set the new accumulated values in the accumulator */
				image_accumulator[pixel + 0] = r;
				image_accumulator[pixel + 1] = g;
				image_accumulator[pixel + 2] = b;

				/* Clamp the color from 0-255 and set the pixel values to return */
				static const Interval intensity(0.000, 0.999);
				image[pixel + 0] = (unsigned char)(256 * intensity.Clamp(r));
				image[pixel + 1] = (unsigned char)(256 * intensity.Clamp(g));
				image[pixel + 2] = (unsigned char)(256 * intensity.Clamp(b));
			});
		});
#else
		for (unsigned int j = 0; j < image_height; j++)
		{
			for (unsigned int i = 0; i < image_width; i++)
			{
				/* Determine the index to start of this pixel */
				unsigned int pixel = 3 * (j * image_width + i);

				/* Create a ray with a random offset within the pixel */
				Ray ray = GetRay(i, j);

				/* Trace ray and determine new color */
				Color pixel_color = RayColor(ray, world);
				auto r = pixel_color.x();
				auto g = pixel_color.y();
				auto b = pixel_color.z();

				/* Extract accumulated color */
				double cr = image_accumulator[pixel + 0];
				double cg = image_accumulator[pixel + 1];
				double cb = image_accumulator[pixel + 2];

				/* Determine the new accumulated color */
				r = (r + current_samples * cr) / (current_samples + 1);
				g = (g + current_samples * cg) / (current_samples + 1);
				b = (b + current_samples * cb) / (current_samples + 1);

				/* Set the new accumulated values in the accumulator */
				image_accumulator[pixel + 0] = r;
				image_accumulator[pixel + 1] = g;
				image_accumulator[pixel + 2] = b;

				/* Clamp the color from 0-255 and set the pixel values to return */
				static const Interval intensity(0.000, 0.999);
				image[pixel + 0] = (unsigned char)(256 * intensity.Clamp(r));
				image[pixel + 1] = (unsigned char)(256 * intensity.Clamp(g));
				image[pixel + 2] = (unsigned char)(256 * intensity.Clamp(b));
			}
		}
#endif

		/* Iterate the sample count */
		current_samples++;

		return image;
	}

	void Initialize(unsigned int width, unsigned int height)
	{
		if (image_width != width || image_height != height)
		{
			image_width = width;
			image_height = height;

			/* Resize and reset the image_accumulator */
			image_accumulator = std::vector<double>(image_width * image_height * 3);
			current_samples = 0;
		}

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

	inline unsigned int GetSampleCount() const { return current_samples; }

private:
	double aspect_ratio = 1.0; /* Ratio of image width over image height */
	Point3 camera_center; /* I.e., origin of the camera */
	Point3 pixel00_loc; /* Location of pixel (0, 0) */
	Vec3 pixel_delta_u; /* Horizontal per-pixel offset */
	Vec3 pixel_delta_v; /* Vertical per-pixel offset */

	std::vector<unsigned int> horizontal_iter; /* Horizontal iterator used for std::for_each */
	std::vector<unsigned int> vertical_iter; /* Vertical iterator used for std::for_each */

	unsigned int current_samples; /* Used to determine previous sample contributions to the accumulated image */
	std::vector<double> image_accumulator; /* Stores results from all previous samples */


	Ray GetRay(unsigned int i, unsigned int j)
	{
		/* Generate a camera ray from the origin to a randomly sampled point around the pixel location i, j */
		Vec3 offset = SampleSquare();
		auto pixel_sample = pixel00_loc + (((double)i + offset.x()) * pixel_delta_u) + (((double)j + offset.y()) * pixel_delta_v);

		auto origin = camera_center;
		auto direction = pixel_sample - origin;

		return Ray(origin, direction);
	}

	Vec3 SampleSquare() const
	{
		/* Returns the vector to a random point in the [-0.5, -0.5] to [0.5, 0.5] unit square */
		return Vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0.0);
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