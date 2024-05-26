#include "cameras.h"

namespace rt
{
/* ========================== */
/* === Perspective Camera === */
/* ========================== */
Ray PerspectiveCamera::GenerateRay(unsigned int i, unsigned int j)
{
	/* Generate a camera ray from the origin to a randomly sampled point around the pixel location i, j */
	Point2 offset = SampleSquare();
	Vec3 pixel_sample = pixel00_loc + (((float)i + offset.x) * pixel_delta_u) + (((float)j + offset.y) * pixel_delta_v);
	Vec3 direction = pixel_sample - origin;

	return Ray(origin, direction);
}

void PerspectiveCamera::Initialize()
{
	/* Check if any of the view params have changed */
	if (old_image_width != image_width 
		|| old_image_height != image_height 
		|| !Equal(old_origin, origin) 
		|| !Equal(old_look_at, look_at) 
		|| !Equal(old_up, up) 
		|| old_vfov != vfov)
	{
		/* Update the stored "prior" camera view params */
		old_image_width = image_width;
		old_image_height = image_height;
		old_origin = origin;
		old_look_at = look_at;
		old_up = up;
		old_vfov = vfov;

		/* Resize and reset the image_accumulator */
		image_accumulator = std::vector<float>(image_width * image_height * 3);
		current_samples = 0;
	}

	/* Determine aspect ratio of the image given its dimensions */
	aspect_ratio = float(image_width) / float(image_height);

	/* Determine viewport dimensions */
	float focal_length = glm::length(origin - look_at);
	float theta = DegreesToRadians(vfov);
	float h = std::tan(theta / 2.0f);
	float viewport_height = 2.0f * h * focal_length;
	float viewport_width = viewport_height * aspect_ratio;

	/* Calculate the orthonormal basis vectors for the camera coordinate frame */
	/* Note: using right hand coordinates! */
	w = glm::normalize(origin - look_at);
	u = glm::normalize(glm::cross(up, w));
	v = glm::cross(w, u);

	/* Calculate the vectors across the horizontal and down the vertical viewport edges. */
	Vec3 viewport_u = viewport_width * u;
	Vec3 viewport_v = viewport_height * -v;

	/* Calculate the horizontal and vertical delta vectors from pixel to pixel. */
	pixel_delta_u = viewport_u / float(image_width);
	pixel_delta_v = viewport_v / float(image_height);

	/* Calculate the location of the upper left pixel. */
	Vec3 viewport_upper_left = origin - (focal_length * w) - viewport_u / 2.0f - viewport_v / 2.0f;
	pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
}


/* ======================== */
/* === Thin Lens Camera === */
/* ======================== */
Ray ThinLensCamera::GenerateRay(unsigned int i, unsigned int j)
{
	/* Generate a ray from the defocus disk directed at a randomly sampled point around pixel location i, j */
	Point2 offset = SampleSquare();
	Vec3 pixel_sample = pixel00_loc + (((float)i + offset.x) * pixel_delta_u) + (((float)j + offset.y) * pixel_delta_v);

	Vec3 ray_origin = (defocus_angle <= 0.0f) ? origin : DefocusDiskSample();
	Vec3 direction = pixel_sample - ray_origin;

	return Ray(ray_origin, direction);
}

Point3 ThinLensCamera::DefocusDiskSample()
{
	Vec2 p = RandomInUnitDisk();
	return origin + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
}

void ThinLensCamera::Initialize()
{
	/* Check if any of the view params have changed */
	if (old_image_width != image_width
		|| old_image_height != image_height
		|| !Equal(old_origin, origin)
		|| !Equal(old_look_at, look_at)
		|| !Equal(old_up, up)
		|| old_vfov != vfov)
	{
		/* Update the stored "prior" camera view params */
		old_image_width = image_width;
		old_image_height = image_height;
		old_origin = origin;
		old_look_at = look_at;
		old_up = up;
		old_vfov = vfov;

		/* Resize and reset the image_accumulator */
		image_accumulator = std::vector<float>(image_width * image_height * 3);
		current_samples = 0;
	}

	/* Determine aspect ratio of the image given its dimensions */
	aspect_ratio = float(image_width) / float(image_height);

	/* Determine viewport dimensions */
	float theta = DegreesToRadians(vfov);
	float h = std::tan(theta / 2.0f);
	float viewport_height = 2.0f * h * focus_distance;
	float viewport_width = viewport_height * aspect_ratio;

	/* Calculate the orthonormal basis vectors for the camera coordinate frame */
	/* Note: using right hand coordinates! */
	w = glm::normalize(origin - look_at);
	u = glm::normalize(glm::cross(up, w));
	v = glm::cross(w, u);

	/* Calculate the vectors across the horizontal and down the vertical viewport edges. */
	Vec3 viewport_u = viewport_width * u;
	Vec3 viewport_v = viewport_height * -v;

	/* Calculate the horizontal and vertical delta vectors from pixel to pixel. */
	pixel_delta_u = viewport_u / float(image_width);
	pixel_delta_v = viewport_v / float(image_height);

	/* Calculate the location of the upper left pixel. */
	Vec3 viewport_upper_left = origin - (focus_distance * w) - viewport_u / 2.0f - viewport_v / 2.0f;
	pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	/* Calculate the camera defocus disk basis vectors */
	float defocus_radius = focus_distance * std::tan(DegreesToRadians(defocus_angle / 2.0f));
	defocus_disk_u = u * defocus_radius;
	defocus_disk_v = v * defocus_radius;
}

}