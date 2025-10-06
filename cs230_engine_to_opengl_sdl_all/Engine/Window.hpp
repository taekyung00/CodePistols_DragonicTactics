/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "CS200/RGBA.hpp"
#include "Vec2.hpp"
#include <functional>
#include <gsl/gsl>
#include <string_view>

struct SDL_Window;
typedef void*           SDL_GLContext;
typedef union SDL_Event SDL_Event;

namespace CS230
{
    /**
     * \brief Cross-platform window management system with OpenGL context integration
     *
     * Window provides a complete windowing solution built on top of SDL2 that handles
     * window creation, OpenGL context management, event processing, and display operations.
     * The class abstracts away the complexities of cross-platform window management while
     * providing a clean, modern C++ interface for graphics applications.
     *
     * Core Functionality:
     * - Cross-platform window creation and management (Windows, macOS, Linux, Web)
     * - OpenGL context creation with proper version and profile selection
     * - Event handling system for window and input events
     * - Automatic resource cleanup through RAII principles
     * - Adaptive VSync and display synchronization
     * - Window resizing and size management
     *
     * SDL2 Integration:
     * The class wraps SDL2 functionality to provide modern C++ interfaces while
     * maintaining compatibility with existing SDL-based code. It handles SDL
     * initialization, error checking, and proper cleanup automatically.
     *
     * OpenGL Context Management:
     * Automatically creates and manages an OpenGL context with appropriate settings
     * for 2D graphics applications, including proper buffer configuration, multisampling
     * for anti-aliasing, and compatibility with both desktop and web platforms.
     *
     * Event System:
     * Provides a flexible callback-based event system that allows other engine
     * components (Input, ImGui, etc.) to receive and process window and input events
     * while maintaining clean separation of concerns.
     *
     * Resource Management:
     * Uses RAII principles to ensure that SDL and OpenGL resources are properly
     * cleaned up, preventing resource leaks and ensuring graceful application
     * shutdown even in the presence of exceptions.
     *
     * The Window class serves as the foundation for all graphics operations in the
     * engine, providing the display surface and rendering context needed for OpenGL
     * operations while abstracting platform-specific windowing details.
     */
    class Window
    {
    public:
        /**
         * \brief Initialize and create the window with OpenGL context
         * \param title Text to display in the window's title bar
         *
         * Performs complete window and OpenGL context initialization, including SDL
         * setup, window creation, OpenGL context creation, and graphics API initialization.
         * This function must be called before any rendering operations can take place.
         *
         * Initialization Sequence:
         * - Initializes SDL2 video subsystem with error checking
         * - Configures OpenGL context attributes (version, profile, buffers)
         * - Creates the application window with appropriate flags and positioning
         * - Creates and activates the OpenGL rendering context
         * - Initializes GLEW for OpenGL extension loading
         * - Configures VSync (adaptive or standard) for smooth rendering
         * - Initializes the rendering API for 2D graphics operations
         *
         * Window Configuration:
         * The window is created with default dimensions (800x600) and is positioned
         * at the center of the screen. It supports resizing and includes OpenGL
         * capabilities with multisampling for anti-aliasing.
         *
         * OpenGL Setup:
         * Automatically selects appropriate OpenGL version and profile based on
         * the target platform (Core profile for desktop, ES for web), with
         * proper buffer configuration for color, depth, and stencil operations.
         *
         * Error Handling:
         * If any part of the initialization fails (SDL init, window creation,
         * context creation, etc.), the function will throw appropriate exceptions
         * with descriptive error messages to aid in debugging.
         */
        void Start(std::string_view title);

