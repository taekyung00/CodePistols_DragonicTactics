/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "DemoFramebuffer.hpp"

#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "DemoShapes.hpp"
#include "DemoText.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Random.hpp"
#include "Engine/Texture.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include "OpenGL/GL.hpp"
#include "../Game/MainMenu.h"

#include <cmath>
#include <imgui.h>

void DemoFramebuffer::Load()
{
    auto&      texture_manager        = Engine::GetTextureManager();
    const auto background_image_paths = { "Assets/images/DemoFramebuffer/Planets.png", "Assets/images/DemoFramebuffer/Ships.png", "Assets/images/DemoFramebuffer/Foreground.png" };
    for (const auto& path : background_image_paths)
    {
        backgroundTextures.push_back(texture_manager.Load(path));
    }

    robotTexture = texture_manager.Load("Assets/images/DemoFramebuffer/Robot.png");
    catTexture   = texture_manager.Load("Assets/images/DemoFramebuffer/Cat.png");

    initializeRobotAnimations();
    initializeCatAnimations();

    CS200::RenderingAPI::SetClearColor(0x2E3440FF);

    // Initialize character states
    idleCat.animation  = CatAnimation::CatIdle;
    idleCat.frameIndex = 0;
    idleCat.timer      = 0.0;
    idleCat.position   = Math::vec2{ 70.0, 80.0 };
    idleCat.faceRight  = true;

    jumpingCat.animation  = CatAnimation::CatJumping;
    jumpingCat.frameIndex = 0;
    jumpingCat.timer      = 0.0;
    jumpingCat.position   = Math::vec2{ 700.0, 80.0 };
    jumpingCat.faceRight  = true;

    walkingRobot.animation     = RobotAnimation::RobotWalking;
    walkingRobot.frameIndex    = 0;
    walkingRobot.timer         = 0.0;
    walkingRobot.position      = Math::vec2{ 400.0, 80.0 };
    walkingRobot.faceRight     = true;
    walkingRobot.walkDirection = 1.0;   // 1 for right, -1 for left
    walkingRobot.walkSpeed     = 100.0; // pixels per second

    initializeWindParticles();
}

template <typename T, typename FLOAT = double>
static void ease_to_target(T& current, const T& target, FLOAT delta_time, FLOAT weight = 1.0)
{
    const auto easing = std::min(delta_time * weight, static_cast<FLOAT>(1.0));
    current += easing * (target - current);
}

void DemoFramebuffer::Update([[maybe_unused]]double dt)
{
    const auto&  environment = Engine::GetWindowEnvironment();
    const double delta_time  = environment.DeltaTime;

    // Ease wind direction towards target
    ease_to_target(windDirection, targetWindDirection, static_cast<float>(delta_time), 1.0f);

    ease_to_target(currentParticleCount, static_cast<float>(targetParticleCount), static_cast<float>(delta_time), 0.5f);
    const int newParticleCount = static_cast<int>(std::round(currentParticleCount));
    if (newParticleCount != particleCount)
    {
        particleCount = newParticleCount;
        windParticles.resize(static_cast<size_t>(particleCount));
    }

    // Update idle cat
    updateCatAnimation(idleCat, delta_time);

    // Update jumping cat with vertical movement
    updateCatAnimation(jumpingCat, delta_time);
    // Add jumping motion (simple sine wave based on animation progress)
    const double jump_progress = jumpingCat.timer / catAnimations[static_cast<int>(CatAnimation::CatJumping)].frames[0].duration;
    const double jump_height   = 150.0 * std::sin(jump_progress * 3.14159); // Simple arc
    jumpingCat.position.y      = 80.0 + jump_height;

    // Update walking robot with horizontal movement
    updateRobotAnimation(walkingRobot, delta_time);
    walkingRobot.position.x += walkingRobot.walkDirection * walkingRobot.walkSpeed * delta_time;

    if (walkingRobot.position.x <= 130.0)
    {
        walkingRobot.walkDirection = 1.0;
        walkingRobot.faceRight     = true;
    }
    else if (walkingRobot.position.x >= 670.0)
    {
        walkingRobot.walkDirection = -1.0;
        walkingRobot.faceRight     = false;
    }

    updateWindParticles(delta_time);
}

