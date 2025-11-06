#pragma once
#include "../Engine/Particle.h"

namespace Particles
{
    class Smoke : public CS230::Particle
    {
    public:
        Smoke() : Particle("Assets/Smoke.spt") { };

        std::string TypeName() override
        {
            return "Smoke Particle";
        }

        static constexpr int    MaxCount = 3;
        static constexpr double MaxLife  = 5.0;
    };

    class Hit : public CS230::Particle
    {
    public:
        Hit() : Particle("Assets/Hit.spt") { };

        std::string TypeName() override
        {
            return "Hit Particle";
        }

        static constexpr int    MaxCount = 10;
        static constexpr double MaxLife  = 1.0;
    };

    class MeteorBit : public CS230::Particle
    {
    public:
        MeteorBit() : Particle("Assets/MeteorBit.spt") { };

        std::string TypeName() override
        {
            return "MeteorBit Particle";
        }

        static constexpr int    MaxCount = 150;
        static constexpr double MaxLife  = 1.25;
    };

    class Tears : public CS230::Particle
    {
    public:
        Tears() : Particle("Assets/sprites/CS230_Final/Tears.spt") { };

        std::string TypeName() override
        {
            return "Tears Particle";
        }

        static constexpr int    MaxCount = 30;
        static constexpr double MaxLife  = 3.0;
    };

    class Shining : public CS230::Particle
    {
    public:
        Shining() : Particle("Assets/sprites/CS230_Final/Shining.spt") { };

        std::string TypeName() override
        {
            return "Shining Particle";
        }

        static constexpr int    MaxCount = 50;
        static constexpr double MaxLife  = 3.0;
    };
}
