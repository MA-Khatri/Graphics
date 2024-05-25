#pragma once

#include "common.h"
#include "shapes.h"
#include "material.h"

#include <algorithm>
#include <execution>

namespace rt 
{

class Camera
{
public:

	/* Image dimensions */
	unsigned int image_width = 100;
	unsigned int image_height = 100;

	/* View parameters */
	double vfov = 90.0; /* Vertical field of view */
	Point3 origin = Point3(0.0, 0.0, 0.0); /* Point the camera is looking from */
	Point3 look_at = Point3(0.0, 0.0, -1.0); /* Point the camera is looking at */
	Vec3 up = Vec3(0.0, 1.0, 0.0); /* Camera-relative "up" vector */

	double decofus_angle = 0.0; /* Variation angle of rays through each pixel */
	double focus_distance = 10.0; /* Distance from the camera origin to plane of perfect focus */

	/* Ray tracing params */
	int max_depth = 10; /* Maximum number of bounces per ray */
	
	/* Post-process params */
	bool gamma_correct = false; /* OpenGL gamma corrects for us so this is optional */


	std::vector<unsigned char> Render(const Shape& world)
	{
#define MULTI true
#if MULTI
		/* Main (parallelized) ray tracing loop */
		std::for_each(std::execution::par, vertical_iter.begin(), vertical_iter.end(), [this, &world](unsigned int j) {
			std::for_each(std::execution::par, horizontal_iter.begin(), horizontal_iter.end(), [this, &world, j](unsigned int i) {
				
				TracePixel(i, j, world);

			});
		});
#else
		for (unsigned int j = 0; j < image_height; j++)
		{
			for (unsigned int i = 0; i < image_width; i++)
			{
				TracePixel(i, j, world);
			}
		}
#endif

		/* Iterate the sample count */
		current_samples++;

		return rendered_image;
	}

	void Initialize(unsigned int width, unsigned int height)
	{
		if (image_width != width || image_height != height || !Equal(old_origin, origin) || !Equal(old_look_at, look_at) || !Equal(old_up, up) || old_vfov != vfov)
		{
			/* Reset the image size*/
			image_width = width;
			image_height = height;

			/* Update the stored "prior" camera view params */
			old_origin = origin;
			old_look_at = look_at;
			old_up = up;
			old_vfov = vfov;

			/* Resize and reset the image_accumulator */
			image_accumulator = std::vector<double>(image_width * image_height * 3);
			rendered_image = std::vector<unsigned char>(image_width * image_height * 3);
			current_samples = 0;
		}

		/* Set up iterators for std::foreach */
		horizontal_iter = std::vector<unsigned int>(image_width);
		vertical_iter = std::vector<unsigned int>(image_height);
		for (unsigned int i = 0; i < image_width; i++) horizontal_iter[i] = i;
		for (unsigned int i = 0; i < image_height; i++) vertical_iter[i] = i;

		/* Determine aspect ratio of the image given its dimensions */
		aspect_ratio = double(image_width) / double(image_height);

		/* Set camera origin */
		camera_center = origin;

		/* Determine viewport dimensions */
		//double focal_length = (camera_center - look_at).Length();
		double theta = DegreesToRadians(vfov);
		double h = std::tan(theta / 2.0);
		//double viewport_height = 2.0 * h * focal_length;
		double viewport_height = 2.0 * h * focus_distance;
		double viewport_width = viewport_height * aspect_ratio;

		/* Calculate the orthonormal basis vectors for the camera coordinate frame */
		/* Note: using right hand coordinates! */
		w = Normalize(origin - look_at);
		u = Normalize(Cross(up, w));
		v = Cross(w, u);

		/* Calculate the vectors across the horizontal and down the vertical viewport edges. */
		auto viewport_u = viewport_width * u;
		auto viewport_v = viewport_height * -v;

		/* Calculate the horizontal and vertical delta vectors from pixel to pixel. */
		pixel_delta_u = viewport_u / double(image_width);
		pixel_delta_v = viewport_v / double(image_height);

		/* Calculate the location of the upper left pixel. */
		//auto viewport_upper_left = camera_center - (focal_length * w) - viewport_u / 2.0 - viewport_v / 2.0;
		Vec3 viewport_upper_left = camera_center - (focus_distance * w) - viewport_u / 2.0 - viewport_v / 2.0;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		/* Calculate the camera defocus disk basis vectors */
		double defocus_radius = focus_distance * std::tan(DegreesToRadians(decofus_angle / 2.0));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}

