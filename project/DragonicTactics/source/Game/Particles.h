/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  MainMenu.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 6, 2025
*/
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

	static constexpr int	MaxCount = 3;
	static constexpr double MaxLife	 = 5.0;
  };

  class Hit : public CS230::Particle
  {
public:
	Hit() : Particle("Assets/Hit.spt") { };

	std::string TypeName() override
	{
	  return "Hit Particle";
	}

	// 피해 1건당 Emit(count=10) 고정. 광역기(Meteor 등)는 한 프레임에 여러 대상을
	// 동시에 때려 10*N개를 즉시 요구하므로 풀 10은 즉시 고갈됐다. MaxLife 1.0초 동안
	// 동시 다발 피해(최대 ~6건)를 커버하도록 64로 확대.
	static constexpr int	MaxCount = 64;
	static constexpr double MaxLife	 = 1.0;
  };

  class MeteorBit : public CS230::Particle
  {
public:
	MeteorBit() : Particle("Assets/MeteorBit.spt") { };

	std::string TypeName() override
	{
	  return "MeteorBit Particle";
	}

	static constexpr int	MaxCount = 150;
	static constexpr double MaxLife	 = 1.25;
  };

  class Tears : public CS230::Particle
  {
public:
	Tears() : Particle("Assets/sprites/CS230_Final/Tears.spt") { };

	std::string TypeName() override
	{
	  return "Tears Particle";
	}

	static constexpr int	MaxCount = 30;
	static constexpr double MaxLife	 = 3.0;
  };

  class Shining : public CS230::Particle
  {
public:
	Shining() : Particle("Assets/sprites/CS230_Final/Shining.spt") { };

	std::string TypeName() override
	{
	  return "Shining Particle";
	}

	static constexpr int	MaxCount = 50;
	static constexpr double MaxLife	 = 3.0;
  };
}
