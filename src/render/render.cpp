#include "render.h"
#include "imgui.h"
#include "stdio.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <thread>
// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>   // for glfwGetWin32Window
#include <Windows.h>
#endif

#define STB_IMAGE_IMPLEMENTATION


namespace render {
    static GLFWwindow* g_window = nullptr;
    static ImGuiWindowFlags currentFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;

    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }


    static void loadDefaultFont(ImGuiIO& io, float size = 18.0f) {
        const char* font_path = nullptr;

#ifdef _WIN32
        font_path = "C:/Windows/Fonts/segoeui.ttf";
#elif __linux__
        font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
#endif

        if (font_path) {
            ImFont* font = io.Fonts->AddFontFromFileTTF(font_path, size);
            if (!font) {
                // Fallback 
                io.Fonts->AddFontDefault();
            }
        }
        else {
            io.Fonts->AddFontDefault();
        }
    }

    static void mainWindow(char isOverlay, void(*renderFunction)()) {
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f)); // Position at the top-left corner
        ImGui::SetNextWindowSize(displaySize); // Size equal to the display size

        // Use the flags passed or the currentFlags
        ImGui::Begin("main window", nullptr, currentFlags);
        renderFunction();
        ImGui::End();

    }

    char createWindow(const char* window_name, void(*renderFunction)(), bool is_overlay, void* hIcon, void* hIconSm)
    {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return 1;

        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        if (is_overlay) {
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);               // No title bar / border
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);  // Transparent background
        }

        // Create window with graphics context
        float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
        GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), window_name, nullptr, nullptr);
        if (window == nullptr)
            return 1;
        
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        // Always on top (overlay)
        if (is_overlay) {
            glfwSetWindowAttrib(window, GLFW_FLOATING, GLFW_TRUE);
        }

#ifdef _WIN32
        if (is_overlay) {
            HWND hwnd = glfwGetWin32Window(window);
            LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
            SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
        }
#endif
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        loadDefaultFont(io, 18.0f);

        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        if (is_overlay) {
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background do once

            currentFlags =
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav;
        }


        while (running && !glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
            {
                ImGui_ImplGlfw_Sleep(10);
                continue;
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            mainWindow(is_overlay, renderFunction);

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            if (is_overlay) {
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // fully transparent
            }
            else {
                glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            }
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
    }

    void startRenderThread(const char* window_name, void(*renderFunction)(), bool is_overlay, void* hIcon, void* hIconSm) {
        if (running.load()) return; // Already running
        running.store(true);
        renderThread = std::thread(createWindow, window_name, renderFunction, is_overlay, hIcon, hIconSm);
        renderThread.detach();
    }

    // Stops the render loop and joins the thread
    void stopRenderThread() {
        if (!running.load()) return;

        running.store(false);
        if (g_window)

            glfwSetWindowShouldClose(g_window, GLFW_TRUE);

        if (renderThread.joinable())
            renderThread.join();
    }
}