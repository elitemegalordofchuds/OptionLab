#include <cmath>
#include <array>
#include <tuple>
#include <chrono>
#include "../include/Result.hpp"


std::pair<double, double> Result::binomial_Amer(const Option& option, int steps) {
	auto start = std::chrono::high_resolution_clock::now();
	const double S0 = option.spot();
	const double r = option.inter();
	const double sigma = option.volat();
	const double T = option.time();
	const double K = option.strike();
	const auto  type = option.type();

	if (T == 0.0) {
		double price = (type == OptionType::Call)
			? std::max(S0 - K, 0.0)
			: std::max(K - S0, 0.0);

		auto end = std::chrono::high_resolution_clock::now();

		return { price, std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() };
	}

	double M = steps;
	double t = T / M;

	double beta = (std::exp(-r * t) + std::exp((r + sigma * sigma) * t)) / 2;
	double u = beta + std::sqrt(beta * beta - 1);
	double d = beta - std::sqrt(beta * beta - 1);
	double p = (std::exp(r * t) - d) / (u - d);

	int max = M;
	std::vector<double> currentV(M + 1);
	std::vector<double> currentS(M + 1);
	if (type == OptionType::Call) {
		for (int i{}; i < M + 1; ++i) {
			currentV[i] = std::max(S0 * std::pow(u, i) * std::pow(d, M - i) - K, 0.0);
			currentS[i] = S0 * std::pow(u, i) * std::pow(d, M - i);
		}
		for (int i{}; i < M; ++i) {
			for (int j{}; j < max; ++j) {
				currentS[j] = std::exp(-r * t) * ((1 - p) * currentS[j] + p * currentS[j + 1]);
				currentV[j] = std::max(std::exp(-r * t) * ((1 - p) * currentV[j] + p * currentV[j + 1]), currentS[j] - K);
			}
			max--;
		}
	}

	else {
		for (int i{}; i < M + 1; ++i) {
			currentV[i] = std::max(K - S0 * std::pow(u, i) * std::pow(d, M - i), 0.0);
			currentS[i] = S0 * std::pow(u, i) * std::pow(d, M - i);
		}
		for (int i{}; i < M; ++i) {
			for (int j{}; j < max; ++j) {
				currentS[j] = std::exp(-r * t) * ((1 - p) * currentS[j] + p * currentS[j + 1]);
				currentV[j] = std::max(std::exp(-r * t) * ((1 - p) * currentV[j] + p * currentV[j + 1]), K - currentS[j]);
			}
			max--;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return { currentV[0], std::chrono::duration<double, std::milli> (end - start).count()};
}