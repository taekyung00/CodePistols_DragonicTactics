#include "ActionPoints.h"

ActionPoints::ActionPoints(int max_points)
    : max_points(max_points), current_points(max_points)
{
}

void ActionPoints::Refresh() {
    current_points = max_points;
}

bool ActionPoints::Consume(int amount) {
    if (HasEnough(amount)) {
        current_points -= amount;
        return true;
    }
    return false;
}

int ActionPoints::GetCurrentPoints() const {
    return current_points;
}

int ActionPoints::GetMaxPoints() const {
    return max_points;
}

bool ActionPoints::HasEnough(int amount) const {
    return current_points >= amount;
}