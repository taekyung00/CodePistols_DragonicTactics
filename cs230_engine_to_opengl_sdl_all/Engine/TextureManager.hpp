/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <filesystem>
#include <map>
#include <vector>

namespace CS230
{
    class Texture;

    /**
     * \brief Centralized texture resource management system with caching and render-to-texture capabilities
     *
     * TextureManager serves as the central hub for all texture-related operations in the engine,
     * providing efficient resource management, automatic caching, and advanced render-to-texture
     * functionality. The manager prevents duplicate texture loading, handles memory cleanup,
     * and provides tools for creating procedural textures through render-to-texture operations.
     *
     * Core Responsibilities:
     * - Texture loading and caching to prevent duplicate GPU resources
     * - Memory management and cleanup of all texture resources
     * - Render-to-texture functionality for creating dynamic textures
     * - Resource lifecycle tracking and automatic cleanup
     *
     * Caching System:
     * The manager maintains a cache of loaded textures indexed by file path, ensuring that
     * the same image file is never loaded multiple times into GPU memory. This significantly
     * reduces memory usage and loading times when the same textures are referenced by
     * multiple game objects or UI elements.
     *
     * Render-to-Texture Features:
     * Provides a complete render-to-texture system that allows drawing operations to be
     * captured into texture objects rather than displayed on screen. This enables:
     * - Procedural texture generation through drawing operations
     * - Text rendering to textures for efficient text display
     * - Dynamic UI element creation and caching
     * - Off-screen rendering for special effects and post-processing
     *
     * Resource Management:
     * All textures created through the manager are automatically tracked and cleaned up
     * during shutdown, preventing memory leaks and ensuring proper resource disposal.
     * The manager handles both file-loaded textures and runtime-generated textures
     * through a unified interface.
     *
     * Integration with Engine:
     * The TextureManager integrates seamlessly with the 2D renderer and coordinate
     * system, automatically handling viewport management and coordinate transformations
     * during render-to-texture operations while preserving the normal rendering state.
     */
    class TextureManager
    {
    public:
        /**
         * \brief Load a texture from file with automatic caching
         * \param file_name Path to the image file to load
         * \return Pointer to the loaded texture (cached if previously loaded)
         *
         * Loads a texture from the specified image file, utilizing an internal cache
         * to ensure that the same file is never loaded multiple times. If the texture
         * has been previously loaded, the cached version is returned immediately
         * without performing file I/O or GPU operations.
         *
         * Supported Formats:
         * The function supports common image formats through the underlying image
         * loading system, including PNG, JPEG, BMP, and other formats supported
         * by the engine's image loader.
         *
         * Caching Behavior:
         * - First load: Reads file, creates GPU texture, stores in cache
         * - Subsequent loads: Returns cached texture immediately
         * - Cache key: Full file path (different paths to same file create separate entries)
         * - Memory efficiency: Prevents duplicate GPU resources for same image
         *
         * Resource Ownership:
         * The returned texture remains owned by the TextureManager and will be
         * automatically cleaned up when Unload() is called or the manager is
         * destroyed. Callers should not manually delete the returned texture.
         *
         * Error Handling:
         * If the file cannot be loaded (missing file, unsupported format, etc.),
         * the function behavior depends on the underlying image loading system.
         * The manager logs loading events for debugging and monitoring purposes.
         *
         * Performance Characteristics:
         * - First load: File I/O + GPU texture creation overhead
         * - Cached loads: Very fast hash table lookup with no I/O
         * - Memory usage: One GPU texture per unique file path
         */
        Texture* Load(const std::filesystem::path& file_name);

        /**
         * \brief Unload and clean up all managed textures
         *
         * Performs comprehensive cleanup of all textures managed by the TextureManager,
         * including both file-loaded textures and render-to-texture generated textures.
         * This function ensures proper GPU memory cleanup and prevents resource leaks.
         *
         * Cleanup Operations:
         * - Deletes all cached file-based textures and clears the cache
         * - Deletes all render-to-texture generated textures
         * - Releases associated GPU memory and OpenGL resources
         * - Logs cleanup operation for debugging purposes
         *
         * When to Call:
         * - Application shutdown to ensure clean resource cleanup
         * - Level transitions when textures are no longer needed
         * - Memory management when switching between different content sets
         * - Error recovery situations requiring resource reset
         *
         * Post-Cleanup State:
         * After calling Unload(), all previously returned texture pointers become
         * invalid and should not be used. The manager returns to its initial empty
         * state and is ready to load new textures.
         *
         */
        void Unload();

