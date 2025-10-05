#pragma once
#include <chrono>
#include "imgui.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <GL/gl.h>       // needed for GL_CLAMP_TO_EDGE
#include "stb_image.h"

namespace render {

    class CTimer
    {
    public:
        void start() {
            t1 = std::chrono::high_resolution_clock::now();
        }

        void end() {
            t2 = std::chrono::high_resolution_clock::now();
        }

        void reset() {
            t1 = std::chrono::high_resolution_clock::now();
        }

        void previous() {
            previous_call_time = std::chrono::high_resolution_clock::now();
        }

        int  current() {
            auto milliseconds_delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - previous_call_time);
            return (int)milliseconds_delta.count();
        }

        bool is_higher(std::chrono::milliseconds time) {
            get_result();
            auto milliseconds_delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - previous_call_time);
            return (milliseconds_delta > time);
        }

    private:

        void get_result() {
            m_result = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        }

        int m_result = 0;
        long long frequency = 0;        // ticks per second
        std::chrono::steady_clock::time_point t1, t2, previous_call_time;
    };

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

    class Texture
    {
        typedef struct frame_data_t {
            int                  delay;
            GLuint               data;
            struct frame_data_t* next;
        } frame_data;

    public:

        Texture(const char* filename);

        ~Texture();

        GLuint get_current();

        ImVec2 get_size();

        std::string get_path();

        void image();

    private:
        std::string filepath = "";

        int x, y, frames = 0;
        frame_data head{};
        frame_data* current{};
        CTimer timer{};
    };

    /*
    
                    texture = std::make_unique<render::Texture>("C:fullpath");
                       if (texture.get()) {
                                ImGui::Image(
                                    (ImTextureID)(intptr_t)texture->get_current(),
                                    texture->get_size()
                       );
        }
    */
}