void DemoFramebuffer::Draw() 
{
    CS200::RenderingAPI::Clear();

    auto& renderer_2d          = Engine::GetRenderer2D();
    const auto [width, height] = Engine::GetWindow().GetSize();

    // Begin offscreen rendering for wind particles
    const auto render_info = beginOffscreenRendering();

    // Render wind particles to offscreen buffer
    drawWindParticles();

    // End offscreen rendering and get the scene texture
    const auto scene_texture = endOffscreenRendering(render_info);

    // Store texture for ImGui display (clean up previous texture)
    if (lastFramebufferTexture != 0)
    {
        GL::DeleteTextures(1, &lastFramebufferTexture);
    }
    lastFramebufferTexture = scene_texture;

    // Render main scene to screen
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    // Draw background
    for (const auto& texture : backgroundTextures)
    {
        texture->Draw(Math::TransformationMatrix{});
    }

    // Draw characters
    drawCat(idleCat);
    drawCat(jumpingCat);
    drawRobot(walkingRobot);

    // Conditionally draw scene texture overlay
    if (enableFramebufferOverlay)
    {
        const auto to_center = Math::TranslationMatrix(Math::vec2{ static_cast<double>(width) / 2.0, static_cast<double>(height) / 2.0 });
        const auto scale     = Math::ScaleMatrix({ static_cast<double>(width), static_cast<double>(height) });
        const auto rotate    = Math::RotationMatrix(static_cast<double>(windDirection));
        const auto transform = to_center * rotate * scale;
        renderer_2d.DrawQuad(transform, scene_texture);
    }

    renderer_2d.EndScene();

    // Note: Don't delete scene_texture here anymore since we're storing it for ImGui
}

