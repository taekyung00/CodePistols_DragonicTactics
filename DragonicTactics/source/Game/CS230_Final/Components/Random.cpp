#include "Random.h"
#include "../../../Engine/Logger.hpp"
int Random::PickRandomIndex(size_t amount, bool same_probability, const std::vector<double>& probability_array)
{
	if (same_probability == true) {
		double uniform_weight = 100.0 / static_cast<double>(amount);
		std::vector<double> weight;
		for (size_t i = 0; i < amount; ++i) {
			weight.push_back(uniform_weight);
		}
		std::discrete_distribution<> dist(weight.begin(), weight.end());
		return dist(gen);

	}
	else {
		if (amount <= 0) {
			Engine::GetLogger().LogDebug("Invalid amount: must be greater than 0.");
			return -1;
		}
		if (probability_array.size() != amount) {
			Engine::GetLogger().LogDebug("Random::PickRandomIndex - Mismatched size: expected "
				+ std::to_string(amount) + ", got " + std::to_string(probability_array.size()));
			return -1;
		}
		std::discrete_distribution<> dist(probability_array.begin(), probability_array.end());
		return dist(gen);
	}
}