        /**
         * \brief Begin render-to-texture mode for creating dynamic textures
         * \param width Width of the render target texture in pixels
         * \param height Height of the render target texture in pixels
         *
         * Initiates render-to-texture mode by setting up a framebuffer and redirecting
         * all subsequent drawing operations to a texture instead of the screen. This
         * powerful feature enables the creation of dynamic textures through normal
         * rendering operations.
         *
         * Setup Operations:
         * - Creates a framebuffer with color attachment of specified dimensions
         * - Saves current viewport and clear color settings for restoration
         * - Switches rendering target from screen to the new texture
         * - Adjusts coordinate system for proper texture-space rendering
         * - Clears the render target with transparent black (0,0,0,0)
         *
         * Coordinate System:
         * The render-to-texture system automatically adjusts the coordinate system
         * to ensure that drawing operations appear correctly in the generated texture,
         * handling differences between screen and texture coordinate conventions.
         *
         * Common Use Cases:
         * - Text rendering: Draw text using font systems to create text textures
         * - UI caching: Render complex UI layouts to textures for performance
         * - Procedural generation: Create textures through algorithmic drawing
         * - Dynamic graphics: Generate textures based on game state or user input
         *
         * Rendering State:
         * During render-to-texture mode, normal 2D rendering operations work as
         * expected, but the output goes to the texture rather than the screen.
         * The engine's transformation and rendering systems function normally.
         *
         * Important Notes:
         * - Must be paired with EndRenderTextureMode() to complete the operation
         * - All drawing between Start and End calls contributes to the texture
         * - The render target is cleared to transparent at the beginning
         */
        void StartRenderTextureMode(int width, int height);

        /**
         * \brief Complete render-to-texture mode and return the generated texture
         * \return Pointer to the newly created texture containing the rendered content
         *
         * Finalizes the render-to-texture operation and returns a texture object
         * containing all the drawing operations performed since StartRenderTextureMode()
         * was called. The function handles all necessary cleanup and state restoration.
         *
         * Finalization Operations:
         * - Completes any pending rendering operations to the texture
         * - Restores the original viewport and rendering target (screen)
         * - Restores previous clear color and rendering state
         * - Creates a Texture object wrapping the generated content
         * - Cleans up the temporary framebuffer resources
         *
         * State Restoration:
         * The function carefully restores all rendering state that was modified
         * during render-to-texture mode, ensuring that subsequent screen rendering
         * continues normally with the previous settings and coordinate systems.
         *
         * Texture Ownership:
         * The returned texture is owned by the TextureManager and will be automatically
         * cleaned up when Unload() is called. The texture can be used immediately
         * for rendering operations just like any file-loaded texture.
         *
         * Generated Texture Properties:
         * - Size: Matches the dimensions specified in StartRenderTextureMode()
         * - Format: RGBA with alpha channel support for transparency
         * - Content: All drawing operations performed between Start and End calls
         * - Usage: Can be drawn, transformed, and used like any other texture
         *
         * Error Handling:
         * If render-to-texture operations fail, the function handles cleanup
         * gracefully and may return nullptr or throw appropriate exceptions
         * depending on the specific failure mode.
         *
         * Performance Considerations:
         * Render-to-texture operations involve GPU framebuffer switching and
         * texture creation, so they should be used judiciously. Consider caching
         * generated textures when the same content will be used multiple times.
         */
        Texture* EndRenderTextureMode();

    private:
        // Core data structures for texture resource management
        // These containers track all textures owned by the manager for proper cleanup

        std::map<std::filesystem::path, Texture*> textures;          // Cache of file-loaded textures indexed by path
        std::vector<Texture*>                     rendered_textures; // Collection of render-to-texture generated textures
    };
}
