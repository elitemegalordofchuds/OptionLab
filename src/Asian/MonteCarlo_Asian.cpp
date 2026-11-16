#include <cmath>
#include "../../include/Result.hpp"
#include <future>


std::tuple<double, double, double> Result::monteCarlo_Asian(const Option& option, int paths) {
	auto start = std::chrono::high_resolution_clock::now();
	const double S0 = option.spot();
	const double r = option.inter();
	const double sigma = option.volat();
	const double T = option.time();
	const double K = option.strike();
	const auto  type = option.type();
	const int steps = option.steps();

	if (T == 0.0) {
		double price = (type == OptionType::Call)
			? std::max(S0 - K, 0.0)
			: std::max(K - S0, 0.0);

		auto end = std::chrono::high_resolution_clock::now();

		return { price, std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(), 0.0 };
	}

	double St1 = S0;
	double St2 = S0;
	const double invar1 = std::exp((r - 0.5 * sigma * sigma) * T / steps);
	const double invar2 = sigma * std::sqrt(T / steps);
	double sum1{};
	double sum2{};
	double sumpaths{};
	double sumSqr{};
	double total{};
	
	int threads = std::thread::hardware_concurrency();
	if (threads == 0) threads = 1;
	const int pairs = paths / 2;
	const int pairs_thread = pairs / threads;
	const int remainder = pairs % threads;
	std::vector<std::future<std::pair<double, double>>> futures;
	futures.reserve(threads);

	for (int i{}; i < threads; ++i) {
		futures.push_back(std::async(std::launch::async,
			[=, &invar1, &invar2]() mutable -> std::pair<double, double> {
				Rand RNG{};
				if (type == OptionType::Put) {
					for (int j{}; j < pairs_thread + (i == 0 ? remainder : 0); ++j) {
						for (int k{}; k < steps; ++k) {
							double Z = RNG.getZ();
							St1 *= invar1 * std::exp(invar2 * Z);
							sum1 += St1;
							St2 *= invar1 * std::exp(invar2 * -Z);
							sum2 += St2;
						}
						sumpaths += (std::max(K - sum1 / steps, 0.0) + std::max(K - sum2 / steps, 0.0)) / 2;
						sumSqr += (std::max(K - sum1 / steps, 0.0) + std::max(K - sum2 / steps, 0.0)) * (std::max(K - sum1 / steps, 0.0) + std::max(K - sum2 / steps, 0.0)) / 4;
						St1 = St2 = S0;
						sum1 = sum2 = 0;
					}
				}
				else {
					for (int j{}; j < pairs_thread + (i == 0 ? remainder : 0); ++j) {
						for (int k{}; k < steps; ++k) {
							double Z = RNG.getZ();
							St1 *= invar1 * std::exp(invar2 * Z);
							sum1 += St1;
							St2 *= invar1 * std::exp(invar2 * -Z);
							sum2 += St2;
						}
						sumpaths += (std::max(sum1 / steps - K, 0.0) + std::max(sum2 / steps - K, 0.0)) / 2;
						sumSqr += (std::max(sum1 / steps - K, 0.0) + std::max(sum2 / steps - K, 0.0)) * (std::max(sum1 / steps - K, 0.0) + std::max(sum2 / steps - K, 0.0)) / 4;
						St1 = St2 = S0;
						sum1 = sum2 = 0;
					}
				}
				return { sumpaths, sumSqr };
			}));
	}

	for (auto& f : futures) {
		auto [first, second] = f.get();
		sumpaths += first;
		sumSqr += second;
	}
	const double price = sumpaths * std::exp(-r * T) / pairs;
	const double SE = std::exp(-r * T) * std::sqrt((sumSqr - sumpaths * sumpaths / pairs) / (pairs * (pairs - 1)));
	auto end = std::chrono::high_resolution_clock::now();
	return { price, std::chrono::duration_cast<std::chrono::microseconds> (end - start).count(), SE };
}