        /**
         * \brief Process window events and update display
         *
         * Handles the main window update cycle including buffer swapping for double
         * buffering and processing all pending window and input events. This function
         * should be called once per frame to maintain responsive window behavior
         * and smooth graphics display.
         *
         * Display Operations:
         * - Swaps the front and back buffers to display the rendered frame
         * - Ensures smooth animation through proper buffer management
         * - Maintains VSync synchronization for tear-free rendering
         *
         * Event Processing:
         * - Polls and processes all pending SDL events
         * - Forwards events to registered callback functions for input handling
         * - Handles window-specific events (close, resize, focus changes)
         * - Updates internal window state based on events
         *
         * Window State Updates:
         * - Tracks window close requests from user or system
         * - Updates window size information when resize events occur
         * - Logs significant window events for debugging purposes
         *
         * The function ensures that the window remains responsive to user input
         * and system events while providing smooth graphics display through
         * proper buffer management and event handling.
         */
        void Update();

        /**
         * \brief Check if the window has been closed
         * \return True if the window should close, false if it should remain open
         *
         * Returns the current close state of the window, which can be triggered by
         * various user actions or system events. This is typically used in the
         * main application loop to determine when to exit the program.
         *
         * Close Triggers:
         * - User clicking the window's close button (X)
         * - System-initiated close requests (Alt+F4, Cmd+Q, etc.)
         * - SDL_QUIT events from the operating system
         * - Programmatic close requests through window events
         *
         * The function provides a clean way to check for termination conditions
         * without requiring direct event handling in the main application code.
         */
        bool IsClosed() const;

        /**
         * \brief Get the current window dimensions
         * \return Vector containing the current window width and height in pixels
         *
         * Returns the current size of the window's drawable area, which may change
         * during the application's lifetime due to user resizing or programmatic
         * size changes. This information is essential for proper viewport setup
         * and coordinate system calculations.
         *
         * Size Information:
         * - Reflects the actual drawable area available for rendering
         * - Updates automatically when the window is resized
         * - Accounts for platform-specific window decorations and scaling
         * - Provides pixel-accurate dimensions for graphics operations
         *
         * Common uses:
         * - Setting up viewport and projection matrices
         * - Calculating aspect ratios for proper rendering
         * - Determining layout positions for UI elements
         * - Handling responsive design based on window size
         */
        Math::ivec2 GetSize() const;

        /**
         * \brief Clear the window with a specified background color
         * \param color RGBA color value to use for clearing the window
         *
         * Clears the entire window surface with the specified color, preparing
         * it for new rendering operations. This is typically called at the
         * beginning of each frame to provide a clean background for drawing.
         *
         * Clearing Operations:
         * - Sets the OpenGL clear color to the specified value
         * - Clears the color buffer to ensure a clean drawing surface
         * - Prepares the window for new rendering operations
         * - Ensures consistent background appearance across frames
         *
         * Color Format:
         * The color parameter uses the engine's standard RGBA format, allowing
         * for full color specification including transparency values, though
         * transparency may not be visible in the final window display.
         *
         * This function integrates with the rendering API to ensure that
         * clearing operations are performed efficiently and correctly within
         * the current rendering context.
         */
        void Clear(CS200::RGBA color);

        /**
         * \brief Programmatically resize the window to specific dimensions
         * \param desired_width New window width in pixels
         * \param desired_height New window height in pixels
         *
         * Forces the window to resize to the specified dimensions, which can be
         * useful for implementing fullscreen modes, fixed aspect ratios, or
         * programmatic window management based on application requirements.
         *
         * Resize Operations:
         * - Immediately changes the window size through SDL
         * - Triggers window resize events that update internal state
         * - May be subject to platform-specific size constraints
         * - Automatically updates the drawable area and viewport
         *
         * The resize operation works in conjunction with the event system to
         * ensure that internal size tracking remains accurate and that other
         * systems are notified of the size change through normal event channels.
         */
        void ForceResize(int desired_width, int desired_height);

