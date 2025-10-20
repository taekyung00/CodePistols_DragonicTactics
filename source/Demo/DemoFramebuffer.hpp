/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Engine/GameState.hpp"
#include "Engine/Vec2.hpp"
#include "OpenGL/Framebuffer.hpp"
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace CS230
{
    class Texture;
}

class DemoFramebuffer : public CS230::GameState
{
public:
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw()  override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:

    struct AnimationFrame
    {
        int    frameIndex{};
        double duration{}; // in seconds
    };

    struct Animation
    {
        std::string                 name{};
        std::vector<AnimationFrame> frames{};
        int                         loopFrame{ 0 };
    };

    enum class RobotAnimation
    {
        None         = 0,
        RobotDead    = 1,
        RobotWalking = 2,
        RobotAngry   = 3
    };

    enum class CatAnimation
    {
        CatIdle    = 0,
        CatRunning = 1,
        CatJumping = 2,
        CatFalling = 3
    };

    // Robot sprite sheet info
    static constexpr Math::ivec2 ROBOT_HOT_SPOT{ 30, 20 };
    static constexpr Math::ivec2 ROBOT_FRAME_SIZE{ 63, 127 };
    static constexpr int         ROBOT_NUM_FRAMES = 5;

    // Cat sprite sheet info
    static constexpr Math::ivec2 CAT_HOT_SPOT{ 65, 15 };
    static constexpr Math::ivec2 CAT_FRAME_SIZE{ 128, 128 };
    static constexpr int         CAT_NUM_FRAMES = 10;

    // Cat frame positions in texture (texel coordinates)
    static constexpr std::array<Math::ivec2, CAT_NUM_FRAMES> CAT_FRAME_POSITIONS = {
        Math::ivec2{   0,   0 }, // Frame 0
        Math::ivec2{ 128,   0 }, // Frame 1
        Math::ivec2{ 256,   0 }, // Frame 2
        Math::ivec2{ 384,   0 }, // Frame 3
        Math::ivec2{ 512,   0 }, // Frame 4
        Math::ivec2{   0, 128 }, // Frame 5
        Math::ivec2{ 128, 128 }, // Frame 6
        Math::ivec2{ 256, 128 }, // Frame 7
        Math::ivec2{ 384, 128 }, // Frame 8
        Math::ivec2{ 512, 128 }  // Frame 9
    };

    std::vector<std::shared_ptr<CS230::Texture>> backgroundTextures;
    std::shared_ptr<CS230::Texture>              robotTexture = nullptr;
    std::shared_ptr<CS230::Texture>              catTexture   = nullptr;

    // Animation data
    std::vector<Animation> robotAnimations;
    std::vector<Animation> catAnimations;

    struct CatState
    {
        CatAnimation animation{};
        int          frameIndex{};
        double       timer{};
        Math::vec2   position{};
        bool         faceRight{};
    };

    struct RobotState
    {
        RobotAnimation animation{};
        int            frameIndex{};
        double         timer{};
        Math::vec2     position{};
        bool           faceRight{};
        double         walkDirection{}; // 1 for right, -1 for left
        double         walkSpeed{};     // pixels per second
    };

    CatState   idleCat;
    CatState   jumpingCat;
    RobotState walkingRobot;

    struct RenderInfo
    {
        OpenGL::FramebufferWithColor Target{};
        Math::ivec2                  Size{};
        std::array<GLfloat, 4>       ClearColor{};
        std::array<GLint, 4>         Viewport{};
    };

    struct WindParticle
    {
        Math::vec2 position{};
        Math::vec2 velocity{};
        double     size{};
        double     alpha{};
        double     lifeTimer{};
        double     maxLife{};
    };

    std::vector<WindParticle> windParticles;

    // ImGui control variables
    bool enableFramebufferOverlay = true;

    // Wind particle controls
    int   particleCount        = 3;
    int   targetParticleCount  = 50;
    float currentParticleCount = 3.0f;
    float windSpeedMin         = 80.0f;
    float windSpeedMax         = 140.0f;
    float particleSizeMin      = 1.5f;
    float particleSizeMax      = 4.5f;
    float particleColor[3]     = { 0.9f, 0.8f, 0.6f }; // RGB color
    float windDirection        = 0.0f;
    float targetWindDirection  = 0.0f;

    // Store the last rendered framebuffer texture for ImGui display
    mutable GLuint lastFramebufferTexture = 0;

private:
    void       initializeRobotAnimations();
    void       initializeCatAnimations();
    void       updateRobotAnimation(RobotState& character, double delta_time);
    void       updateCatAnimation(CatState& character, double delta_time);
    void       drawRobot(const RobotState& character) const;
    void       drawCat(const CatState& character) const;
    void       initializeWindParticles();
    void       spawnWindParticle(WindParticle& particle) const;
    void       updateWindParticles(double delta_time);
    void       drawWindParticles() const;
    RenderInfo beginOffscreenRendering() const;
    GLuint     endOffscreenRendering(const RenderInfo& render_info) const;
};
