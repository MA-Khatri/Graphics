#include "Utils.h"

void glfw_framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;
	GLCall(glViewport(0, 0, width, height));
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

	//if ((glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) && !p_pressed)
	//{
	//	ui_mode *= -1;
	//	p_pressed = true;
	//	std::cout << "P PRESSED!" << std::endl;
	//}
	//if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
	//{
	//	p_pressed = false;
	//}
}