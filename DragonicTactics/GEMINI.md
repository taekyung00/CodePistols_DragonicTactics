# Dragonic Tactics: A C++ 2D Game Engine

## Project Overview

This is a 2D game engine written in C++20. The project, named "Dragonic Tactics," is built using CMake and targets Windows, Linux, and Web (via Emscripten). It serves as a foundation for creating 2D games, with a clear separation between the core engine and game-specific logic.

The engine provides a comprehensive set of features, including:
*   **Rendering:** Utilizes OpenGL for 2D graphics, managed by a rendering subsystem.
*   **Windowing & Input:** Uses the SDL2 library for cross-platform window and input handling.
*   **Architecture:** Employs a singleton pattern for the main `Engine` class, which manages various subsystems like a `GameStateManager`, `GameObjectManager`, `TextureManager`, and an `EventBus`. Game logic is built around a `GameObject` class that uses a component-based architecture and a state machine pattern.
*   **UI:** Integrates `Dear ImGui` for in-game debugging and development user interfaces.
*   **Dependencies:** Other libraries include `GLEW` for OpenGL extension loading, `GSL` (Guideline Support Library), and `stb_image` for image loading.

The project includes the game "Dragonic Tactics" and several smaller demos to showcase the engine's capabilities.

## Building and Running

The project uses a standard CMake workflow with presets defined in `CMakePresets.json`.

### Prerequisites

A C++20 compatible compiler (like MSVC on Windows or GCC/Clang on Linux) and CMake (version 3.21+) are required. For web builds, the Emscripten SDK must be installed and configured.

### Build Commands

You can configure and build the project using CMake presets from the command line.

**On Windows:**

1.  **Configure:**
    ```sh
    # For a standard debug build
    cmake --preset windows-debug
    ```

2.  **Build:**
    ```sh
    cmake --build --preset windows-debug
    ```

The executable will be located in the `build/windows-debug/source/` directory.

**Other Presets:**
*   `windows-developer-release`: Optimized build with debug info.
*   `windows-release`: Fully optimized release build.
*   `web-debug-on-windows`: WebAssembly debug build on Windows.

### Running the Game

After building, run the executable generated in the corresponding build directory.

## Development Conventions

*   **Language:** The project is written in C++20.
*   **Code Style:** A `.clang-format` file is present in the root directory, which defines the code formatting rules. Developers should use `clang-format` to maintain a consistent style.
*   **Architecture:** The code is structured into several layers:
    *   `source/Engine`: The core, reusable game engine components.
    *   `source/Game`: Game-specific logic, including different game states like `MainMenu` and `Splash`.
    *   `source/OpenGL`: A wrapper around OpenGL objects.
    *   `source/CS200`: Contains various helper modules for graphics, including an immediate mode 2D renderer.
*   **Game Objects:** New game entities should inherit from the `CS230::GameObject` class and implement its virtual methods. Functionality can be extended by adding components that inherit from `CS230::Component`.
*   **Game States:** The application flow is managed by a `GameStateManager`. New screens or major game modes should be implemented as new classes inheriting from `CS230::GameState`.
