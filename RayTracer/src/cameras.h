#pragma once

#include "common.h"
#include "shapes.h"
#include "material.h"

namespace rt
{

class Camera
{
public:
	/* Determine the ray that will exit the sensor for pixel i, j */
	virtual Ray GenerateRay(unsigned int i, unsigned int j) { return Ray(); }

	/* Initialize camera parameters */
	virtual void Initialize() {}

	/* Return current sample count of rendered image */
	inline unsigned int GetSampleCount() const { return current_samples; }

public:
	/* Image dimensions */
	unsigned int image_width = 100;
	unsigned int image_height = 100;

	/* Ray Tracing params */
	int max_depth = 10; /* Maximum number of bounces per ray */
	unsigned int current_samples = 0; /* Used to determine previous sample contributions to the accumulated image */
	std::vector<float> image_accumulator; /* Stores results from all previous samples used for accumulation */

	/* Post-process params */
	bool gamma_correct = false; /* OpenGL gamma corrects for us so this is optional */

protected:
	float aspect_ratio = 1.0f; /* Ratio of image width over image height */

	/* Store previous sample's view params */
	unsigned int old_image_width = 100;
	unsigned int old_image_height = 100;
};


class ProjectiveCamera : public Camera
{
public:
	/* View Params */
	Point3 origin = Point3(0.0f, 0.0f, 0.0f); /* Point the camera is looking from */
	Point3 look_at = Point3(0.0f, 0.0f, -1.0f); /* Point the camera is looking at */
	Vec3 up = Vec3(0.0f, 1.0f, 0.0f); /* Camera-relative "up" vector */

protected:
	/* Calculated params (in Initialize) */
	Vec3 pixel_delta_u; /* Horizontal per-pixel offset */
	Vec3 pixel_delta_v; /* Vertical per-pixel offset */
	Vec3 u, v, w; /* Camera frame orthonormal basis vectors */
	Vec3 pixel00_loc; /* The location of the upper-left pixel */

	/* Store the previous sample's camera view params */
	Vec3 old_origin, old_look_at, old_up;
};


class OrthographicCamera : public ProjectiveCamera
{

};


class PerspectiveCamera : public ProjectiveCamera
{
public:
	Ray GenerateRay(unsigned int i, unsigned int j) override;
	void Initialize() override;

public:
	float vfov = 90.0f; /* Vertical field of view */

private:
	float old_vfov; /* Store the previous sample's vfov */

};


class ThinLensCamera : public ProjectiveCamera
{
public:
	Ray GenerateRay(unsigned int i, unsigned int j) override;
	void Initialize();

public:
	float defocus_angle = 0.0f; /* Variation angle of rays through each pixel */
	float focus_distance = 10.0f; /* Distance from the camera origin to plane of perfect focus */
	float vfov = 90.0f; /* Vertical field of view */

private:
	/* Returns a random point in the camera defocus disk */
	Point3 DefocusDiskSample();

private:
	/* Calculated values in initialize */
	Vec3 defocus_disk_u; /* Defocus disk horizontal radius */
	Vec3 defocus_disk_v; /* Defocus disk vertical radius */

	float old_vfov; /* Store the previous sample's vfov */
};


class RealisticCamera : public Camera
{

};


class EnvironmentCamera : public Camera
{

};

}