	inline unsigned int GetSampleCount() const { return current_samples; }

private:
	double aspect_ratio = 1.0; /* Ratio of image width over image height */
	Point3 camera_center; /* I.e., origin of the camera */
	Point3 pixel00_loc; /* Location of pixel (0, 0) */
	Vec3 pixel_delta_u; /* Horizontal per-pixel offset */
	Vec3 pixel_delta_v; /* Vertical per-pixel offset */
	Vec3 u, v, w; /* Camera frame orthonormal basis vectors */
	Vec3 defocus_disk_u; /* Defocus disk horizontal radius */
	Vec3 defocus_disk_v; /* Defocus disk vertical radius */

	Vec3 old_origin, old_look_at, old_up; /* Store the previous sample's camera view params */
	double old_vfov; /* Store the previous sample's vfov */

	std::vector<unsigned int> horizontal_iter; /* Horizontal iterator used for std::for_each */
	std::vector<unsigned int> vertical_iter; /* Vertical iterator used for std::for_each */

	unsigned int current_samples; /* Used to determine previous sample contributions to the accumulated image */
	std::vector<double> image_accumulator; /* Stores results from all previous samples */

	std::vector<unsigned char> rendered_image; /* Holds the final result that is returned by Render() */


	Ray GetRay(unsigned int i, unsigned int j)
	{
		///* Generate a camera ray from the origin to a randomly sampled point around the pixel location i, j */
		//Vec3 offset = SampleSquare();
		//auto pixel_sample = pixel00_loc + (((double)i + offset.x()) * pixel_delta_u) + (((double)j + offset.y()) * pixel_delta_v);

		//auto origin = camera_center;
		//auto direction = pixel_sample - origin;

		//return Ray(origin, direction);

		/* Generate a camera ray originating from the defocus disk and directed at a randomly sampled point around the pixel location i, j */
		Vec3 offset = SampleSquare();
		auto pixel_sample = pixel00_loc + (((double)i + offset.x()) * pixel_delta_u) + (((double)j + offset.y()) * pixel_delta_v);

		auto origin = (decofus_angle <= 0) ? camera_center : DefocusDiskSample();
		auto direction = pixel_sample - origin;

		return Ray(origin, direction);
	}

	Vec3 SampleSquare() const
	{
		/* Returns the vector to a random point in the [-0.5, -0.5] to [0.5, 0.5] unit square */
		return Vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0.0);
	}

	Point3 DefocusDiskSample()
	{
		/* Returns a random point in the camera defocus disk */
		Vec3 p = RandomInUnitDisk();
		return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	Color RayColor(const Ray& ray_in, int depth, const Shape& world) const
	{
		/* If we exceed the ray bounce limit, no more light is gathered */
		if (depth <= 0) return Color(0, 0, 0);

		Interaction interaction;
		if (world.Intersect(ray_in, Interval(1e-4, Inf), interaction))
		{
			Ray ray_out;
			Color attenuation;
			if (interaction.material->Scatter(ray_in, interaction, attenuation, ray_out))
			{
				return attenuation * RayColor(ray_out, depth - 1, world);
			}
			return Color(0.0, 0.0, 0.0);
		}

		/* Default sky background */
		Vec3 unit_direction = Normalize(ray_in.direction);
		double a = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
	}

	void TracePixel(unsigned int i, unsigned int j, const Shape& world)
	{
		/* Determine the index to start of this pixel */
		unsigned int pixel = 3 * (j * image_width + i);

		/* Create a ray with a random offset within the pixel */
		Ray ray = GetRay(i, j);

		/* Trace ray and determine new color */
		Color pixel_color = RayColor(ray, max_depth, world);
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
		rendered_image[pixel + 0] = (unsigned char)(256 * intensity.Clamp(gamma_correct ? LinearToGamma(r) : r));
		rendered_image[pixel + 1] = (unsigned char)(256 * intensity.Clamp(gamma_correct ? LinearToGamma(g) : g));
		rendered_image[pixel + 2] = (unsigned char)(256 * intensity.Clamp(gamma_correct ? LinearToGamma(b) : b));
	}

	inline double LinearToGamma(double linear_component)
	{
		if (linear_component > 0) return sqrt(linear_component);
		return 0;
	}
};

} /* namespace rt */