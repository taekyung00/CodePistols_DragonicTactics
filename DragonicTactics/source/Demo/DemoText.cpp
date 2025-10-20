/**
 * \file
 * \author Generated
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "DemoText.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "DemoFramebuffer.hpp"
#include "DemoShapes.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Window.hpp"
#include "../Game/MainMenu.h"
#include <imgui.h>
#include <sstream>

void DemoText::Load()
{
    CS200::RenderingAPI::SetClearColor(0x2E3440FF);

    simpleFont   = std::make_unique<CS230::Font>("Assets/fonts/Font_Simple.png");
    outlinedFont = std::make_unique<CS230::Font>("Assets/fonts/Font_Outlined.png");

    updateCachedTextures();
}

void DemoText::Update([[maybe_unused]]double dt)
{
    const auto& environment = Engine::GetWindowEnvironment();

    if (environment.ElapsedTime - lastOccasionalTextUpdate >= settings.OccasionalUpdateInterval)
    {
        lastOccasionalTextUpdate = environment.ElapsedTime;
        occasionalCounter++;
        occasionalText = "Sometimes changing: " + std::to_string(occasionalCounter);
    }

    if (environment.ElapsedTime - lastFrequentTextUpdate >= settings.FrequentUpdateInterval)
    {
        lastFrequentTextUpdate = environment.ElapsedTime;
        frequentCounter++;
        frequentText = "Frequent: " + std::to_string(frequentCounter);
    }

    updateCachedTextures();
}

void DemoText::Unload()
{
}

void DemoText::Draw() 
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();
    renderer2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    const auto   display_size = Engine::GetWindowEnvironment().DisplaySize;
    // Draw world-coordinate lines (fixed positions)
    const double line_width   = 16.0;
    const auto   into_screen  = line_width * 0.5;
    const auto   rect_size    = display_size - Math::vec2{ into_screen, into_screen };
    // const auto lineColor = CS200::BLACK;
    renderer2d.DrawRectangle(Math::TranslationMatrix(rect_size * 0.5) * Math::ScaleMatrix(rect_size), 0x2E3440FF, CS200::BLACK, line_width);
    // renderer2d.DrawLine({ 0.0, into_screen }, { display_size.x, into_screen }, lineColor, line_width);
    // renderer2d.DrawLine({ into_screen, 0.0 }, { into_screen, display_size.y }, lineColor, line_width);
    // renderer2d.DrawLine({ 0.0, display_size.y - into_screen }, { display_size.x, display_size.y - into_screen }, lineColor, line_width);
    // renderer2d.DrawLine({ display_size.x - into_screen, 0.0 }, { display_size.x - into_screen, display_size.y }, lineColor, line_width);

    constexpr double BOTTOM_START = 10.0;

    // Calculate center X position based on the widest text
    double center_x  = LEFT_MARGIN;
    double max_width = 0.0;

    // Find the widest text among all displayed texts
    if (settings.ShowSimpleFont)
    {
        if (auto texture = simpleFont->PrintToTexture(frequentText, 0x00FFFFFF))
            max_width = std::max(max_width, static_cast<double>(texture->GetSize().x) * settings.TextScale);
        if (auto texture = simpleFont->PrintToTexture(occasionalText, 0xFF00FFFF))
            max_width = std::max(max_width, static_cast<double>(texture->GetSize().x) * settings.TextScale);
        if (auto texture = simpleFont->PrintToTexture(staticText, settings.TextColor))
            max_width = std::max(max_width, static_cast<double>(texture->GetSize().x) * settings.TextScale);
    }

    if (settings.ShowOutlinedFont)
    {
        if (auto texture = outlinedFont->PrintToTexture(frequentText, 0xFF8000FF))
            max_width = std::max(max_width, static_cast<double>(texture->GetSize().x) * settings.TextScale);
        if (auto texture = outlinedFont->PrintToTexture(occasionalText, 0xFFFF00FF))
            max_width = std::max(max_width, static_cast<double>(texture->GetSize().x) * settings.TextScale);
        if (auto texture = outlinedFont->PrintToTexture(staticText, settings.TextColor))
            max_width = std::max(max_width, static_cast<double>(texture->GetSize().x) * settings.TextScale);
    }

    // Calculate the starting X position for center alignment
    const auto window_size = Engine::GetWindow().GetSize();
    center_x               = (window_size.x - max_width) / 2.0;

    double current_y = BOTTOM_START;

    if (settings.ShowSimpleFont)
    {
        drawText(frequentText, Math::vec2{ center_x, current_y }, *simpleFont, 0x00FFFFFF);
        current_y += LINE_HEIGHT;

        drawText(occasionalText, Math::vec2{ center_x, current_y }, *simpleFont, 0xFF00FFFF);
        current_y += LINE_HEIGHT;

        drawText(staticText, Math::vec2{ center_x, current_y }, *simpleFont, settings.TextColor);
        current_y += LINE_HEIGHT;
    }

    if (settings.ShowOutlinedFont)
    {
        drawText(frequentText, Math::vec2{ center_x, current_y }, *outlinedFont, 0xFF8000FF);
        current_y += LINE_HEIGHT;

        drawText(occasionalText, Math::vec2{ center_x, current_y }, *outlinedFont, 0xFFFF00FF);
        current_y += LINE_HEIGHT;

        drawText(staticText, Math::vec2{ center_x, current_y }, *outlinedFont, settings.TextColor);
        current_y += LINE_HEIGHT;
    }

    if (settings.ShowCacheAddresses)
    {
        drawCacheInfo(current_y, center_x);
    }

    renderer2d.EndScene();
}

void DemoText::DrawImGui()
{
    if (ImGui::Begin("Demo Controls"))
    {
        ImGui::Checkbox("Show Simple Font", &settings.ShowSimpleFont);
        ImGui::Checkbox("Show Outlined Font", &settings.ShowOutlinedFont);
        ImGui::Checkbox("Show Cache Addresses", &settings.ShowCacheAddresses);

        ImGui::SeparatorText("Text Appearance");

        float color[4] = { static_cast<float>((settings.TextColor >> 24) & 0xFF) / 255.0f, static_cast<float>((settings.TextColor >> 16) & 0xFF) / 255.0f,
                           static_cast<float>((settings.TextColor >> 8) & 0xFF) / 255.0f, static_cast<float>(settings.TextColor & 0xFF) / 255.0f };
        if (ImGui::ColorEdit4("Text Color", color))
        {
            settings.TextColor =
                (static_cast<uint32_t>(color[0] * 255) << 24) | (static_cast<uint32_t>(color[1] * 255) << 16) | (static_cast<uint32_t>(color[2] * 255) << 8) | static_cast<uint32_t>(color[3] * 255);
        }

        float text_scale = static_cast<float>(settings.TextScale);
        ImGui::SliderFloat("Text Scale", &text_scale, 0.5f, 3.0f);
        settings.TextScale = static_cast<double>(text_scale);

        ImGui::SeparatorText("Update Intervals");

        float intervals[] = { static_cast<float>(settings.OccasionalUpdateInterval), static_cast<float>(settings.FrequentUpdateInterval) };
        ImGui::SliderFloat("Occasional Update Interval", intervals, 0.5f, 5.0f);
        ImGui::SliderFloat("Frequent Update Interval", intervals + 1, 0.1f, 2.0f);
        settings.OccasionalUpdateInterval = static_cast<double>(intervals[0]);
        settings.FrequentUpdateInterval   = static_cast<double>(intervals[1]);

        ImGui::SeparatorText("Current Text Content");
        ImGui::TextWrapped("Static: %s", staticText.c_str());
        ImGui::TextWrapped("Occasional: %s", occasionalText.c_str());
        ImGui::TextWrapped("Frequent: %s", frequentText.c_str());

        ImGui::SeparatorText("Cache Information");
        ImGui::Text("Static Text Address: %p", staticTextAddress);
        ImGui::Text("Occasional Text Address: %p", occasionalTextAddress);
        ImGui::Text("Frequent Text Address: %p", frequentTextAddress);
        ImGui::Text("Blinking Header Address: %p", blinkingHeaderAddress);

        ImGui::SeparatorText("Cache Analysis");

        if (staticTextAddress == lastStaticAddress && lastStaticAddress != nullptr)
        {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Static text is cached (same address)");
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Static text address changed");
            lastStaticAddress = staticTextAddress;
        }

        if (blinkingHeaderAddress == lastBlinkingAddress && lastBlinkingAddress != nullptr)
        {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Blinking header is cached (same address)");
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Blinking header address changed");
            lastBlinkingAddress = blinkingHeaderAddress;
        }

        ImGui::SeparatorText("Switch Demo");
        if (ImGui::Button("Switch to Demo Framebuffer"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoFramebuffer>();
        }
        if (ImGui::Button("Switch to Demo Shapes"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoShapes>();
        }

        if (ImGui::Button("Switch to MainMenu"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<MainMenu>();
        }
    }

    ImGui::End();
}

gsl::czstring DemoText::GetName() const
{
    return "Text Demo";
}

void DemoText::updateCachedTextures()
{
    if (simpleFont)
    {
        staticTextTexture = simpleFont->PrintToTexture(staticText, settings.TextColor);
        staticTextAddress = staticTextTexture.get();

        occasionalTextTexture = simpleFont->PrintToTexture(occasionalText, 0xFF00FFFF);
        occasionalTextAddress = occasionalTextTexture.get();

        frequentTextTexture = simpleFont->PrintToTexture(frequentText, 0x00FFFFFF);
        frequentTextAddress = frequentTextTexture.get();
    }
}

void DemoText::drawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color) const
{
    if (auto text_texture = font.PrintToTexture(text, color); text_texture)
    {
        const auto transform = Math::TranslationMatrix(position) * Math::ScaleMatrix(Math::vec2{ settings.TextScale, settings.TextScale });
        text_texture->Draw(transform, color);
    }
}

void DemoText::drawCacheInfo(double start_y, double x_offset) const
{
    if (!simpleFont)
        return;

    double current_y = start_y;

    std::stringstream frequent_info;
    frequent_info << "Frequent: " << std::hex << frequentTextAddress;
    drawText(frequent_info.str(), Math::vec2{ x_offset, current_y }, *outlinedFont, CS200::WHITE);
    current_y += LINE_HEIGHT;

    std::stringstream occasional_info;
    occasional_info << "Occasional: " << std::hex << occasionalTextAddress;
    drawText(occasional_info.str(), Math::vec2{ x_offset, current_y }, *outlinedFont, CS200::WHITE);
    current_y += LINE_HEIGHT;

    std::stringstream static_info;
    static_info << "Static: " << std::hex << staticTextAddress;
    drawText(static_info.str(), Math::vec2{ x_offset, current_y }, *outlinedFont, CS200::WHITE);
    current_y += LINE_HEIGHT;

    std::stringstream blinking_info;
    blinking_info << "Blinking: " << std::hex << blinkingHeaderAddress;
    drawText(blinking_info.str(), Math::vec2{ x_offset, current_y }, *outlinedFont, 0xAAAAAAFF);
    current_y += LINE_HEIGHT;

    const uint64_t current_frame = Engine::GetWindowEnvironment().FrameCount;
    const uint64_t cycle_frame   = current_frame & (BLINKING_CYCLE_FRAMES - 1);

    if (const bool should_show_blinking_header = cycle_frame >= VISIBLE_START_FRAME; should_show_blinking_header)
    {
        auto temp_blinking_texture = outlinedFont->PrintToTexture("Cache Demo - Memory Addresses:", CS200::WHITE);
        blinkingHeaderAddress      = temp_blinking_texture.get();
        const auto transform       = Math::TranslationMatrix(Math::vec2{ x_offset, current_y }) * Math::ScaleMatrix(Math::vec2{ settings.TextScale, settings.TextScale });
        temp_blinking_texture->Draw(transform, CS200::WHITE);
    }
}
