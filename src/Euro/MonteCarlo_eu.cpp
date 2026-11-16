#include "../include/Result.hpp"


std::tuple<double, double, double> Result::monteCarlo_Eu(const Option& option, int paths) {
	auto start = std::chrono::high_resolution_clock::now();
	const double S0 = option.spot();
	const double r = option.inter();
	const double sigma = option.volat();
	const double T = option.time();
	const double K = option.strike();
	const auto  type = option.type();
	double ST1{};
	double ST2{};
	double sumpaths{};
	double sumSqr{};
	const double invar1 = S0 * std::exp((r - 0.5 * sigma * sigma) * T);
	const double invar2 = sigma * std::sqrt(T);

	int threads = std::thread::hardware_concurrency();
	if (threads == 0) threads = 1;
	const int pairs = paths / 2;
	const int pairs_thread = pairs / threads;
	const int remainder = pairs % threads;
	std::vector<std::future<std::pair<double, double>>> futures;
	futures.reserve(threads);

	if (T == 0.0 || sigma == 0.0) {

		const double ST = S0 * std::exp(r * T);

		const double price = (type == OptionType::Call)
			? std::max(ST - K, 0.0) * std::exp(-r * T)
			: std::max(K - ST, 0.0) * std::exp(-r * T);

		auto end = std::chrono::high_resolution_clock::now();

		return { price, std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(), 0.0 };
	}

	for (int i{}; i < threads; ++i) {
		futures.push_back(std::async(std::launch::async,
			[=, &invar1, &invar2]() mutable -> std::pair<double, double> {
				Rand RNG{};
				if (type == OptionType::Put) {
					for (int j{}; j < pairs_thread + (i == 0 ? remainder : 0); ++j) {
						double Z = RNG.getZ();
						ST1 = invar1 * std::exp(invar2 * Z);
						sumpaths += std::max(K - ST1, 0.0) / 2;
						ST2 = invar1 * std::exp(invar2 * -Z);
						sumpaths += std::max(K - ST2, 0.0) / 2;

						sumSqr += ((std::max(K - ST1, 0.0) + std::max(K - ST2, 0.0)) / 2) * ((std::max(K - ST1, 0.0) + std::max(K - ST2, 0.0)) / 2);
					}
				}
				else {
					for (int j{}; j < pairs_thread + (i == 0 ? remainder : 0); ++j) {
						double Z = RNG.getZ();
						ST1 = invar1 * std::exp(invar2 * Z);
						sumpaths += std::max(ST1 - K, 0.0) / 2;
						ST2 = invar1 * std::exp(invar2 * -Z);
						sumpaths += std::max(ST2 - K, 0.0) / 2;

						sumSqr += ((std::max(ST1 - K, 0.0) + std::max(ST2 - K, 0.0)) / 2) * ((std::max(ST1 - K, 0.0) + std::max(ST2 - K, 0.0)) / 2);
					}
				}
				return { sumpaths , sumSqr };
			}));
	}

	for (auto& f : futures) {
		auto [first, second] = f.get();
		sumpaths += first;
		sumSqr += second;
	}
	const double price = sumpaths * std::exp(-r * T) / pairs;
	const double SE = std::exp(-r * T) * std::sqrt( (sumSqr - sumpaths * sumpaths / pairs) / (pairs * (pairs - 1)));
	auto end = std::chrono::high_resolution_clock::now();
	return { price, std::chrono::duration_cast<std::chrono::microseconds> (end - start).count(), SE };
}