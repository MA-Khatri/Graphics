#include "utils.h"

// Boolean to determine if "P" key has already been pressed
// Used to prevent continuous switching between modes when "P" is pressed
bool p_pressed = false;

void glfw_framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight)
{
	/* We do not set the width and height to the window size anymore? */
	window_width = newWidth;
	window_height = newHeight;

	GLCall(glViewport(0, 0, window_width, window_height));
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	yfov -= (float)yoffset;
	if (yfov < 1.0f) yfov = 1.0f;
	if (yfov > 180.0f) yfov = 180.0f;
}

void glfw_process_input(GLFWwindow* window)
{
	// If the escape key is pressed, set WindowShouldClose to true
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if ((glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) && !p_pressed)
	{
		ui_mode *= -1;
		p_pressed = true;
		//std::cout << "P PRESSED!" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		p_pressed = false;
	}
}

std::string AbsPath(std::string rel_path)
{
	std::filesystem::path cur = __FILE__;
	std::string absPath = (cur.parent_path().parent_path() / rel_path).generic_string();
	return absPath;
}