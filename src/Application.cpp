#include <GL/glew.h> // easier access to modern OpenGL functions
#include <GLFW/glfw3.h> // windowing

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Utils.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


/* == Globals == */

// Window size
unsigned int width = 800;
unsigned int height = 800;

// Vertical field of view
float yfov = 45.0f;

/* Globals end */


int main(void)
{
    if (!glfwInit())
        return -1;

    /* Specify OpenGL version to 3.3 core */
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
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

    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        float positions[] = {
            -0.5f, -0.5f, 0.0f, 0.0f,// 0
             0.5f, -0.5f, 1.0f, 0.0f,// 1
             0.5f,  0.5f, 1.0f, 1.0f,// 2
            -0.5f,  0.5f, 0.0f, 1.0f,// 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Enable blending
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        // Enable depth testing
        GLCall(glEnable(GL_DEPTH_TEST));
        // Enables face culling
        GLCall(glEnable(GL_CULL_FACE));
        // Keeps front faces
        GLCall(glCullFace(GL_FRONT));
        // Uses counter clock-wise standard
        GLCall(glFrontFace(GL_CCW));

        VertexArray va;
        va.Bind();
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        // Initialize the camera with fov=45 at (1, 0, 1), oriented at (-1, 0, 0) with up vector (0, 0, 1)
        Camera camera(width, height, glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        Shader shader("res/shaders/BasicTexture.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

        Texture texture("res/textures/rend.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        /* ImGui setup */
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        ////io.ConfigViewportsNoAutoMerge = true;
        ////io.ConfigViewportsNoTaskBarIcon = true;

        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        /* ImGui setup end */

        float r = 0.0f;
        float increment = 0.05f;
        glm::vec3 translation(0.0f, 0.0f, 0.0f);

        Renderer renderer;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            // Handle inputs to the window
            glfwPollEvents();
            // Handle non-camera keyboard inputs
            glfw_process_input(window);

            // Handle camera inputs
            camera.Inputs(window);
            // Update the camera matrix
            camera.updateMatrix(yfov, 0.1f, 100.0f, width, height);
            // Pass camera matrix to shader
            camera.Matrix(shader, "u_MVP");

            /* Render here */
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            /* ==== Non-ImGui rendering... ==== */

            renderer.Clear();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            renderer.Draw(va, ib, shader);

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;
            r += increment;

            /* ==== <> ==== */

            {
                ImGui::Begin("Hello, world!");

                ImGui::SliderFloat2("Translate", &translation.x, -1.0f, 1.0f);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();
            }

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
    }

    /* Clean up */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}