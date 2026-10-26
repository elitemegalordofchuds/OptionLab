#include "../include/Option.hpp"
#include <cmath>
#include "../include/RNG.hpp"
#include "../../include/Result.hpp"

void Result::Euro::MonteCarlo_eu(const Option& option) {
	double S0 = option.spot();
	double r = option.inter();
	double sigma = option.volat();
	double T = option.time();
	double K = option.strike();
	auto  type = option.type();
	Rand RNG{};
	double St{};
	double sum{};
	int paths{ 100000 };
	double invar1 = S0 * std::exp((r - 0.5 * sigma * sigma * T);
	double invar2 = std::exp(sigma * std::sqrt(T));

	if (type == OptionType::Put) {
		for (int i{}; i < paths / 2; ++i) {
			double Z = RNG.getZ();
			St = invar1 * std::exp(invar2 * Z);
			sum += std::max(K - St, 0);
			St = invar1 * std::exp(invar2 * -Z);
			sum += std::max(K - St, 0);
		}
	}
	else {
		for (int i{}; i < paths / 2; ++i) {
			double Z = RNG.getZ();
			St = invar1 * std::exp(invar2 * Z);
			sum += std::max(St - K, 0);
			St = invar1 * std::exp(invar2 * -Z);
			sum += std::max(St - K, 0);
		}
	}
	price_monteC = std::exp(-r * T) * (sum / paths);
}