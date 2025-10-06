/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  TextureManager.cpp
Project:    CS230 Engine
Author:     Taekuyng Ho
Created:    April 29, 2025
*/

#include "TextureManager.h"
#include "Engine.h"
namespace CS230 {
	Texture* TextureManager::Load(const std::filesystem::path& file_name)
	{
		if (textures.find(file_name) == textures.end()) {
			textures[file_name] = new Texture(file_name);
			Engine::GetLogger().LogEvent("Loading Texture: " + file_name.string());
		}
		return textures[file_name];
	}

	void TextureManager::Unload()
	{
		for (std::pair<std::filesystem::path, Texture*> texture : textures) {
			delete texture.second;
			Engine::GetLogger().LogEvent("Unload Texture: " + texture.first.string());
		}
		textures.clear();
	}
}

namespace {
	RenderTexture2D s_render_texture = {};
}

void CS230::TextureManager::StartRenderTextureMode(int width, int height) {
	s_render_texture = LoadRenderTexture(width, height);
	BeginTextureMode(s_render_texture);
	rlClearColor(0, 0, 0, 0);
	rlClearScreenBuffers();
}

CS230::Texture* CS230::TextureManager::EndRenderTextureMode() {
	EndTextureMode();
	const auto window_size = Engine::GetWindow().GetSize();
	rlSetFramebufferWidth(window_size.x);
	rlSetFramebufferHeight(window_size.y);
	rlUnloadFramebuffer(s_render_texture.id);
	const auto result = new Texture{ s_render_texture.texture };
	rendered_textures.push_back(result);
	return result;
}


