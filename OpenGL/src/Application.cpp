#include <GL/glew.h> // easier access to modern OpenGL functions
#include <GLFW/glfw3.h> // windowing

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "renderer.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "utils.h"
#include "mesh.h"
#include "object.h"
#include "light.h"
#include "framebuffer.h"

#include "RayTracer/src/ray_tracer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


/* ===================== */
/* ====== Globals ====== */
/* ===================== */

/* Window size */
unsigned int window_width = 1280;
unsigned int window_height = 800;

/* Viewport size */
unsigned int viewport_width = 912;
unsigned int viewport_height = 765;

/* Framebuffer scale allows rendering viewport at higher/lower resolution */
float framebuffer_scale = 2.0f; // By setting to 2, we are effectively forcing 4x MSAA

/* Vertical field of view */
float yfov = 45.0f;

/* Clipping */
float near_clip = 0.01f;
float far_clip = 1000.0f;

/* Camera mode = 1, UI mode = -1 */
int ui_mode = -1;


int main(void)
{
	/* =================================== */
	/* ======= GLFW AND GLEW SETUP ======= */
	/* =================================== */
	if (!glfwInit())
		return -1;

	/* Specify OpenGL version to 4.6 core */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenGL", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	/* Window settings */
	// Enable vsync
	glfwSwapInterval(1);
	// Register the resize function
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	// Register the scroll function
	glfwSetScrollCallback(window, glfw_scroll_callback);

	/* GLEW can only be initialized after creating a valid context */
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	/* ============================= */
	/* ====== OpenGL SETTINGS ====== */
	/* ============================= */
	/* Blending */
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	
	/* Back face culling */
	GLCall(glFrontFace(GL_CCW));
	GLCall(glCullFace(GL_BACK));
	GLCall(glEnable(GL_CULL_FACE));

	/* Depth testing */
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthFunc(GL_LESS));

	/* Cube map bilinear filtering on edges */
	GLCall(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

	/* ========================= */
	/* ====== SCENE SETUP ====== */
	/* ========================= */

	/* ====== Framebuffers ====== */
	Framebuffer rasterized_framebuffer = Framebuffer(viewport_width, viewport_height, framebuffer_scale); // primary rendering frame buffer

	Framebuffer texture_framebuffer = Framebuffer(viewport_width, viewport_height, framebuffer_scale); // render to texture...
	Texture* rendered_texture = new Texture(texture_framebuffer.GetTexture(), viewport_width, viewport_height);

	int shadow_map_resolution = 1000;
	Framebuffer shadow_map_fb_light_1 = Framebuffer(shadow_map_resolution, shadow_map_resolution, 1.0f, true);
	Texture* shadow_map_light_1 = new Texture(shadow_map_fb_light_1.GetTexture(), shadow_map_resolution, shadow_map_resolution, "ShadowMap");

	/* ====== Shaders ====== */
	Shader* shader_environment = new Shader(AbsPath("res/shaders/Environment.shader"));
	Shader* shader_environment_reflections = new Shader(AbsPath("res/shaders/EnvironmentReflections.shader"));
	Shader* shader_world = new Shader(AbsPath("res/shaders/World.shader"));
	Shader* shader_basic = new Shader(AbsPath("res/shaders/Basic.shader"));
	Shader* shader_floor = new Shader(AbsPath("res/shaders/Floor.shader"));
	Shader* shader_shadowed = new Shader(AbsPath("res/shaders/ShadowedSurface.shader"));
	//Shader* shader_texture_fullscreen = new Shader(AbsPath("res/shaders/DrawTextureToFullScreen.shader"));

	/* ====== Textures ====== */
	std::vector<Texture*> floorTextures = {
		new Texture(AbsPath("res/textures/planks_diffuse.png"), "Diffuse", GL_RGBA, GL_UNSIGNED_BYTE),
		new Texture(AbsPath("res/textures/planks_specular.png"), "Specular", GL_RED, GL_UNSIGNED_BYTE)
	};

	std::vector<Texture*> environmentTextures = {
		new Texture(AbsPath("res/textures/yokohama_night_cube_map/"), 2048, ".jpg")
	};

	/* Create a texture to save the ray traced result to, which will then be displayed with ImGui */
	/* Initialize to smallest possible empty texture */
	std::vector<unsigned char> image(4*4*3);
	Texture* ray_traced_texture = new Texture(&image[0], 4, 4, "Diffuse", GL_RGB, GL_UNSIGNED_BYTE);

	std::vector<Texture*> shadowMaps = {shadow_map_light_1};

	/* ====== Objects ====== */
	Object environmentTriangle = Object(CreateEnvironmentMapTriangle(), environmentTextures);
	Object fullscreenTriangle = Object((CreateEnvironmentMapTriangle()));
	Object groundGrid = Object(CreateGroundPlaneGrid(101, 101, 50.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), glm::vec4(0.0f, 1.0f, 0.0f, 0.5f)));

	Object plane1 = Object(CreatePlane());

	Object plane2 = Object(CreatePlane(), shadowMaps);
	plane2.Scale(10.0f);

	Object sphere = Object(LoadOBJ(AbsPath("res/meshes/sphere.obj")), shadowMaps);
	sphere.Translate(1.0f, 1.0f, 4.0f);
	//sphere.Translate(-0.35f, 0.35f, -0.25f);
	//sphere.Scale(0.25f);

	Object bunny1 = Object(LoadOBJ(AbsPath("res/meshes/bunny.obj")));
	bunny1.Translate(0.0f, -0.5f, 0.0f);
	bunny1.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 225.0f);
	bunny1.Scale(0.5f);

	Object bunny2 = Object(LoadOBJ(AbsPath("res/meshes/bunny.obj")), shadowMaps);
	//bunny2.Translate(0.0f, -4.0f, 0.0f);
	bunny2.Translate(0.0f, 0.0f, 0.5f);
	bunny2.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), 180.0f);
	bunny2.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), 90.0f);

	Object lightCube = Object(CreateCube());

	/* ====== Light ====== */
	Light light1 = Light(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	shader_floor->Bind();
	shader_floor->SetUniform3f("u_LightPosition", light1.position.x, light1.position.y, light1.position.z);
	shader_floor->SetUniform3f("u_LightColor", light1.color.x, light1.color.y, light1.color.z);

	shader_environment_reflections->Bind();
	shader_environment_reflections->SetUniform3f("u_LightPosition", light1.position.x, light1.position.y, light1.position.z);
	shader_environment_reflections->SetUniform3f("u_LightColor", light1.color.x, light1.color.y, light1.color.z);


	/* ====== Uniforms ====== */
	shader_floor->Bind();
	shader_floor->SetUniform1i("u_LightMode", 2);


	/* ====== Camera ====== */
	//Camera camera(viewport_width, viewport_height, glm::vec3(10.0f, -3.0f, 2.0f), glm::normalize(glm::vec3(-0.95f, 0.2f, -0.1f)), glm::vec3(0.0f, 0.0f, 1.0f));
	Camera camera(viewport_width, viewport_height, glm::vec3(17.5f, 0.0f, 5.0f), glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
	//Camera camera(viewport_width, viewport_height, glm::vec3(0.0f, 0.0f, 20.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), glm::vec3(0.0f, 1.0f, 0.0f));

	/* 
		Initialize camera matrix. 
		Note: this is just so we can see *something* initially. Unfortunately though, 
		this will use the initial width & height set, not the initial viewport size which 
		we cannot get until we're in the main loop. 
	*/
	camera.Update(yfov, near_clip, far_clip, viewport_width, viewport_height);

	/* ====== Ray tracer setup ====== */
	//rt::ThinLensCamera ray_camera;
	rt::PerspectiveCamera ray_camera;

	ray_camera.max_depth = 10;
	ray_camera.simulate_time = true;

	rt::Scene scene = rt::GenerateScene(rt::Scenes::CornellBox);

	/* ========================= */
	/* ====== ImGui SETUP ====== */
	/* ========================= */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;   // Prevent ImGui from changing mouse cursor settings
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	/* Select Dear ImGui style */
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");


	/* ======================= */
	/* ====== MAIN LOOP ====== */
	/* ======================= */
	while (!glfwWindowShouldClose(window))
	{
		/* Handle inputs to the window */
		glfwPollEvents();

		/* Handle non - camera keyboard inputs */ 
		glfw_process_input(window);

		if (ui_mode > 0)
		{
			/* Handle camera inputs */
			camera.Inputs(window);
			/* Update the camera matrix */ 
			camera.Update(yfov, near_clip, far_clip, viewport_width, viewport_height);
		}

		/* ====== IMGUI ====== */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); /* No padding on viewports */

		ImGui::Begin("Rasterized Viewport");
		{
			ImGui::BeginChild("Rasterized");

			ImVec2 wsize = ImGui::GetWindowSize();

			/* Handle resizing of the viewport */
			if (wsize.x != viewport_width || wsize.y != viewport_height)
			{
				viewport_width = (unsigned int)wsize.x;
				viewport_height = (unsigned int)wsize.y;

				rasterized_framebuffer.UpdateFramebufferSizeAndScale(viewport_width, viewport_height, framebuffer_scale);
				texture_framebuffer.UpdateFramebufferSizeAndScale(viewport_width, viewport_height, framebuffer_scale);
			}

			/* Draw scene... */
			//if (ImGui::IsWindowFocused())
			{
				/* Render to the texture framebuffer... */
				texture_framebuffer.Bind();
				{
					GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
					GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
					bunny1.Draw();
				}
				texture_framebuffer.Unbind();

				/* Render the shadowmap(s) */
				//GLCall(glCullFace(GL_FRONT)); /* Cull front face as a trick to improve peter-panning */
				shadow_map_fb_light_1.Bind();
				{
					light1.UpdateMatrix();
					GLCall(glClear(GL_DEPTH_BUFFER_BIT));
					plane2.Draw(light1);
					sphere.Draw(light1);
					bunny2.Draw(light1);
				}
				shadow_map_fb_light_1.Unbind();
				//GLCall(glCullFace(GL_BACK));


				/* Bind our frame buffer so that we render to it instead of the default viewport */
				rasterized_framebuffer.Bind();
				{
					GLCall(glClearColor(0.075f, 0.133f, 0.173f, 1.0f));
					GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

					groundGrid.Draw(camera, *shader_world);

					//shader_floor->Bind();
					//shader_floor->SetUniform3f("u_LightPosition", light1.position.x, light1.position.y, light1.position.z);
					//shader_floor->SetUniform3f("u_CameraPosition", camera.position.x, camera.position.y, camera.position.z);
					////plane1.UpdateTextures(std::vector<Texture*>{rendered_texture});
					//plane1.Draw(camera, *shader_floor);

					//shader_environment_reflections->Bind();
					//shader_environment_reflections->SetUniform3f("u_LightPosition", light1.position.x, light1.position.y, light1.position.z);
					//shader_environment_reflections->SetUniform3f("u_CameraPosition", camera.position.x, camera.position.y, camera.position.z);
					//sphere.Draw(camera, *shader_environment_reflections);

					lightCube.SetTransform(glm::inverse(light1.view_matrix));
					lightCube.Draw(camera);

					shader_shadowed->Bind();
					shader_shadowed->SetUniformMat4f("u_MatrixShadow", glm::translate(glm::vec3(0.5f, 0.5f, 0.5f - 0.001f)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * light1.matrix);
					shader_shadowed->SetUniform3f("u_LightPosition", light1.position.x, light1.position.y, light1.position.z);
					shader_shadowed->SetUniform3f("u_CameraPosition", camera.position.x, camera.position.y, camera.position.z);
					plane2.Draw(camera, *shader_shadowed);
					sphere.Draw(camera, *shader_shadowed);
					bunny2.Draw(camera, *shader_shadowed);

					/* Draw environment map */
					GLCall(glDepthMask(GL_FALSE));
					environmentTriangle.Draw(camera, *shader_environment, 1);
					GLCall(glDepthMask(GL_TRUE));
				}
				/* Unbind the frame buffer so that ImGui can do its thing */
				rasterized_framebuffer.Unbind();
			}

			/* Note: We invert the V axis for the texture returned by OpenGL */
			ImGui::Image((ImTextureID)rasterized_framebuffer.GetTexture(), wsize, ImVec2(0, 1), ImVec2(1, 0));

			ImGui::EndChild();
		}
		ImGui::End();

		ImGui::Begin("Depth Texture");
		{
			ImGui::BeginChild("Depth");
			ImVec2 wsize = ImGui::GetWindowSize();
			ImGui::Image((ImTextureID)shadow_map_fb_light_1.GetTexture(), wsize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
		}
		ImGui::End();

		ImGui::Begin("Ray Traced Viewport");
		{
			ImGui::BeginChild("Ray Traced");

			if (ImGui::IsWindowFocused())
			{
				ImVec2 wsize = ImGui::GetWindowSize();
				viewport_width = (unsigned int)wsize.x;
				viewport_height = (unsigned int)wsize.y;

				/* RayTracer camera setup */
				ray_camera.simulate_time = true;
				ray_camera.image_width = viewport_width;
				ray_camera.image_height = viewport_height;
				ray_camera.vfov = camera.vfov;
				ray_camera.origin = rt::Point3(camera.position.x, camera.position.y, camera.position.z);
				rt::Vec3 look_at = camera.position + camera.orientation;
				ray_camera.look_at = rt::Point3(look_at.x, look_at.y, look_at.z);
				ray_camera.up = rt::Vec3(camera.up.x, camera.up.y, camera.up.z);

				/* Specific to thin lens setup */
				//ray_camera.defocus_angle = 1.0f; /* Aperture size -- affects how much out of focus things are blurred */
				//ray_camera.focus_distance = 11.0f; /* Distance to plane of perfect focus */

				ray_camera.Initialize();

				image = rt::RayTrace(&scene, &ray_camera);

				ray_traced_texture->Update(&image[0], viewport_width, viewport_height);
				ImGui::Image((ImTextureID)ray_traced_texture->GetTexture(), wsize);
			}

			ImGui::EndChild();
		}
		ImGui::End();

		/* Add back in padding for non-viewport ImGui */
		ImGui::PopStyleVar();

		ImGui::Begin("Info");
		{                
			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); /* use built-in frame rate (average of 120 frames) */
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", ImGui::GetIO().DeltaTime, 1.0f / ImGui::GetIO().DeltaTime);
			ImGui::Text("Window size\n  Width: %.1i  Height: %.1i", window_width, window_height);
			ImGui::Text("Viewport size\n  Width: %.1i  Height: %.1i", viewport_width, viewport_height);
			ImGui::Text("Field of View (Y):  %.1f deg", yfov);
			ImGui::Text("Camera Position:    X=%.3f, Y=%.3f, Z=%.3f", camera.position.x, camera.position.y, camera.position.z);
			ImGui::Text("Camera Orientation: X=%.3f, Y=%.3f, Z=%.3f", camera.orientation.x, camera.orientation.y, camera.orientation.z);
			ImGui::Text("Current Sample Count: %.1i", ray_camera.GetSampleCount());
			   
			std::string m_mode; // Mouse mode
			if (ui_mode > 0) m_mode = "Camera";
			else m_mode = "UI";
				
			ImGui::Text("Mouse Mode: %s", m_mode.c_str());

			ImGui::SliderFloat3("Light Position", &light1.position[0], -20.0f, 20.0f);
		}
		ImGui::End();
		
		ImGui::ShowDemoWindow();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		/* Swap front and back buffers */
		GLCall(glfwSwapBuffers(window));
	}

	/* ====================== */
	/* ====== CLEAN UP ====== */
	/* ======================= */
	//delete(shader_basic);
	delete(shader_world);
	delete(shader_floor);
	delete(shader_environment);
	delete(shader_environment_reflections);
	for (auto& tex : floorTextures) delete(tex);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}