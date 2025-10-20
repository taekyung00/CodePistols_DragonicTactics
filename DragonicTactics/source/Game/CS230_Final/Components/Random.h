#pragma once
#include <random>
#include <vector>

#include "../../../Engine/Engine.hpp"
#include "../../../Engine/Component.h"

class Random : public CS230::Component {
public:
	Random() :gen(rd()){}

	int PickRandomIndex(size_t amount, bool same_probability, const std::vector<double>& probability_array = {});

private:
	std::random_device rd;
	std::mt19937 gen;
};