        /**
         * \brief Get direct access to the underlying SDL window object
         * \return Pointer to the SDL_Window for advanced SDL operations
         *
         * Provides access to the raw SDL window handle for advanced operations
         * that require direct SDL functionality not exposed by the Window class.
         * This method should be used sparingly and only when high-level
         * functionality is insufficient.
         *
         * Advanced use cases:
         * - Integration with third-party libraries that require SDL handles
         * - Direct SDL operations not wrapped by the Window class
         * - Platform-specific window manipulation
         * - Custom event handling or window property access
         *
         * Handle Ownership:
         * The returned pointer remains owned by the Window object and should
         * not be manually destroyed or transferred. The handle becomes invalid
         * when the Window object is destroyed.
         */
        SDL_Window* GetSDLWindow() const;

        /**
         * \brief Get direct access to the OpenGL rendering context
         * \return SDL_GLContext handle for advanced OpenGL operations
         *
         * Provides access to the OpenGL context handle for advanced graphics
         * operations that require direct context manipulation or integration
         * with systems that need explicit context access.
         *
         * Advanced use cases:
         * - Context sharing between multiple windows or threads
         * - Direct OpenGL context state management
         * - Integration with graphics libraries requiring context handles
         * - Custom rendering setup or debugging operations
         *
         * Context Ownership:
         * The returned context remains owned by the Window object and should
         * not be manually destroyed. The context becomes invalid when the
         * Window object is destroyed.
         */
        SDL_GLContext GetGLContext() const;

        /**
         * \brief Function type for window event callbacks
         *
         * Defines the signature for callback functions that receive window and
         * input events. These callbacks enable other engine systems to respond
         * to user input and window events without directly coupling to the
         * window implementation.
         */
        using WindowEventCallback = std::function<void(const SDL_Event&)>;

        /**
         * \brief Register a callback function to receive window events
         * \param callback Function to call for each window event
         *
         * Sets up an event callback that will be invoked for every SDL event
         * processed by the window. This enables other engine systems (Input,
         * ImGui, etc.) to receive and process events while maintaining clean
         * separation between the window system and event consumers.
         *
         * Event Flow:
         * - Window processes SDL events during Update()
         * - Registered callback receives each event for custom handling
         * - Window continues with internal event processing
         * - Multiple systems can layer their event handling through callbacks
         *
         * Callback Responsibilities:
         * The callback function should handle events efficiently and avoid
         * blocking operations, as it's called during the main update cycle.
         * Event filtering and routing should be performed within the callback.
         *
         * Common uses:
         * - Input system event processing
         * - ImGui event forwarding
         * - Custom application event handling
         * - Debug and logging systems
         */
        void SetEventCallback(WindowEventCallback callback);

        /**
         * \brief Default constructor creating an uninitialized window
         *
         * Creates a Window object in an uninitialized state. The window is not
         * usable until Start() is called to perform the actual initialization
         * of SDL, window creation, and OpenGL context setup.
         */
        Window() = default;

        /**
         * \brief Destructor ensuring proper cleanup of window and OpenGL resources
         *
         * Automatically cleans up all SDL and OpenGL resources when the Window
         * object is destroyed, including the OpenGL context, SDL window, and
         * SDL subsystem. This ensures no resource leaks occur even if the
         * application terminates unexpectedly.
         *
         * Cleanup sequence:
         * - Destroys the OpenGL context
         * - Destroys the SDL window
         * - Shuts down SDL subsystems
         * - Ensures all resources are properly released
         */
        ~Window();

        /**
         * \brief Deleted copy and move operations for singleton-like behavior
         *
         * The Window class is designed to manage a single window instance and
         * its associated resources. Copy and move operations are disabled to
         * prevent resource management complications and ensure clear ownership
         * of the underlying SDL and OpenGL resources.
         *
         * This design enforces that each Window object represents exactly one
         * window throughout its lifetime, simplifying resource management and
         * preventing accidental duplication of system resources.
         */
        Window(const Window&)                = delete;
        Window& operator=(const Window&)     = delete;
        Window(Window&&) noexcept            = delete;
        Window& operator=(Window&&) noexcept = delete;

    private:
        // TODO private data and helper member functions here
        // TODO default size should be 800x600
    };
}
