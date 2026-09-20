#include "pch.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Engine.h"
#include "Logger.h"
#include "OpenGL/GL.h"
#include "Screenshot.h"
#include "Window.h"

#include <ctime>
#include <filesystem>
#include <string>
#include <vector>

namespace
{
	std::string MakeTimestampedPath()
	{
		std::time_t now = std::time(nullptr);
		std::tm		local{};
#if defined(_WIN32)
		localtime_s(&local, &now);
#else
		localtime_r(&now, &local);
#endif
		char buffer[64] = {};
		std::strftime(buffer, sizeof(buffer), "screenshot_%Y%m%d_%H%M%S.png", &local);
		return std::string("screenshots/") + buffer;
	}
}

void CS230::CaptureScreenshot()
{
	const Math::ivec2 size = Engine::GetWindow().GetSize();
	const int		  w	   = size.x;
	const int		  h	   = size.y;
	if (w <= 0 || h <= 0)
	{
		Engine::GetLogger().LogError("Screenshot skipped: invalid window size.");
		return;
	}

	const int					 channels = 4;
	std::vector<unsigned char>	 pixels(static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(channels));

	GL::PixelStorei(GL_PACK_ALIGNMENT, 1);
	GL::ReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	// OpenGL's origin is the bottom-left; PNG rows go top-to-bottom.
	stbi_flip_vertically_on_write(1);

	std::error_code ec;
	std::filesystem::create_directories("screenshots", ec);

	const std::string path = MakeTimestampedPath();
	const int		  ok   = stbi_write_png(path.c_str(), w, h, channels, pixels.data(), w * channels);
	if (ok != 0)
	{
		Engine::GetLogger().LogEvent("Screenshot saved: " + path);
	}
	else
	{
		Engine::GetLogger().LogError("Screenshot failed to write: " + path);
	}
}
