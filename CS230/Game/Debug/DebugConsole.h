#pragma once
#include "../../Engine/Matrix.h"
#include "../../Engine/Vec2.h"
#include "../../Engine/Component.h"

#include <string>
#include <vector>
#include <map>
#include <functional>

enum class ConsoleColor {
    White,
    Green,
    Red,
    Yellow,
    Blue
};

class DebugConsole : public CS230::Component {
public:
    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix);

    void ToggleConsole();
    bool IsOpen() const;
};