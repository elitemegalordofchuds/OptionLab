#include "../include/Option.hpp"
#include <cmath>
#include "../include/RNG.hpp"
#include "../../include/Result.hpp"
#include <tuple>

void Result<OptionStyle::Asian>::MonteCarlo_Asian(const Option& option) {
	double S0 = option.spot();
	double r = option.inter();
	double sigma = option.volat();
	double T = option.time();
	double K = option.strike();
	auto  type = option.type();
	Rand RNG{};
	int paths{ 100000 };
	int steps = option.steps();

	std::pair<double, double> St_base{S0, S0};
	std::pair<double, double> invar_base = {std::exp(r - 0.5 * sigma * sigma * T / steps), std::exp(sigma * std::sqrt(T / steps)};
	std::pair<double, double> sum_base{0, 0};
	double sumpaths_base{0};

	double bumped_S0 = 0.0001 * S0 + S0;
	std::pair<double, double> St_bumped_S0{bumped_S0, bumped_S0};
	std::pair<double, double> invar_bumped_S0 = invar_base;
	std::pair<double, double> sum_bumped_S0{0, 0};
	double sumpaths_bumped_S0{0};

	double bumped_down_S0 = -0.0001 * S0 + S0;
	std::pair<double, double> St_bumped_down_S0{bumped_down_S0, bumped_down_S0};
	std::pair<double, double> invar_bumped_down_S0 = invar_base;
	std::pair<double, double> sum_bumped_down_S0{0, 0};
	double sumpaths_bumped_down_S0{0};

	double bumped_sigma = 0.0001 * sigma + sigma;
	std::pair<double, double> St_bumped_sigma{S0, S0};
	std::pair<double, double> invar_bumped_sigma = {std::exp((r - 0.5 * (bumped_sigma) * (bumped_sigma)) * T / steps), std::exp((bumped_sigma) * std::sqrt(T / steps))};
	std::pair<double, double> sum_bumped_sigma{0, 0};
	double sumpaths_bumped_sigma{0};

	double bumped_r = 0.0001 * r + r;
	std::pair<double, double> St_bumped_r{S0, S0};
	std::pair<double, double> invar_bumped_r = {std::exp((bumped_r - 0.5 * sigma * sigma) * T / steps), std::exp(sigma * std::sqrt(T / steps))};
	std::pair<double, double> sum_bumped_r{0, 0};
	double sumpaths_bumped_r{0};

	double bumped_time = T - 0.0001 * T;
	std::pair<double, double> St_bumped_time{ S0, S0 };
	std::pair<double, double> invar_bumped_time = {std::exp((r - 0.5 * sigma * sigma) * bumped_time / steps), std::exp(sigma * std::sqrt(bumped_time / steps))};
	std::pair<double, double> sum_bumped_time{0, 0};
	double sumpaths_bumped_time{0};

	if (type == OptionType::Put) {
		for (int i{}; i < paths / 2; ++i) {
			for (int j{}; j < steps; ++j) {
				double Z = RNG.getZ();
				St_base.first = invar_base.first * std::exp(invar_base.second * Z);
				sum_base.first += St_base.first;
				St_base.second = invar_base.first * std::exp(invar_base.second * -Z);
				sum_base.second += St_base.second;

				St_bumped_S0.first = invar_bumped_S0.first * std::exp(invar_bumped_S0.second * Z);
				sum_bumped_S0.first += St_bumped_S0.first;
				St_bumped_S0.second = invar_bumped_S0.first * std::exp(invar_bumped_S0.second * -Z);
				sum_bumped_S0.second += St_bumped_S0.second;

				St_bumped_sigma.first = invar_bumped_sigma.first * std::exp(invar_bumped_sigma.second * Z);
				sum_bumped_sigma.first += St_bumped_sigma.first;
				St_bumped_sigma.second = invar_bumped_sigma.first * std::exp(invar_bumped_sigma.second * -Z);
				sum_bumped_sigma.second += St_bumped_sigma.second;

				St_bumped_r.first = invar_bumped_r.first * std::exp(invar_bumped_r.second * Z);
				sum_bumped_r.first += St_bumped_r.first;
				St_bumped_r.second = invar_bumped_r.first * std::exp(invar_bumped_r.second * -Z);
				sum_bumped_r.second += St_bumped_r.second;

				St_bumped_down_S0.first = invar_bumped_down_S0.first * std::exp(invar_bumped_down_S0.second * Z);
				sum_bumped_down_S0.first += St_bumped_down_S0.first;
				St_bumped_down_S0.second = invar_bumped_down_S0.first * std::exp(invar_bumped_down_S0.second * -Z);
				sum_bumped_down_S0.second += St_bumped_down_S0.second;

				St_bumped_time.first = invar_bumped_time.first * std::exp(invar_bumped_time.second * Z);
				sum_bumped_time.first += St_bumped_time.first;
				St_bumped_time.second = invar_bumped_time.first * std::exp(invar_bumped_time.second * -Z);
				sum_bumped_time.second += St_bumped_time.second;
			}
			sumpaths_base += std::max(K - sum_base.first / steps, 0) + std::max(K - sum_base.second / steps, 0);
			sumpaths_bumped_S0 += std::max(K - sum_bumped_S0.first / steps, 0) + std::max(K - sum_bumped_S0.second / steps, 0);
			sumpaths_bumped_sigma += std::max(K - sum_bumped_sigma.first / steps, 0) + std::max(K - sum_bumped_sigma.second / steps, 0);
			sumpaths_bumped_r += std::max(K - sum_bumped_r.first / steps, 0) + std::max(K - sum_bumped_r.second / steps, 0);
			sumpaths_bumped_down_S0 += std::max(K - sum_bumped_down_S0.first / steps, 0) + std::max(K - sum_bumped_down_S0.second / steps, 0);
			sumpaths_bumped_time += std::max(K - sum_bumped_time.first / steps, 0) + std::max(K - sum_bumped_time.second / steps, 0);
		}
	}
	else {
	for (int i{}; i < paths; ++i) {
		for (int j{}; j < steps; ++j) {
			double Z = RNG.getZ();
			St_base.first = invar_base.first * std::exp(invar_base.second * Z);
			sum_base.first += St_base.first;
			St_base.second = invar_base.first * std::exp(invar_base.second * -Z);
			sum_base.second += St_base.second;

			St_bumped_S0.first = invar_bumped_S0.first * std::exp(invar_bumped_S0.second * Z);
			sum_bumped_S0.first += St_bumped_S0.first;
			St_bumped_S0.second = invar_bumped_S0.first * std::exp(invar_bumped_S0.second * -Z);
			sum_bumped_S0.second += St_bumped_S0.second;

			St_bumped_sigma.first = invar_bumped_sigma.first * std::exp(invar_bumped_sigma.second * Z);
			sum_bumped_sigma.first += St_bumped_sigma.first;
			St_bumped_sigma.second = invar_bumped_sigma.first * std::exp(invar_bumped_sigma.second * -Z);
			sum_bumped_sigma.second += St_bumped_sigma.second;

			St_bumped_r.first = invar_bumped_r.first * std::exp(invar_bumped_r.second * Z);
			sum_bumped_r.first += St_bumped_r.first;
			St_bumped_r.second = invar_bumped_r.first * std::exp(invar_bumped_r.second * -Z);
			sum_bumped_r.second += St_bumped_r.second;

			St_bumped_down_S0.first = invar_bumped_down_S0.first * std::exp(invar_bumped_down_S0.second * Z);
			sum_bumped_down_S0.first += St_bumped_down_S0.first;
			St_bumped_down_S0.second = invar_bumped_down_S0.first * std::exp(invar_bumped_down_S0.second * -Z);
			sum_bumped_down_S0.second += St_bumped_down_S0.second;

			St_bumped_time.first = invar_bumped_time.first * std::exp(invar_bumped_time.second * Z);
			sum_bumped_time.first += St_bumped_time.first;
			St_bumped_time.second = invar_bumped_time.first * std::exp(invar_bumped_time.second * -Z);
			sum_bumped_time.second += St_bumped_time.second;
		}
		sumpaths_base += std::max(sum_base.first / steps - K, 0) + std::max(sum_base.second / steps - K, 0);
		sumpaths_bumped_S0 += std::max(sum_bumped_S0.first / steps - K, 0) + std::max(sum_bumped_S0.second / steps - K, 0);
		sumpaths_bumped_sigma += std::max(sum_bumped_sigma.first / steps - K, 0) + std::max(sum_bumped_sigma.second / steps - K, 0);
		sumpaths_bumped_r += std::max(sum_bumped_r.first / steps - K, 0) + std::max(sum_bumped_r.second / steps - K, 0);
		sumpaths_bumped_down_S0 += std::max(sum_bumped_down_S0.first / steps - K, 0) + std::max(sum_bumped_down_S0.second / steps - K, 0);
		sumpaths_bumped_time += std::max(sum_bumped_time.first / steps - K, 0) + std::max(sum_bumped_time.second / steps - K, 0);
	}
	price_monteC = std::exp(-r * T) * (sumpaths_base / paths);
	double price_bumped_S0 = std::exp(-r * T) * (sumpaths_bumped_S0 / paths);
	double price_bumped_r = std::exp(-bumped_r * T) * (sumpaths_bumped_r / paths);
	double price_bumped_sigma = std::exp(-r * T) * (sumpaths_bumped_sigma / paths);
	double price_bumped_down_S0 = std::exp(-r * T) * (sumpaths_bumped_down_S0 / paths);
	double price_bumped_time = std::exp(-r * bumped_time) * (sumpaths_bumped_time / paths);

	delta = (price_bumped_S0 - price_monteC) / (bumped_S0 - S0);
	rho = (price_bumped_r - price_monteC) / (bumped_r - r);
	vega = (price_bumped_sigma - price_monteC) / (bumped_sigma - sigma);
	gamma = (price_bumped_down_S0 - 2 * price_monteC + price_bumped_S0) / ((bumped_S0 - S0)*(bumped_S0 - S0));
	theta = (price_bumped_time - price_monteC) / (T - bumped_time);
	}
}