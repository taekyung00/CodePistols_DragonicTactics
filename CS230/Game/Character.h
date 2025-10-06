#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/GameObjectManager.h"

class Character : public CS230::GameObject
{
  public:
    Character(Math::ivec2 coordinates, int type);
    ~Character();
    void Update(double dt) override;

  private:
};

Character::Character()
{
}

Character::~Character()
{
}