void DemoFramebuffer::DrawImGui()
{
    // Main control panel
    if (ImGui::Begin("Demo Controls"))
    {
        ImGui::Checkbox("Enable Framebuffer Overlay", &enableFramebufferOverlay);

        ImGui::SeparatorText("Wind Particle System Controls");
        ImGui::SliderAngle("Wind Direction", &targetWindDirection, 0.0f, 360.0f);
        ImGui::SliderInt("Particle Count", &targetParticleCount, 0, 100);

        // Wind speed and direction controls
        ImGui::Text("Wind Speed Range:");
        ImGui::SliderFloat("Min Speed", &windSpeedMin, 10.0f, 200.0f, "%.1f px/s");
        ImGui::SliderFloat("Max Speed", &windSpeedMax, 10.0f, 201.0f, "%.1f px/s");
        if (windSpeedMin >= windSpeedMax)
        {
            windSpeedMax = windSpeedMin + 1.0f;
        }

        ImGui::Text("Particle Size Range:");
        ImGui::SliderFloat("Min Size", &particleSizeMin, 0.5f, 10.0f, "%.1f px");
        ImGui::SliderFloat("Max Size", &particleSizeMax, 0.5f, 11.0f, "%.1f px");
        if (particleSizeMin >= particleSizeMax)
        {
            particleSizeMax = particleSizeMin + 1.0f;
        }

        ImGui::Text("Particle Color:");
        ImGui::ColorEdit3("Color", particleColor, ImGuiColorEditFlags_Float);

        ImGui::SeparatorText("Framebuffer Information");
        if (lastFramebufferTexture != 0)
        {
            const auto [width, height] = Engine::GetWindow().GetSize();
            const auto fb_width        = width / 2;
            const auto fb_height       = height / 2;
            ImGui::Text("Resolution: %dx%d", fb_width, fb_height);

            ImGui::SeparatorText("Texture Viewer");
            const float aspect_ratio   = static_cast<float>(fb_width) / static_cast<float>(fb_height);
            const float window_width   = ImGui::GetContentRegionAvail().x;
            const float display_width  = std::max(window_width - 20.0f, 20.0f);
            const float display_height = display_width / aspect_ratio;
            ImGui::Text("Wind Particles Texture");

            // Calculate contrasting background color based on particle color
            // Convert particle color to perceived brightness using luminance formula
            const float  luminance     = 0.299f * particleColor[0] + 0.587f * particleColor[1] + 0.114f * particleColor[2];
            const float  contrast_grey = 1.0f - (3.0f * luminance * luminance - 2.0f * luminance * luminance * luminance);
            const ImVec4 background_color(contrast_grey, contrast_grey, contrast_grey, 1.0f);

            // Note: ImGui expects texture coordinates with (0,0) at top-left, but OpenGL has (0,0) at bottom-left
            // So we need to flip the V coordinate
            ImGui::ImageWithBg(
                static_cast<ImTextureID>(lastFramebufferTexture), ImVec2(display_width, display_height), ImVec2(0, 1), // uv0 - top-left in ImGui = bottom-left in OpenGL
                ImVec2(1, 0), background_color);
        }

        ImGui::SeparatorText("Switch Demo");
        if (ImGui::Button("Switch to Demo Shapes"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoShapes>();
        }
        if (ImGui::Button("Switch to Demo Text"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoText>();
        }
        if (ImGui::Button("Switch to MainMenu"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<MainMenu>();
        }
    }
    ImGui::End();
}

void DemoFramebuffer::Unload()
{
    // Clean up stored framebuffer texture
    if (lastFramebufferTexture != 0)
    {
        GL::DeleteTextures(1, &lastFramebufferTexture);
        lastFramebufferTexture = 0;
    }
}

gsl::czstring DemoFramebuffer::GetName() const
{
    return "Demo Framebuffer";
}

void DemoFramebuffer::initializeRobotAnimations()
{
    robotAnimations.clear();
    robotAnimations.resize(4); // 4 animations: None, RobotDead, RobotWalking, RobotAngry

    // Animation: None - PlayFrame 0 10
    robotAnimations[static_cast<int>(RobotAnimation::None)] = { "None", { { 0, 10.0 } }, 0 };

    // Animation: RobotDead - PlayFrame 2 5
    robotAnimations[static_cast<int>(RobotAnimation::RobotDead)] = { "RobotDead", { { 2, 5.0 } }, 0 };

    // Animation: RobotWalking - PlayFrame 0 0.2, PlayFrame 1 0.2
    robotAnimations[static_cast<int>(RobotAnimation::RobotWalking)] = {
        "RobotWalking", { { 0, 0.2 }, { 1, 0.2 } },
         0
    };

    // Animation: RobotAngry - PlayFrame 3 0.1, PlayFrame 4 0.1
    robotAnimations[static_cast<int>(RobotAnimation::RobotAngry)] = {
        "RobotAngry", { { 3, 0.1 }, { 4, 0.1 } },
         0
    };
}

void DemoFramebuffer::initializeCatAnimations()
{
    catAnimations.clear();
    catAnimations.resize(4); // 4 animations: CatIdle, CatRunning, CatJumping, CatFalling

    // Animation: CatIdle
    catAnimations[static_cast<int>(CatAnimation::CatIdle)] = {
        "CatIdle",
        { { 0, 6.0 }, { 1, 0.5 }, { 2, 0.1 }, { 1, 0.5 }, { 2, 0.1 }, { 1, 0.5 }, { 0, 3.0 }, { 1, 0.5 }, { 2, 0.3 }, { 1, 0.5 },
          { 2, 0.3 }, { 1, 0.5 }, { 0, 3.0 }, { 1, 0.5 }, { 2, 1.0 }, { 1, 0.3 }, { 2, 2.0 }, { 3, 0.4 }, { 4, 0.4 }, { 5, 0.4 } },
        16  // Loop back to frame 16
    };

    // Animation: CatRunning - PlayFrame 6 .1, PlayFrame 0 .1, PlayFrame 7 .1, PlayFrame 0 .1, Loop 0
    catAnimations[static_cast<int>(CatAnimation::CatRunning)] = {
        "CatRunning", { { 6, 0.1 }, { 0, 0.1 }, { 7, 0.1 }, { 0, 0.1 } },
         0
    };

    // Animation: CatJumping - PlayFrame 9 3, Loop 0
    catAnimations[static_cast<int>(CatAnimation::CatJumping)] = { "CatJumping", { { 9, 3.0 } }, 0 };

    // Animation: CatFalling - PlayFrame 8 3, Loop 0
    catAnimations[static_cast<int>(CatAnimation::CatFalling)] = { "CatFalling", { { 8, 3.0 } }, 0 };
}

void DemoFramebuffer::updateCatAnimation(CatState& character, double delta_time)
{
    const auto& anim = catAnimations[static_cast<size_t>(character.animation)];
    character.timer += delta_time;
    const auto& current_frame = anim.frames[static_cast<size_t>(character.frameIndex)];
    if (character.timer >= current_frame.duration)
    {
        character.timer      = 0.0;
        character.frameIndex = (character.frameIndex + 1);
        if (static_cast<size_t>(character.frameIndex) >= anim.frames.size())
        {
            character.frameIndex = anim.loopFrame;
        }
    }
}

void DemoFramebuffer::updateRobotAnimation(RobotState& character, double delta_time)
{
    const auto& anim = robotAnimations[static_cast<size_t>(character.animation)];
    character.timer += delta_time;
    const auto& current_frame = anim.frames[static_cast<size_t>(character.frameIndex)];
    if (character.timer >= current_frame.duration)
    {
        character.timer      = 0.0;
        character.frameIndex = (character.frameIndex + 1);
        if (static_cast<size_t>(character.frameIndex) >= anim.frames.size())
        {
            character.frameIndex = anim.loopFrame;
        }
    }
}

void DemoFramebuffer::drawCat(const CatState& character) const
{
    const auto& anim         = catAnimations[static_cast<size_t>(character.animation)];
    const int   sprite_frame = anim.frames[static_cast<size_t>(character.frameIndex)].frameIndex;
    const auto  texel_base   = CAT_FRAME_POSITIONS[static_cast<size_t>(sprite_frame)];
    const auto  frame_size   = CAT_FRAME_SIZE;
    const auto  hot_spot     = CAT_HOT_SPOT;

    const auto to_center = Math::TranslationMatrix(Math::vec2{ static_cast<double>(-hot_spot.x), static_cast<double>(-hot_spot.y) });
    const auto scale     = character.faceRight ? Math::ScaleMatrix({ 1.0, 1.0 }) : Math::ScaleMatrix({ -1.0, 1.0 });
    const auto translate = Math::TranslationMatrix(character.position);
    const auto transform = translate * scale * to_center;

    catTexture->Draw(transform, texel_base, frame_size);
}

void DemoFramebuffer::drawRobot(const RobotState& character) const
{
    const auto& anim         = robotAnimations[static_cast<size_t>(character.animation)];
    const int   sprite_frame = anim.frames[static_cast<size_t>(character.frameIndex)].frameIndex;
    const auto  texel_base   = Math::ivec2{ sprite_frame * ROBOT_FRAME_SIZE.x, 0 };
    const auto  frame_size   = ROBOT_FRAME_SIZE;
    const auto  hot_spot     = ROBOT_HOT_SPOT;

    const auto to_center = Math::TranslationMatrix(Math::vec2{ static_cast<double>(-hot_spot.x), static_cast<double>(-hot_spot.y) });
    const auto scale     = character.faceRight ? Math::ScaleMatrix({ 1.0, 1.0 }) : Math::ScaleMatrix({ -1.0, 1.0 });
    const auto translate = Math::TranslationMatrix(character.position);
    const auto transform = translate * scale * to_center;

    robotTexture->Draw(transform, texel_base, frame_size);
}

void DemoFramebuffer::initializeWindParticles()
{
    windParticles.clear();
    for (int i = 0; i < particleCount; ++i)
    {
        WindParticle particle;
        spawnWindParticle(particle);
        windParticles.push_back(particle);
    }
}

void DemoFramebuffer::spawnWindParticle(WindParticle& particle) const
{
    const auto [width, height] = Engine::GetWindow().GetSize() / 2;
    // Always spawn from left edge for left-to-right movement
    particle.position          = Math::vec2{ -20.0, util::random(0.0, static_cast<double>(height)) };

    // Calculate velocity for left-to-right movement
    const double speed = util::random(static_cast<double>(windSpeedMin), static_cast<double>(windSpeedMax));
    particle.velocity  = Math::vec2{
        speed,                  // Horizontal movement (left to right)
        util::random(-5.0, 5.0) // Small vertical variation
    };

    particle.size      = util::random(static_cast<double>(particleSizeMin), static_cast<double>(particleSizeMax));
    particle.lifeTimer = 0.0;
    particle.maxLife   = util::random(3.0, 4.0 + 4 * width / 400.0);
}

void DemoFramebuffer::updateWindParticles(double delta_time)
{
    const auto [width, height] = Engine::GetWindow().GetSize() / 2;

    for (auto& particle : windParticles)
    {
        particle.position.x += particle.velocity.x * delta_time;
        particle.position.y += particle.velocity.y * delta_time + std::sin(particle.lifeTimer * 3.1415) * 0.1 * particle.size;
        particle.lifeTimer += delta_time;
        const double life_ratio = particle.lifeTimer / particle.maxLife;
        particle.alpha          = (1.0 - life_ratio);
        if (particle.position.x > width + 20 || particle.lifeTimer >= particle.maxLife)
        {
            spawnWindParticle(particle);
        }
    }
}

void DemoFramebuffer::drawWindParticles() const
{
    auto& renderer_2d = Engine::GetRenderer2D();

    for (const auto& particle : windParticles)
    {
        if (particle.alpha > 0.0)
        {
            // Create transform for particle
            const auto        translate  = Math::TranslationMatrix(particle.position);
            const auto        scale      = Math::ScaleMatrix({ particle.size, particle.size });
            const auto        transform  = translate * scale;
            // Set color with alpha using configurable color
            const float       alpha      = static_cast<float>(particle.alpha);
            const CS200::RGBA dust_color = CS200::pack_color({ particleColor[0], particleColor[1], particleColor[2], alpha });
            // Draw particle as a small circle
            renderer_2d.DrawCircle(transform, dust_color, CS200::CLEAR);
        }
    }
}

DemoFramebuffer::RenderInfo DemoFramebuffer::beginOffscreenRendering() const
{
    RenderInfo render_info;
    auto&      renderer_2d     = Engine::GetRenderer2D();
    const auto [width, height] = Engine::GetWindow().GetSize();

    // End current scene
    renderer_2d.EndScene();

    // Set up offscreen framebuffer
    render_info.Size   = { width / 2, height / 2 };
    render_info.Target = OpenGL::CreateFramebufferWithColor(render_info.Size);

    // Save current OpenGL state
    GL::GetFloatv(GL_COLOR_CLEAR_VALUE, render_info.ClearColor.data());
    GL::GetIntegerv(GL_VIEWPORT, render_info.Viewport.data());

    // Switch to offscreen rendering
    const auto ndc_matrix = Math::ScaleMatrix({ 1.0, 1.0 }) * CS200::build_ndc_matrix(render_info.Size);
    renderer_2d.BeginScene(ndc_matrix);
    GL::BindFramebuffer(GL_FRAMEBUFFER, render_info.Target.Framebuffer);
    GL::Viewport(0, 0, render_info.Size.x, render_info.Size.y);
    GL::ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    GL::Clear(GL_COLOR_BUFFER_BIT);

    return render_info;
}

GLuint DemoFramebuffer::endOffscreenRendering(const RenderInfo& render_info) const
{
    auto& renderer_2d = Engine::GetRenderer2D();

    // End offscreen scene
    renderer_2d.EndScene();

    // Restore OpenGL state
    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
    GL::Viewport(render_info.Viewport[0], render_info.Viewport[1], render_info.Viewport[2], render_info.Viewport[3]);
    GL::ClearColor(render_info.ClearColor[0], render_info.ClearColor[1], render_info.ClearColor[2], render_info.ClearColor[3]);

    // Clean up framebuffer but keep the color texture
    const auto scene_texture         = render_info.Target.ColorAttachment;
    auto       framebuffer_to_delete = render_info.Target.Framebuffer;
    GL::DeleteFramebuffers(1, &framebuffer_to_delete);

    return scene_texture;
}
