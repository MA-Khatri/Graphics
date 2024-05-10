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

/* ===================== */
/* ====== Globals ====== */
/* ===================== */

// (Initial) Window size
unsigned int width = 1280;
unsigned int height = 800;

// Vertical field of view
float yfov = 45.0f;

// Camera mode = 1, UI mode  = -1
int ui_mode = -1;

/* ==================== */
/* ====== Shapes ====== */
/* ==================== */

float size = 1.0f;
float positions[] = {
     size,  size, 1.0f, 1.0f, // 0
     size, -size, 1.0f, 0.0f, // 1
    -size, -size, 0.0f, 0.0f, // 2
    -size,  size, 0.0f, 1.0f, // 3
};

unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
};


int main(void)
{
    /* =================================== */
    /* ======= GLFW AND GLEW SETUP ======= */
    /* =================================== */
    if (!glfwInit())
        return -1;

    /* Specify OpenGL version to 3.3 core */
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

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /* ============================= */
    /* ====== OpenGL SETTINGS ====== */
    /* ============================= */
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GLCall(glEnable(GL_DEPTH_TEST));
    
    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_FRONT));
    GLCall(glFrontFace(GL_CCW));

    /* ========================= */
    /* ====== SCENE SETUP ====== */
    /* ========================= */
    Renderer renderer;

    VertexArray* va = new VertexArray();
    va->Bind();

    VertexBuffer* vb = new VertexBuffer(positions, 4 * 4 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    va->AddBuffer(*vb, layout);
    
    IndexBuffer* ib = new IndexBuffer(indices, 6);

    Shader* shader = new Shader("res/shaders/BasicTexture.shader");
    shader->Bind();
    shader->SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

    Texture* texture = new Texture("res/textures/rend.png");
    texture->Bind();
    shader->SetUniform1i("u_Texture", 0);

    Camera camera(width, height, glm::vec3(7.0f, 0.0f, 2.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    va->Unbind();
    vb->Unbind();
    ib->Unbind();
    shader->Unbind();

    /* 
        Initialize camera matrix. 
        Note: this is just so we can see *something* initially. Unfortunately though, 
        this will use the initial width & height set, not the initial viewport size which 
        we cannot get until we're in the main loop. 
    */
    camera.Update(yfov, 0.1f, 100.0f, width, height);

    float r = 0.0f;
    float increment = 0.05f;

    /* ========================= */
    /* ====== Framebuffer ====== */
    /* ========================= */

    /* Following: https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/ */

    GLuint FramebufferName = 0;
    GLCall(glGenFramebuffers(1, &FramebufferName));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName));

    /* The texture we're going to render to */
    GLuint renderedTexture;
    GLCall(glGenTextures(1, &renderedTexture));

    /* "Bind" the newly created texture: all future texture functions will modify this texture until we unbind it */
    GLCall(glBindTexture(GL_TEXTURE_2D, renderedTexture));

    /* Give an empty image to OpenGL (the last "0") */
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0));

    /* Needed due to poor filtering */
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

    /* Set "renderedTexture" as our colour attachement #0 */
    GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0));

    /* Set the list of draw buffers. */
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    GLCall(glDrawBuffers(1, DrawBuffers)); // "1" is the size of DrawBuffers

    /* Always check that our framebuffer is ok */
    GLCall(if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return -1);

    /* Unbind the frame buffer and texture */
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

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
    ImGui_ImplOpenGL3_Init("#version 130");

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
            camera.Update(yfov, 0.1f, 100.0f, width, height);
        }

        /* Modify r channel of color uniform */
        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;
        r += increment;

        /* ============================= */
        /* ==== Non-ImGui Rendering ==== */
        /* ============================= */

        /* Bind our frame buffer so that we render to it instead of the default viewport */
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName));

        GLCall(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
        renderer.Clear();

        shader->Bind();
        shader->SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
        shader->SetUniformMat4f("u_MVP", camera.matrix);
        renderer.Draw(*va, *ib, *shader);

        shader->SetUniformMat4f("u_MVP", camera.matrix * glm::translate(glm::mat4(1), glm::vec3(0.0f, 2.2f, 0.0f)));
        renderer.Draw(*va, *ib, *shader);

        /* Unbind the frame buffer so that ImGui can do its thing */
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        /* ============================= */
        /* ====== ImGui Rendering ====== */
        /* ============================= */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        ImGui::Begin("Info");
        {                
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Window size\n  Width: %.1i  Height: %.1i", width, height);
            ImGui::Text("Field of View (Y):  %.1f deg", yfov);
            ImGui::Text("Camera Position:    X=%.3f, Y=%.3f, Z=%.3f", camera.position.x, camera.position.y, camera.position.z);
            ImGui::Text("Camera Orientation: X=%.3f, Y=%.3f, Z=%.3f", camera.orientation.x, camera.orientation.y, camera.orientation.z);
               
            std::string m_mode; // Mouse mode
            if (ui_mode > 0) m_mode = "Camera";
            else m_mode = "UI";
                
            ImGui::Text("Mouse Mode: %s", m_mode.c_str());
        }
        ImGui::End();

        ImGui::Begin("Viewport");
        {
            ImGui::BeginChild("GameRender");

            ImVec2 wsize = ImGui::GetWindowSize();
            width = wsize.x;
            height = wsize.y;

            /* Note: We invert the V axis for the texture returned by OpenGL */
            ImGui::Image((ImTextureID)renderedTexture, wsize, ImVec2(0, 1), ImVec2(1, 0));

            ImGui::EndChild();
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
    delete(va);
    delete(vb);
    delete(ib);
    delete(shader);
    delete(texture);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}