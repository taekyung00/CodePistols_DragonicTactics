

#pragma once
#include "OpenGL/Framebuffer.hpp"
#include <filesystem>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CS230
{
    class Texture;


    class TextureManager
    {
    public:
        
        std::shared_ptr<Texture> Load(const std::filesystem::path& file_name);

        
        void Unload();

        
        static void StartRenderTextureMode(int width, int height);

        
        static std::shared_ptr<Texture> EndRenderTextureMode();

    private:
        
        std::map<std::filesystem::path, std::shared_ptr<Texture>> textures;

        
        struct RenderInfo
        {
            //RenderInfo() = default;
            OpenGL::FramebufferWithColor Target{};
            Math::ivec2                  Size{};
            std::array<GLfloat, 4>       ClearColor{};
            std::array<GLint, 4>         Viewport{};
        };

        // inline static RenderInfo render_info{};

        static RenderInfo& get_render_info()
        {
           static RenderInfo instance;
           return instance;
        }
    };
}
