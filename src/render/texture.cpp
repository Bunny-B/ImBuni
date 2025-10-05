#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <format>
namespace render {

    static bool LoadTextureFromMemory(unsigned char* image_data, GLuint* out_texture, int image_width, int image_height)
    {
        if (image_data == nullptr || out_texture == nullptr)
            return false;

        GLuint texture_id = 0;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        // Upload the texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

        // Set texture filtering and wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Unbind
        glBindTexture(GL_TEXTURE_2D, 0);

        *out_texture = texture_id;
        return true;
    }

    Texture::Texture(const char* filename) {
        filepath = std::string(filename);
        FILE* f;
        stbi__context s;
        unsigned char* result = 0;

        x, y, frames = 0;

        if (!(f = stbi__fopen(filename, "rb")))
            return;

        stbi__start_file(&s, f);

        if (stbi__gif_test(&s))
        {
            int c;
            stbi__gif g;
            frame_data* prev = 0, * gr = &head;

            memset(&g, 0, sizeof(g));
            memset(&head, 0, sizeof(head));

            frames = 0;

            while (result = stbi__gif_load_next(&s, &g, &c, 4, nullptr))
            {
                if (result == (unsigned char*)&s)
                {
                    gr->data = 0;
                    break;
                }

                LoadTextureFromMemory(result, &gr->data, g.w, g.h);
                if (prev) prev->next = gr;
                gr->delay = g.delay;
                gr->next = &head;
                prev = gr;
                gr = (frame_data*)stbi__malloc(sizeof(frame_data));
                memset(gr, 0, sizeof(frame_data));
                ++frames;
            }

            STBI_FREE(g.out);

            if (gr != &head)
                STBI_FREE(gr);

            if (frames > 0)
            {
                x = g.w;
                y = g.h;
            }
        }
        else
        {
            stbi__result_info ri;
            result = (unsigned char*)stbi__load_main(&s, &x, &y, &frames, 4, &ri, 8);
            frames = !!result;
            LoadTextureFromMemory(result, &head.data, x, y);
            head.next = &head;
            STBI_FREE(result);

        }

        fclose(f);

        current = &head;
        timer.previous();
    }

    Texture::~Texture()
    {
        auto current = head.next;
        for (int i = 1; i < frames; ++i)
        {
            if (current)
            {
                auto prev = current;
                current = current->next;
                glDeleteTextures(1, &prev->data);
            }
        }
    }

    GLuint Texture::get_current()// -> GLuint
    {
        if (timer.is_higher((std::chrono::milliseconds)current->delay)) {
            current = current->next;
            timer.previous();
        }
        return current->data;
    }

    ImVec2 Texture::get_size() { return ImVec2(x, y); }

    std::string Texture::get_path() { return filepath; }

    void Texture::image()
    {
        ImGui::Image((ImTextureID)(intptr_t)get_current(), get_size());
    }

}