#include "GridPosition.h"

GridPosition::GridPosition(Math::ivec2 start_coordinates)
    : coordinates(start_coordinates) {
}

void GridPosition::Set(Math::ivec2 new_coordinates) {
    coordinates = new_coordinates;
}

const Math::ivec2& GridPosition::Get() const {
    return coordinates;
}