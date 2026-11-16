#include <cmath>
#include <tuple>
#include <numbers>
#include "../../include/Result.hpp"

void Result::defaultPricer_Eu(const Option& option) {
	const double S0 = option.spot();
	const double r = option.inter();
	const double sigma = option.volat();
	const double T = option.time();
	const double K = option.strike();
	const auto  type = option.type();
	const int steps = option.steps();

	double d1 = (std::log(S0 / K) + (r + std::pow(sigma, 2) / 2) * T) /
								(sigma * std::sqrt(T));

	double d2 = d1 - sigma * std::sqrt(T);

	double N1{};
	double N2{};
	double n1{};
	double n2{};

	if (T == 0.0) {
		price_default = (type == OptionType::Call)
			? std::max(S0 - K, 0.0)
			: std::max(K - S0, 0.0);

		vega = 0.0;
		rho = 0.0;

		if (S0 == K) {
			delta = gamma = theta =
				std::numeric_limits<double>::quiet_NaN();
		}
		else {
			delta = (type == OptionType::Call)
				? (S0 > K ? 1.0 : 0.0)
				: (S0 < K ? -1.0 : 0.0);

			gamma = 0.0;
			theta = 0.0;
		}

		return;
	}

	if (type == OptionType::Call) {
		N1 = 0.5 + 0.5 * std::erf(d1 / std::sqrt(2));
		N2 = 0.5 + 0.5 * std::erf(d2 / std::sqrt(2));
		n1 = (1 / std::sqrt(std::numbers::pi * 2)) * std::exp(-std::pow(d1, 2) / 2);
		n2 = (1 / std::sqrt(std::numbers::pi * 2)) * std::exp(-std::pow(d2, 2) / 2);
		price_default = S0 * N1 - K * std::exp(-r * T) * N2;
		delta = N1;
		gamma = n1 / (S0 * sigma * std::sqrt(T));
		rho = K * T * std::exp(-r * T) * N2;
		vega = S0 * n1 * std::sqrt(T);
		theta = (-S0 * n1 * sigma) / (2 * std::sqrt(T)) - r * K * std::exp(-r * T) * N2;
	}
	else {
		N1 = 0.5 + 0.5 * std::erf(-d1 / std::sqrt(2));
		N2 = 0.5 + 0.5 * std::erf(-d2 / std::sqrt(2));
		n1 = (1 / std::sqrt(std::numbers::pi * 2)) * std::exp(-std::pow(-d1, 2) / 2);
		n2 = (1 / std::sqrt(std::numbers::pi * 2)) * std::exp(-std::pow(-d2, 2) / 2);
		price_default = K * std::exp(-r * T) * N2 - S0 * N1;
		delta = -N1;
		gamma = n1 / (S0 * sigma * std::sqrt(T));
		rho = -K * T * std::exp(-r * T) * N2;
		vega = S0 * n1 * std::sqrt(T);
		theta = (-S0 * n1 * sigma) / (2 * std::sqrt(T)) + r * K * std::exp(-r * T) * N2;
	}
}