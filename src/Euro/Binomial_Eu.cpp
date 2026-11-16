#include "../include/Result.hpp"
#include <vector>
#include <cmath>


std::pair<double, double> Result::binomial_Eu(const Option& option, int steps) {
	auto start = std::chrono::high_resolution_clock::now();
	const double S0 = option.spot();
	const double r = option.inter();
	const double sigma = option.volat();
	const double T = option.time();
	const double K = option.strike();
	const auto  type = option.type();

	double M = steps;
	double t = T / M;

	double beta = (std::exp(-r * t) + std::exp((r + sigma * sigma) * t)) / 2;
	double u = beta + std::sqrt(beta * beta - 1);
	double d = beta - std::sqrt(beta * beta - 1);
	double p = (std::exp(r * t) - d) / (u - d);

	if (T == 0.0) {
		double payoff = (type == OptionType::Call)
			? std::max(S0 - K, 0.0)
			: std::max(K - S0, 0.0);

		auto end = std::chrono::high_resolution_clock::now();

		return {
			payoff,
			std::chrono::duration_cast<std::chrono::microseconds>(
				end - start
			).count()
		};
	}
	
	std::vector<double> maturity(M + 1);
	if (type == OptionType::Call) {
		for (int i{}; i < M + 1; ++i) {
			maturity[i] = std::max(S0 * std::pow(u, i) * std::pow(d, M - i) - K, 0.0);
		}
		
	}

	else {
		for (int i{}; i < M + 1; ++i) {
			maturity[i] = std::max(K - S0 * std::pow(u, i) * std::pow(d, M - i), 0.0);
		}
	}

	int max = M;
	for (int i{}; i < M; ++i) {
		for (int j{}; j < max; ++j) {
			maturity[j] = std::exp(-r * t) * ((1 - p) * maturity[j] + maturity[j + 1] * p);
		}
		max--;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return { maturity[0], std::chrono::duration_cast<std::chrono::microseconds> (end - start).count()};
}