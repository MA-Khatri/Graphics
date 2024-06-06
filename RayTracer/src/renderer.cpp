#include "renderer.h"


namespace rt
{
std::vector<unsigned char> Render(const Scene& scene, Camera& camera)
{
	auto rendered_image = std::vector<unsigned char>(camera.image_width * camera.image_height * 3);

#define MULTI_THREADED true
#if MULTI_THREADED
	/* Set up iterators for std::foreach */
	auto horizontal_iter = std::vector<unsigned int>(camera.image_width);
	auto vertical_iter = std::vector<unsigned int>(camera.image_height);
	for (unsigned int i = 0; i < camera.image_width; i++) horizontal_iter[i] = i;
	for (unsigned int i = 0; i < camera.image_height; i++) vertical_iter[i] = i;

	/* Main (parallelized) ray tracing loop */
	std::for_each(std::execution::par, vertical_iter.begin(), vertical_iter.end(), [&](unsigned int j) {
		std::for_each(std::execution::par, horizontal_iter.begin(), horizontal_iter.end(), [&](unsigned int i) {

			PixelColor(rendered_image, i, j, scene, camera);

			});
		});
#else
	for (unsigned int j = 0; j < camera.image_height; j++)
	{
		for (unsigned int i = 0; i < camera.image_width; i++)
		{
			PixelColor(rendered_image, i, j, world, camera);
		}
	}
#endif

	/* Iterate the sample count for this camera */
	camera.current_samples++;

	return rendered_image;
}


Color TraceRay(const Ray& ray_in, int depth, const Scene& scene)
{
	/* If we exceed the ray bounce limit, no more light is gathered */
	if (depth <= 0) return Color(0.0, 0.0, 0.0);

	Interaction interaction;
	
	/* If the ray hits nothing, sample the sky texture */
	if (!scene.World().Hit(ray_in, Interval(Eps, Inf), interaction))
	{
		return scene.SampleSky(ray_in);
	}

	Ray ray_out;
	Color attenuation;
	Color color_from_emission = interaction.material->Emitted(interaction.u, interaction.v, interaction.posn);

	/* If ray does not scatter, return the emitted color */
	if (!interaction.material->Scatter(ray_in, interaction, attenuation, ray_out)) return color_from_emission;

	/* Otherwise, determine the scattered color by recursively tracing the ray */
	Color color_from_scatter = attenuation * TraceRay(ray_out, depth - 1, scene);

	return color_from_emission + color_from_scatter;
}

void PixelColor(std::vector<unsigned char>& rendered_image, unsigned int i, unsigned int j, const Scene& scene, Camera& camera)
{
	/* Determine the index to start of this pixel */
	unsigned int pixel = 3 * (j * camera.image_width + i);

	/* Create a ray with a random offset within the pixel */
	Ray ray = camera.GenerateRay(i, j);

	/* Trace ray and determine new color */
	Color pixel_color = TraceRay(ray, camera.max_depth, scene);
	double r = pixel_color.r;
	double g = pixel_color.g;
	double b = pixel_color.b;

	/* Extract accumulated color */
	double cr = camera.image_accumulator[pixel + 0];
	double cg = camera.image_accumulator[pixel + 1];
	double cb = camera.image_accumulator[pixel + 2];

	/* Determine the new accumulated color */
	r = (r + camera.current_samples * cr) / (camera.current_samples + 1);
	g = (g + camera.current_samples * cg) / (camera.current_samples + 1);
	b = (b + camera.current_samples * cb) / (camera.current_samples + 1);

	/* Set the new accumulated values in the accumulator */
	camera.image_accumulator[pixel + 0] = r;
	camera.image_accumulator[pixel + 1] = g;
	camera.image_accumulator[pixel + 2] = b;

	/* Clamp the color from 0-255 and set the pixel values to return */
	static const Interval intensity(0.000, 0.999);
	rendered_image[pixel + 0] = (unsigned char)(256 * intensity.Clamp(camera.gamma_correct ? LinearToGamma(r) : r));
	rendered_image[pixel + 1] = (unsigned char)(256 * intensity.Clamp(camera.gamma_correct ? LinearToGamma(g) : g));
	rendered_image[pixel + 2] = (unsigned char)(256 * intensity.Clamp(camera.gamma_correct ? LinearToGamma(b) : b));
}

}
