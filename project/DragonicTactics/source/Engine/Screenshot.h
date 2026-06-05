/**
 * \file
 * \brief Capture the current window framebuffer to a timestamped PNG on disk.
 */
#pragma once

namespace CS230
{
	// Reads the back buffer and writes it to screenshots/screenshot_<timestamp>.png.
	// Intended to be called at the end of a frame, after rendering.
	void CaptureScreenshot();
}
