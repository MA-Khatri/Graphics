#include <iostream>

#include "ray_tracer.h"

double HitSphere(const Point3& center, double radius, const Ray& r) {
	Vec3 oc = center - r.origin();
	auto a = r.direction().lengthSquared();
	auto h = dot(r.direction(), oc);
	auto c = oc.lengthSquared() - radius * radius;
	auto discriminant = h * h - a * c;

	if (discriminant < 0) {
		return -1.0;
	}
	else {
		return (h - sqrt(discriminant)) / a;
	}
}

Vec3 RayColor(const Ray& r)
{
	auto t = HitSphere(Point3(0, 0, -1), 0.5, r);
	if (t > 0.0) {
		Vec3 N = normalize(r.at(t) - Vec3(0, 0, -1));
		return 0.5 * Color(N.x() + 1, N.y() + 1, N.z() + 1);
	}

	Vec3 unit_direction = normalize(r.direction());
	auto a = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

std::vector<unsigned char> RayTrace(unsigned int width, unsigned int height, float yfov)
{
	// Camera

	auto focal_length = 1.0;
	auto viewport_height = 2.0;
	auto viewport_width = viewport_height * (double(width) / height);
	auto camera_center = Point3(0, 0, 0);

	// Calculate the vectors across the horizontal and down the vertical viewport edges.
	auto viewport_u = Vec3(viewport_width, 0, 0);
	auto viewport_v = Vec3(0, -viewport_height, 0);

	// Calculate the horizontal and vertical delta vectors from pixel to pixel.
	auto pixel_delta_u = viewport_u / width;
	auto pixel_delta_v = viewport_v / height;

	// Calculate the location of the upper left pixel.
	auto viewport_upper_left = camera_center - Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
	auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	std::vector<unsigned char> image;
	for (unsigned int j = 0; j < height; j++)
	{
		for (unsigned int i = 0; i < width; i++)
		{
			auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
			auto ray_direction = pixel_center - camera_center;
			Ray r(camera_center, ray_direction);

			Color pixel_color = RayColor(r);

			image.push_back((unsigned char)(pixel_color.x() * 255.999f));
			image.push_back((unsigned char)(pixel_color.y() * 255.999f));
			image.push_back((unsigned char)(pixel_color.z() * 255.999f));
		}
	}
	std::vector<unsigned char> flipped = FlipImage(image, width, height);

	return flipped;
}