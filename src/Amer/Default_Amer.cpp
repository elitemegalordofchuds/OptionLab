#include "../include/Result.hpp"
#include <tuple>
#include <vector>

void Result::defaultPricer_Amer(const Option& option, int steps) {
	const double S0 = option.spot();
	const double r = option.inter();
	const double sigma = option.volat();
	const double T = option.time();
	const double K = option.strike();
	const auto  type = option.type();

	if (T == 0.0) {
		price_default = (type == OptionType::Call)
			? std::max(S0 - K, 0.0)
			: std::max(K - S0, 0.0);

		delta = (type == OptionType::Call)
			? (S0 > K ? 1.0 : (S0 == K ? 0.5 : 0.0))
			: (S0 < K ? -1.0 : (S0 == K ? -0.5 : 0.0));

		gamma = 0.0;
		theta = 0.0;
		vega = 0.0;
		rho = 0.0;
		return;
	}

	double M = steps;
	double t = T / M;

	double beta = (std::exp(-r * t) + std::exp((r + sigma * sigma) * t)) / 2;
	double u = beta + std::sqrt(beta * beta - 1);
	double d = beta - std::sqrt(beta * beta - 1);
	double p = (std::exp(r * t) - d) / (u - d);

	double bumpedV = sigma + 0.0001;
	double beta_bumpedV = (std::exp(-r * t) + std::exp((r + bumpedV * bumpedV) * t)) / 2;
	double u_bumpedV = beta_bumpedV + std::sqrt(beta_bumpedV * beta_bumpedV - 1);
	double d_bumpedV = beta_bumpedV - std::sqrt(beta_bumpedV * beta_bumpedV - 1);
	double p_bumpedV = (std::exp(r * t) - d_bumpedV) / (u_bumpedV - d_bumpedV);

	double bumpedr = r + 0.0001;
	double beta_bumpedr = (std::exp(-bumpedr * t) + std::exp((bumpedr + sigma * sigma) * t)) / 2;
	double u_bumpedr = beta_bumpedr + std::sqrt(beta_bumpedr * beta_bumpedr - 1);
	double d_bumpedr = beta_bumpedr - std::sqrt(beta_bumpedr * beta_bumpedr - 1);
	double p_bumpedr = (std::exp(bumpedr * t) - d_bumpedr) / (u_bumpedr - d_bumpedr);

	double bumped_downV = std::max(0.0, sigma - 0.0001);
	double beta_bumped_downV = (std::exp(-r * t) + std::exp((r + bumped_downV * bumped_downV) * t)) / 2;
	double u_bumped_downV = beta_bumped_downV + std::sqrt(beta_bumped_downV * beta_bumped_downV - 1);
	double d_bumped_downV = beta_bumped_downV - std::sqrt(beta_bumped_downV * beta_bumped_downV - 1);
	double p_bumped_downV = (std::exp(r * t) - d_bumped_downV) / (u_bumped_downV - d_bumped_downV);

	double bumped_downr = r - 0.0001;
	double beta_bumped_downr = (std::exp(-bumped_downr * t) + std::exp((bumped_downr + sigma * sigma) * t)) / 2;
	double u_bumped_downr = beta_bumped_downr + std::sqrt(beta_bumped_downr * beta_bumped_downr - 1);
	double d_bumped_downr = beta_bumped_downr - std::sqrt(beta_bumped_downr * beta_bumped_downr - 1);
	double p_bumped_downr = (std::exp(bumped_downr * t) - d_bumped_downr) / (u_bumped_downr - d_bumped_downr);

	int max = M;
	std::vector<double> currentV(M + 1);
	std::vector<double> currentS(M + 1);

	std::vector<double> currentV_bumpedV(M + 1);
	std::vector<double> currentS_bumpedV(M + 1);

	std::vector<double> currentV_bumpedr(M + 1);
	std::vector<double> currentS_bumpedr(M + 1);

	std::vector<double> currentV_bumped_downV(M + 1);
	std::vector<double> currentS_bumped_downV(M + 1);

	std::vector<double> currentV_bumped_downr(M + 1);
	std::vector<double> currentS_bumped_downr(M + 1);

	if (type == OptionType::Call) {
		for (int i{}; i < M + 1; ++i) {
			currentV[i] = std::max(S0 * std::pow(u, i) * std::pow(d, M - i) - K, 0.0);
			currentS[i] = S0 * std::pow(u, i) * std::pow(d, M - i);

			currentV_bumpedV[i] = std::max(S0 * std::pow(u_bumpedV, i) * std::pow(d_bumpedV, M - i) - K, 0.0);
			currentS_bumpedV[i] = S0 * std::pow(u_bumpedV, i) * std::pow(d_bumpedV, M - i);

			currentV_bumpedr[i] = std::max(S0 * std::pow(u_bumpedr, i) * std::pow(d_bumpedr, M - i) - K, 0.0);
			currentS_bumpedr[i] = S0 * std::pow(u_bumpedr, i) * std::pow(d_bumpedr, M - i);

			currentV_bumped_downV[i] = std::max(S0 * std::pow(u_bumped_downV, i) * std::pow(d_bumped_downV, M - i) - K, 0.0);
			currentS_bumped_downV[i] = S0 * std::pow(u_bumped_downV, i) * std::pow(d_bumped_downV, M - i);

			currentV_bumped_downr[i] = std::max(S0 * std::pow(u_bumped_downr, i) * std::pow(d_bumped_downr, M - i) - K, 0.0);
			currentS_bumped_downr[i] = S0 * std::pow(u_bumped_downr, i) * std::pow(d_bumped_downr, M - i);
		}

		for (int i{}; i < M - 2; ++i) {
			for (int j{}; j < max; ++j) {
				currentS[j] = std::exp(-r * t) * ((1 - p) * currentS[j] + p * currentS[j + 1]);
				currentV[j] = std::max(std::exp(-r * t) * ((1 - p) * currentV[j] + p * currentV[j + 1]), currentS[j] - K);

				currentS_bumpedV[j] = std::exp(-r * t) * ((1 - p_bumpedV) * currentS_bumpedV[j] + p_bumpedV * currentS_bumpedV[j + 1]);
				currentV_bumpedV[j] = std::max(std::exp(-r * t) * ((1 - p_bumpedV) * currentV_bumpedV[j] + p_bumpedV * currentV_bumpedV[j + 1]), currentS_bumpedV[j] - K);

				currentS_bumpedr[j] = std::exp(-bumpedr * t) * ((1 - p_bumpedr) * currentS_bumpedr[j] + p_bumpedr * currentS_bumpedr[j + 1]);
				currentV_bumpedr[j] = std::max(std::exp(-bumpedr * t) * ((1 - p_bumpedr) * currentV_bumpedr[j] + p_bumpedr * currentV_bumpedr[j + 1]), currentS_bumpedr[j] - K);

				currentS_bumped_downV[j] = std::exp(-r * t) * ((1 - p_bumped_downV) * currentS_bumped_downV[j] + p_bumped_downV * currentS_bumped_downV[j + 1]);
				currentV_bumped_downV[j] = std::max(std::exp(-r * t) * ((1 - p_bumped_downV) * currentV_bumped_downV[j] + p_bumped_downV * currentV_bumped_downV[j + 1]), currentS_bumped_downV[j] - K);

				currentS_bumped_downr[j] = std::exp(-bumped_downr * t) * ((1 - p_bumped_downr) * currentS_bumped_downr[j] + p_bumped_downr * currentS_bumped_downr[j + 1]);
				currentV_bumped_downr[j] = std::max(std::exp(-bumped_downr * t) * ((1 - p_bumped_downr) * currentV_bumped_downr[j] + p_bumped_downr * currentV_bumped_downr[j + 1]), currentS_bumped_downr[j] - K);
			}
			max--;
		}
	}

	else {
		for (int i{}; i < M + 1; ++i) {
			currentV[i] = std::max(K - S0 * std::pow(u, i) * std::pow(d, M - i), 0.0);
			currentS[i] = S0 * std::pow(u, i) * std::pow(d, M - i);

			currentV_bumpedV[i] = std::max(K - S0 * std::pow(u_bumpedV, i) * std::pow(d_bumpedV, M - i), 0.0);
			currentS_bumpedV[i] = S0 * std::pow(u_bumpedV, i) * std::pow(d_bumpedV, M - i);

			currentV_bumpedr[i] = std::max(K - S0 * std::pow(u_bumpedr, i) * std::pow(d_bumpedr, M - i), 0.0);
			currentS_bumpedr[i] = S0 * std::pow(u_bumpedr, i) * std::pow(d_bumpedr, M - i);

			currentV_bumped_downV[i] = std::max(K - S0 * std::pow(u_bumped_downV, i) * std::pow(d_bumped_downV, M - i), 0.0);
			currentS_bumped_downV[i] = S0 * std::pow(u_bumped_downV, i) * std::pow(d_bumped_downV, M - i);

			currentV_bumped_downr[i] = std::max(K - S0 * std::pow(u_bumped_downr, i) * std::pow(d_bumped_downr, M - i), 0.0);
			currentS_bumped_downr[i] = S0 * std::pow(u_bumped_downr, i) * std::pow(d_bumped_downr, M - i);
		}

		for (int i{}; i < M - 2; ++i) {
			for (int j{}; j < max; ++j) {
				currentS[j] = std::exp(-r * t) * ((1 - p) * currentS[j] + p * currentS[j + 1]);
				currentV[j] = std::max(std::exp(-r * t) * ((1 - p) * currentV[j] + p * currentV[j + 1]), K - currentS[j]);

				currentS_bumpedV[j] = std::exp(-r * t) * ((1 - p_bumpedV) * currentS_bumpedV[j] + p_bumpedV * currentS_bumpedV[j + 1]);
				currentV_bumpedV[j] = std::max(std::exp(-r * t) * ((1 - p_bumpedV) * currentV_bumpedV[j] + p_bumpedV * currentV_bumpedV[j + 1]), K - currentS_bumpedV[j]);

				currentS_bumpedr[j] = std::exp(-bumpedr * t) * ((1 - p_bumpedr) * currentS_bumpedr[j] + p_bumpedr * currentS_bumpedr[j + 1]);
				currentV_bumpedr[j] = std::max(std::exp(-bumpedr * t) * ((1 - p_bumpedr) * currentV_bumpedr[j] + p_bumpedr * currentV_bumpedr[j + 1]), K - currentS_bumpedr[j]);

				currentS_bumped_downV[j] = std::exp(-r * t) * ((1 - p_bumped_downV) * currentS_bumped_downV[j] + p_bumped_downV * currentS_bumped_downV[j + 1]);
				currentV_bumped_downV[j] = std::max(std::exp(-r * t) * ((1 - p_bumped_downV) * currentV_bumped_downV[j] + p_bumped_downV * currentV_bumped_downV[j + 1]), K - currentS_bumped_downV[j]);

				currentS_bumped_downr[j] = std::exp(-bumped_downr * t) * ((1 - p_bumped_downr) * currentS_bumped_downr[j] + p_bumped_downr * currentS_bumped_downr[j + 1]);
				currentV_bumped_downr[j] = std::max(std::exp(-bumped_downr * t) * ((1 - p_bumped_downr) * currentV_bumped_downr[j] + p_bumped_downr * currentV_bumped_downr[j + 1]), K - currentS_bumped_downr[j]);
			}
			max--;
		}
	}

	double deltaDown = (currentV[1] - currentV[0]) / (currentS[1] - currentS[0]);
	double deltaUp = (currentV[2] - currentV[1]) / (currentS[2] - currentS[1]);
	double Vud = currentV[1];

	gamma = (deltaUp - deltaDown) / (0.5 * (currentS[2] - currentS[0]));

	currentS[0] = std::exp(-r * t) * ((1 - p) * currentS[0] + p * currentS[1]);
	currentS[1] = std::exp(-r * t) * ((1 - p) * currentS[1] + p * currentS[2]);

	if (type == OptionType::Call) {
		currentV[0] = std::max(std::exp(-r * t) * ((1 - p) * currentV[0] + p * currentV[1]), currentS[0] - K);
		currentV[1] = std::max(std::exp(-r * t) * ((1 - p) * currentV[1] + p * currentV[2]), currentS[1] - K);
	}
	else {
		currentV[0] = std::max(std::exp(-r * t) * ((1 - p) * currentV[0] + p * currentV[1]), K - currentS[0]);
		currentV[1] = std::max(std::exp(-r * t) * ((1 - p) * currentV[1] + p * currentV[2]), K - currentS[1]);
	}

	delta = (currentV[1] - currentV[0]) / (currentS[1] - currentS[0]);

	currentS[0] = std::exp(-r * t) * ((1 - p) * currentS[0] + p * currentS[1]);

	if (type == OptionType::Call) {
		currentV[0] = std::max(std::exp(-r * t) * ((1 - p) * currentV[0] + p * currentV[1]), currentS[0] - K);
	}
	else {
		currentV[0] = std::max(std::exp(-r * t) * ((1 - p) * currentV[0] + p * currentV[1]), K - currentS[0]);
	}

	price_default = currentV[0];
	theta = (Vud - currentV[0]) / (2 * t);

	if (type == OptionType::Call) {
		for (int i{}; i < 2; ++i) {
			for (int j{}; j < max; ++j) {
				currentS_bumpedV[j] = std::exp(-r * t) * ((1 - p_bumpedV) * currentS_bumpedV[j] + p_bumpedV * currentS_bumpedV[j + 1]);
				currentV_bumpedV[j] = std::max(std::exp(-r * t) * ((1 - p_bumpedV) * currentV_bumpedV[j] + p_bumpedV * currentV_bumpedV[j + 1]), currentS_bumpedV[j] - K);

				currentS_bumpedr[j] = std::exp(-bumpedr * t) * ((1 - p_bumpedr) * currentS_bumpedr[j] + p_bumpedr * currentS_bumpedr[j + 1]);
				currentV_bumpedr[j] = std::max(std::exp(-bumpedr * t) * ((1 - p_bumpedr) * currentV_bumpedr[j] + p_bumpedr * currentV_bumpedr[j + 1]), currentS_bumpedr[j] - K);

				currentS_bumped_downV[j] = std::exp(-r * t) * ((1 - p_bumped_downV) * currentS_bumped_downV[j] + p_bumped_downV * currentS_bumped_downV[j + 1]);
				currentV_bumped_downV[j] = std::max(std::exp(-r * t) * ((1 - p_bumped_downV) * currentV_bumped_downV[j] + p_bumped_downV * currentV_bumped_downV[j + 1]), currentS_bumped_downV[j] - K);

				currentS_bumped_downr[j] = std::exp(-bumped_downr * t) * ((1 - p_bumped_downr) * currentS_bumped_downr[j] + p_bumped_downr * currentS_bumped_downr[j + 1]);
				currentV_bumped_downr[j] = std::max(std::exp(-bumped_downr * t) * ((1 - p_bumped_downr) * currentV_bumped_downr[j] + p_bumped_downr * currentV_bumped_downr[j + 1]), currentS_bumped_downr[j] - K);
			}
			max--;
		}
	}
	else {
		for (int i{}; i < 2; ++i) {
			for (int j{}; j < max; ++j) {
				currentS_bumpedV[j] = std::exp(-r * t) * ((1 - p_bumpedV) * currentS_bumpedV[j] + p_bumpedV * currentS_bumpedV[j + 1]);
				currentV_bumpedV[j] = std::max(std::exp(-r * t) * ((1 - p_bumpedV) * currentV_bumpedV[j] + p_bumpedV * currentV_bumpedV[j + 1]), K - currentS_bumpedV[j]);

				currentS_bumpedr[j] = std::exp(-bumpedr * t) * ((1 - p_bumpedr) * currentS_bumpedr[j] + p_bumpedr * currentS_bumpedr[j + 1]);
				currentV_bumpedr[j] = std::max(std::exp(-bumpedr * t) * ((1 - p_bumpedr) * currentV_bumpedr[j] + p_bumpedr * currentV_bumpedr[j + 1]), K - currentS_bumpedr[j]);

				currentS_bumped_downV[j] = std::exp(-r * t) * ((1 - p_bumped_downV) * currentS_bumped_downV[j] + p_bumped_downV * currentS_bumped_downV[j + 1]);
				currentV_bumped_downV[j] = std::max(std::exp(-r * t) * ((1 - p_bumped_downV) * currentV_bumped_downV[j] + p_bumped_downV * currentV_bumped_downV[j + 1]), K - currentS_bumped_downV[j]);

				currentS_bumped_downr[j] = std::exp(-bumped_downr * t) * ((1 - p_bumped_downr) * currentS_bumped_downr[j] + p_bumped_downr * currentS_bumped_downr[j + 1]);
				currentV_bumped_downr[j] = std::max(std::exp(-bumped_downr * t) * ((1 - p_bumped_downr) * currentV_bumped_downr[j] + p_bumped_downr * currentV_bumped_downr[j + 1]), K - currentS_bumped_downr[j]);
			}
			max--;
		}
	}

	vega = (currentV_bumpedV[0] - currentV_bumped_downV[0]) / (bumpedV - bumped_downV);
	rho = (currentV_bumpedr[0] - currentV_bumped_downr[0]) / (bumpedr - bumped_downr);
}
