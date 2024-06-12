#pragma once

#include "common.h"
#include "hittable.h"
#include "material.h"

#include <algorithm>

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
	std::vector<double> image_accumulator; /* Stores results from all previous samples used for accumulation */
	bool simulate_time = false; /* Determines if camera has a "shutter speed" to simulate effects like motion blur.
								   Note: Timescale for the cameras is always defined within 0-1; it is up to the user
								   to decide how much/where objects move within that time frame. */
	int stratified_side_length = 32; /* Used for stratified sampling. Each pixel is divided into statified_side_length**2 
									    sub pixels. On each sample, a random sub pixel is chosen to generate a ray using a 
									    point within that sub pixel. Usually, this should be set to the square root of 
										the number of samples you wish to do a final render with. So, if its 32, we are 
										assuming a final render with 1024 samples. */

	/* Post-process params */
	bool gamma_correct = false; /* OpenGL gamma corrects for us so this is optional */

protected:
	double aspect_ratio = 1.0; /* Ratio of image width over image height */

	/* Store previous sample's view params */
	unsigned int old_image_width = 100;
	unsigned int old_image_height = 100;

protected:
	/* Choose a sub pixel to generate stratified samples from. */
	Point2 GetSubPixelOffset(); 
};


class ProjectiveCamera : public Camera
{
public:
	/* View Params */
	Point3 origin = Point3(0.0, 0.0, 0.0); /* Point the camera is looking from */
	Point3 look_at = Point3(0.0, 0.0, -1.0); /* Point the camera is looking at */
	Vec3 up = Vec3(0.0, 1.0, 0.0); /* Camera-relative "up" vector */

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
	double vfov = 90.0; /* Vertical field of view */

private:
	double old_vfov; /* Store the previous sample's vfov */

};


class ThinLensCamera : public ProjectiveCamera
{
public:
	Ray GenerateRay(unsigned int i, unsigned int j) override;
	void Initialize();

public:
	double defocus_angle = 0.0; /* Variation angle of rays through each pixel */
	double focus_distance = 10.0; /* Distance from the camera origin to plane of perfect focus */
	double vfov = 90.0; /* Vertical field of view */

private:
	/* Returns a random point in the camera defocus disk */
	Point3 DefocusDiskSample();

private:
	/* Calculated values in initialize */
	Vec3 defocus_disk_u; /* Defocus disk horizontal radius */
	Vec3 defocus_disk_v; /* Defocus disk vertical radius */

	double old_vfov; /* Store the previous sample's vfov */
};


class RealisticCamera : public Camera
{

};


class EnvironmentCamera : public Camera
{

};

}