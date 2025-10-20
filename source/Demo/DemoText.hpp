/**
 * \file
 * \author Generated
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/Font.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Vec2.hpp"
#include <gsl/gsl>
#include <memory>

class DemoText : public CS230::GameState
{
public:
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw()  override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    std::unique_ptr<CS230::Font> simpleFont;
    std::unique_ptr<CS230::Font> outlinedFont;

    double lastOccasionalTextUpdate = 0.0;
    double lastFrequentTextUpdate   = 0.0;

    std::string staticText     = "This text never changes";
    std::string occasionalText = "Sometimes changing: 0";
    std::string frequentText   = "Frequent: 0";

    int occasionalCounter = 0;
    int frequentCounter   = 0;

    std::shared_ptr<CS230::Texture> staticTextTexture;
    std::shared_ptr<CS230::Texture> occasionalTextTexture;
    std::shared_ptr<CS230::Texture> frequentTextTexture;

    void*         staticTextAddress     = nullptr;
    void*         occasionalTextAddress = nullptr;
    void*         frequentTextAddress   = nullptr;
    mutable void* blinkingHeaderAddress = nullptr;

    void* lastStaticAddress   = nullptr;
    void* lastBlinkingAddress = nullptr;

    struct Settings
    {
        bool        ShowSimpleFont           = true;
        bool        ShowOutlinedFont         = true;
        bool        ShowCacheAddresses       = true;
        CS200::RGBA TextColor                = CS200::WHITE;
        double      TextScale                = 1.0;
        double      OccasionalUpdateInterval = 2.0;
        double      FrequentUpdateInterval   = 0.5;
    } settings;

    static constexpr double   LEFT_MARGIN           = 50.0;
    static constexpr double   LINE_HEIGHT           = 64.0;
    static constexpr uint64_t BLINKING_CYCLE_FRAMES = 256;
    static constexpr uint64_t VISIBLE_START_FRAME   = 30;

private:
    void updateCachedTextures();
    void drawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color = CS200::WHITE) const;
    void drawCacheInfo(double start_y, double x_offset) const;
};
