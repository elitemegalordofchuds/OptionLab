#include <cmath>
#include "../../include/Result.hpp"
#include <tuple>
#include <future>

void Result::defaultPricer_Asian(const Option& option, int paths) {
	const double S0 = option.spot();
	const double r = option.inter();
	const double sigma = option.volat();
	const double T = option.time();
	const double K = option.strike();
	const auto  type = option.type();
	const int steps = option.steps();

	if (T == 0.0) {
		price_default = (type == OptionType::Call)
			? std::max(S0 - K, 0.0)
			: std::max(K - S0, 0.0);

		if (type == OptionType::Call)
			delta = (S0 > K) ? 1.0 : (S0 < K) ? 0.0 : 0.5;
		else
			delta = (S0 < K) ? -1.0 : (S0 > K) ? 0.0 : -0.5;

		gamma = 0.0;
		vega = 0.0;
		rho = 0.0;
		theta = 0.0;

		return;
	}

	std::pair<double, double> St_base{ S0, S0 };
	const std::pair<double, double> invar_base = { std::exp((r - 0.5 * sigma * sigma) * T / steps), sigma * std::sqrt(T / steps) };
	std::pair<double, double> sum_base{ 0, 0 };
	double sumpaths_base{ 0 };

	double bumped_S0 = 0.0001 * S0 + S0;
	std::pair<double, double> St_bumped_S0{ bumped_S0, bumped_S0 };
	const std::pair<double, double> invar_bumped_S0 = invar_base;
	std::pair<double, double> sum_bumped_S0{ 0, 0 };
	double sumpaths_bumped_S0{ 0 };

	double bumped_down_S0 = -0.0001 * S0 + S0;
	std::pair<double, double> St_bumped_down_S0{ bumped_down_S0, bumped_down_S0 };
	const std::pair<double, double> invar_bumped_down_S0 = invar_base;
	std::pair<double, double> sum_bumped_down_S0{ 0, 0 };
	double sumpaths_bumped_down_S0{ 0 };

	double bumped_sigma = 0.0001 * sigma + sigma;
	std::pair<double, double> St_bumped_sigma{ S0, S0 };
	const std::pair<double, double> invar_bumped_sigma = { std::exp((r - 0.5 * (bumped_sigma) * (bumped_sigma)) * T / steps), (bumped_sigma)*std::sqrt(T / steps) };
	std::pair<double, double> sum_bumped_sigma{ 0, 0 };
	double sumpaths_bumped_sigma{ 0 };

	double bumped_r = 0.0001 * r + r;
	std::pair<double, double> St_bumped_r{ S0, S0 };
	const std::pair<double, double> invar_bumped_r = { std::exp((bumped_r - 0.5 * sigma * sigma) * T / steps), sigma * std::sqrt(T / steps) };
	std::pair<double, double> sum_bumped_r{ 0, 0 };
	double sumpaths_bumped_r{ 0 };

	double bumped_time = T - 0.0001 * T;
	std::pair<double, double> St_bumped_time{ S0, S0 };
	const std::pair<double, double> invar_bumped_time = { std::exp((r - 0.5 * sigma * sigma) * bumped_time / steps), sigma * std::sqrt(bumped_time / steps) };
	std::pair<double, double> sum_bumped_time{ 0, 0 };
	double sumpaths_bumped_time{ 0 };

	const int pairs = paths / 2;
	int threads = std::min(static_cast<int> (std::thread::hardware_concurrency()), pairs);
	if (threads == 0) threads = 1;
	const int pairs_thread = pairs / threads;
	const int remainder = pairs % threads;
	std::vector<std::future<std::array<double, 6>>> futures;
	futures.reserve(threads);

	for (int i{}; i < threads; ++i) {
		futures.push_back(std::async(std::launch::async, 
			[=, &invar_base, &invar_bumped_S0, &invar_bumped_down_S0, &invar_bumped_sigma, &invar_bumped_r, &invar_bumped_time]() mutable
			-> std::array<double, 6>{
			Rand RNG{};
			if (type == OptionType::Put) {
				for (int j{}; j < pairs_thread + (j == 0 ? remainder : 0); ++j) {
					for (int k{}; k < steps; ++k) {
						double Z = RNG.getZ();
						St_base.first *= invar_base.first * std::exp(invar_base.second * Z);
						sum_base.first += St_base.first;
						St_base.second *= invar_base.first * std::exp(invar_base.second * -Z);
						sum_base.second += St_base.second;

						St_bumped_S0.first *= invar_bumped_S0.first * std::exp(invar_bumped_S0.second * Z);
						sum_bumped_S0.first += St_bumped_S0.first;
						St_bumped_S0.second *= invar_bumped_S0.first * std::exp(invar_bumped_S0.second * -Z);
						sum_bumped_S0.second += St_bumped_S0.second;

						St_bumped_sigma.first *= invar_bumped_sigma.first * std::exp(invar_bumped_sigma.second * Z);
						sum_bumped_sigma.first += St_bumped_sigma.first;
						St_bumped_sigma.second *= invar_bumped_sigma.first * std::exp(invar_bumped_sigma.second * -Z);
						sum_bumped_sigma.second += St_bumped_sigma.second;

						St_bumped_r.first *= invar_bumped_r.first * std::exp(invar_bumped_r.second * Z);
						sum_bumped_r.first += St_bumped_r.first;
						St_bumped_r.second *= invar_bumped_r.first * std::exp(invar_bumped_r.second * -Z);
						sum_bumped_r.second += St_bumped_r.second;

						St_bumped_down_S0.first *= invar_bumped_down_S0.first * std::exp(invar_bumped_down_S0.second * Z);
						sum_bumped_down_S0.first += St_bumped_down_S0.first;
						St_bumped_down_S0.second *= invar_bumped_down_S0.first * std::exp(invar_bumped_down_S0.second * -Z);
						sum_bumped_down_S0.second += St_bumped_down_S0.second;

						St_bumped_time.first *= invar_bumped_time.first * std::exp(invar_bumped_time.second * Z);
						sum_bumped_time.first += St_bumped_time.first;
						St_bumped_time.second *= invar_bumped_time.first * std::exp(invar_bumped_time.second * -Z);
						sum_bumped_time.second += St_bumped_time.second;
					}
					sumpaths_base += (std::max(K - sum_base.first / steps, 0.0) + std::max(K - sum_base.second / steps, 0.0)) / 2;
					sumpaths_bumped_S0 += (std::max(K - sum_bumped_S0.first / steps, 0.0) + std::max(K - sum_bumped_S0.second / steps, 0.0)) / 2;
					sumpaths_bumped_sigma += (std::max(K - sum_bumped_sigma.first / steps, 0.0) + std::max(K - sum_bumped_sigma.second / steps, 0.0)) / 2;
					sumpaths_bumped_r += (std::max(K - sum_bumped_r.first / steps, 0.0) + std::max(K - sum_bumped_r.second / steps, 0.0)) / 2;
					sumpaths_bumped_down_S0 += (std::max(K - sum_bumped_down_S0.first / steps, 0.0) + std::max(K - sum_bumped_down_S0.second / steps, 0.0)) / 2;
					sumpaths_bumped_time += (std::max(K - sum_bumped_time.first / steps, 0.0) + std::max(K - sum_bumped_time.second / steps, 0.0)) / 2;

					St_base = { S0, S0 };
					St_bumped_S0 = { bumped_S0, bumped_S0 };
					St_bumped_down_S0 = { bumped_down_S0, bumped_down_S0 };
					St_bumped_sigma = { S0, S0 };
					St_bumped_r = { S0, S0 };
					St_bumped_time = { S0, S0 };

					sum_base = { 0.0, 0.0 };
					sum_bumped_S0 = { 0.0, 0.0 };
					sum_bumped_down_S0 = { 0.0, 0.0 };
					sum_bumped_sigma = { 0.0, 0.0 };
					sum_bumped_r = { 0.0, 0.0 };
					sum_bumped_time = { 0.0, 0.0 };
				}
			}
			else {
				for (int j{}; j < pairs_thread + (j == 0 ? remainder : 0); ++j) {
					for (int k{}; k < steps; ++k) {
						double Z = RNG.getZ();
						St_base.first *= invar_base.first * std::exp(invar_base.second * Z);
						sum_base.first += St_base.first;
						St_base.second *= invar_base.first * std::exp(invar_base.second * -Z);
						sum_base.second += St_base.second;

						St_bumped_S0.first *= invar_bumped_S0.first * std::exp(invar_bumped_S0.second * Z);
						sum_bumped_S0.first += St_bumped_S0.first;
						St_bumped_S0.second *= invar_bumped_S0.first * std::exp(invar_bumped_S0.second * -Z);
						sum_bumped_S0.second += St_bumped_S0.second;

						St_bumped_sigma.first *= invar_bumped_sigma.first * std::exp(invar_bumped_sigma.second * Z);
						sum_bumped_sigma.first += St_bumped_sigma.first;
						St_bumped_sigma.second *= invar_bumped_sigma.first * std::exp(invar_bumped_sigma.second * -Z);
						sum_bumped_sigma.second += St_bumped_sigma.second;

						St_bumped_r.first *= invar_bumped_r.first * std::exp(invar_bumped_r.second * Z);
						sum_bumped_r.first += St_bumped_r.first;
						St_bumped_r.second *= invar_bumped_r.first * std::exp(invar_bumped_r.second * -Z);
						sum_bumped_r.second += St_bumped_r.second;

						St_bumped_down_S0.first *= invar_bumped_down_S0.first * std::exp(invar_bumped_down_S0.second * Z);
						sum_bumped_down_S0.first += St_bumped_down_S0.first;
						St_bumped_down_S0.second *= invar_bumped_down_S0.first * std::exp(invar_bumped_down_S0.second * -Z);
						sum_bumped_down_S0.second += St_bumped_down_S0.second;

						St_bumped_time.first *= invar_bumped_time.first * std::exp(invar_bumped_time.second * Z);
						sum_bumped_time.first += St_bumped_time.first;
						St_bumped_time.second *= invar_bumped_time.first * std::exp(invar_bumped_time.second * -Z);
						sum_bumped_time.second += St_bumped_time.second;
					}
					sumpaths_base += (std::max(sum_base.first / steps - K, 0.0) + std::max(sum_base.second / steps - K, 0.0)) / 2;
					sumpaths_bumped_S0 += (std::max(sum_bumped_S0.first / steps - K, 0.0) + std::max(sum_bumped_S0.second / steps - K, 0.0)) / 2;
					sumpaths_bumped_sigma += (std::max(sum_bumped_sigma.first / steps - K, 0.0) + std::max(sum_bumped_sigma.second / steps - K, 0.0)) / 2;
					sumpaths_bumped_r += (std::max(sum_bumped_r.first / steps - K, 0.0) + std::max(sum_bumped_r.second / steps - K, 0.0)) / 2;
					sumpaths_bumped_down_S0 += (std::max(sum_bumped_down_S0.first / steps - K, 0.0) + std::max(sum_bumped_down_S0.second / steps - K, 0.0)) / 2;
					sumpaths_bumped_time += (std::max(sum_bumped_time.first / steps - K, 0.0) + std::max(sum_bumped_time.second / steps - K, 0.0)) / 2;

					St_base = { S0, S0 };
					St_bumped_S0 = { bumped_S0, bumped_S0 };
					St_bumped_down_S0 = { bumped_down_S0, bumped_down_S0 };
					St_bumped_sigma = { S0, S0 };
					St_bumped_r = { S0, S0 };
					St_bumped_time = { S0, S0 };

					sum_base = { 0.0, 0.0 };
					sum_bumped_S0 = { 0.0, 0.0 };
					sum_bumped_down_S0 = { 0.0, 0.0 };
					sum_bumped_sigma = { 0.0, 0.0 };
					sum_bumped_r = { 0.0, 0.0 };
					sum_bumped_time = { 0.0, 0.0 };
				}
			}
			return { sumpaths_base, sumpaths_bumped_S0, sumpaths_bumped_sigma, sumpaths_bumped_r, sumpaths_bumped_down_S0, sumpaths_bumped_time };
			}));
	}
	for (auto& f : futures) {
		const auto& data = f.get();
		sumpaths_base += data[0];
		sumpaths_bumped_S0 += data[1];
		sumpaths_bumped_sigma += data[2];
		sumpaths_bumped_r += data[3];
		sumpaths_bumped_down_S0 += data[4];
		sumpaths_bumped_time += data[5];
	}
	price_default = std::exp(-r * T) * (sumpaths_base / pairs);
	double price_bumped_S0 = std::exp(-r * T) * (sumpaths_bumped_S0 / pairs);
	double price_bumped_r = std::exp(-bumped_r * T) * (sumpaths_bumped_r / pairs);
	double price_bumped_sigma = std::exp(-r * T) * (sumpaths_bumped_sigma / pairs);
	double price_bumped_down_S0 = std::exp(-r * T) * (sumpaths_bumped_down_S0 / pairs);
	double price_bumped_time = std::exp(-r * bumped_time) * (sumpaths_bumped_time / pairs);

	delta = (price_bumped_S0 - price_default) / (bumped_S0 - S0);
	rho = (price_bumped_r - price_default) / (bumped_r - r);
	vega = (price_bumped_sigma - price_default) / (bumped_sigma - sigma);
	gamma = (price_bumped_down_S0 - 2 * price_default + price_bumped_S0) / ((bumped_S0 - S0)*(bumped_S0 - S0));
	theta = (price_bumped_time - price_default) / (T - bumped_time);
}