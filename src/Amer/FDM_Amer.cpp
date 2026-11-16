#include "../include/Option.hpp"
#include "../../include/Result.hpp"
#include <algorithm>

std::pair<double, double> Result::FDM_Amer(const Option& option, int steps) {
	auto start = std::chrono::high_resolution_clock::now();

	const double S0 = option.spot();
	const double r = option.inter();
	const double sigma = option.volat();
	const double T = option.time();
	const double K = option.strike();
	const auto type = option.type();

	if (T == 0.0) {
		double price = (type == OptionType::Call)
			? std::max(S0 - K, 0.0)
			: std::max(K - S0, 0.0);

		auto end = std::chrono::high_resolution_clock::now();
		return { price, std::chrono::duration<double, std::milli>(end - start).count() };
	}

	if (sigma == 0.0) {
		double price;

		if (type == OptionType::Call) {
			price = std::max({
				S0 - K,
				S0 - K * std::exp(-r * T),
				0.0
				});
		}
		else {
			price = std::max({
				K - S0,
				K * std::exp(-r * T) - S0,
				0.0
				});
		}

		auto end = std::chrono::high_resolution_clock::now();
		return { price, std::chrono::duration<double, std::milli>(end - start).count() };
	}

	if ((type == OptionType::Call && r >= 0.0)
		|| (type == OptionType::Put && r == 0.0)) {
		return FDM_Eu(option, steps);
	}

	int N = std::max(2, steps);
	int M = std::max(2, static_cast<int>(20.0 * std::sqrt(N)));

	double drift = r - 0.5 * sigma * sigma;
	double x0 = std::log(S0 / K);
	double width = 8.0 * sigma * std::sqrt(T);

	double xmin = std::min({ x0, 0.0, x0 + drift * T }) - width - 0.5;
	double xmax = std::max({ x0, 0.0, x0 + drift * T }) + width + 0.5;

	double dx = (xmax - xmin) / M;
	double dt = T / N;
	double Smax = K * std::exp(xmax);

	std::vector<double> V(M + 1);

	for (int i = 0; i <= M; ++i) {
		double S = K * std::exp(xmin + i * dx);

		if (type == OptionType::Call) {
			V[i] = std::max(S - K, 0.0);
		}
		else {
			V[i] = std::max(K - S, 0.0);
		}
	}

	int m = M - 1;

	std::vector<double> lower(m);
	std::vector<double> diagonal(m);
	std::vector<double> upper(m);
	std::vector<double> rhs(m);
	std::vector<double> solution(m);
	std::vector<double> payoff(m);

	double alpha = 0.25 * dt * (sigma * sigma / (dx * dx) - drift / dx);
	double beta = -0.5 * dt * (sigma * sigma / (dx * dx) + r);
	double gamma = 0.25 * dt * (sigma * sigma / (dx * dx) + drift / dx);

	for (int j = 0; j < m; j++) {
		int i = j + 1;
		double S = K * std::exp(xmin + i * dx);

		if (type == OptionType::Call) {
			payoff[j] = std::max(S - K, 0.0);
		}
		else {
			payoff[j] = std::max(K - S, 0.0);
		}

		lower[j] = -alpha;
		diagonal[j] = 1.0 - beta;
		upper[j] = -gamma;
	}

	const double omega = 1.5;
	const double tolerance = 1e-7;
	const int maxIterations = 2000;

	for (int n = 0; n < N; n++) {
		double tau = (n + 1) * dt;

		double boundaryLeft;
		double boundaryRight;

		if (type == OptionType::Call) {
			boundaryLeft = 0.0;
			boundaryRight = std::max(
				Smax - K,
				Smax - K * std::exp(-r * tau)
			);
		}
		else {
			boundaryLeft = K;
			boundaryRight = 0.0;
		}

		for (int j = 0; j < m; j++) {
			int i = j + 1;

			rhs[j] = alpha * V[i - 1]
				+ (1.0 + beta) * V[i]
				+ gamma * V[i + 1];

			if (j == 0) {
				rhs[j] += alpha * boundaryLeft;
			}

			if (j == m - 1) {
				rhs[j] += gamma * boundaryRight;
			}

			solution[j] = std::max(payoff[j], V[i]);
		}

		for (int iteration = 0; iteration < maxIterations; iteration++) {
			double maxChange = 0.0;

			for (int j = 0; j < m; j++) {
				double left = (j == 0) ? 0.0 : lower[j] * solution[j - 1];
				double right = (j == m - 1) ? 0.0 : upper[j] * solution[j + 1];

				double candidate = (rhs[j] - left - right) / diagonal[j];
				double relaxed = solution[j] + omega * (candidate - solution[j]);
				double updated = std::max(payoff[j], relaxed);

				maxChange = std::max(maxChange, std::abs(updated - solution[j]));
				solution[j] = updated;
			}

			if (maxChange < tolerance) {
				break;
			}
		}

		V[0] = boundaryLeft;
		V[M] = boundaryRight;

		for (int j = 0; j < m; j++) {
			V[j + 1] = solution[j];
		}
	}

	double position = (x0 - xmin) / dx;
	int index = static_cast<int>(position);
	double S_left = K * std::exp(xmin + index * dx);
	double S_right = K * std::exp(xmin + (index + 1) * dx);

	double weight = (S0 - S_left) / (S_right - S_left);
	double price = V[index] * (1.0 - weight) + V[index + 1] * weight;

	auto end = std::chrono::high_resolution_clock::now();

	return { price, std::chrono::duration<double, std::milli>(end - start).count() };
}
