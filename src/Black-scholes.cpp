#include <cmath>
#include "../include/Types.hpp"
#include <tuple>
#include <numbers>


resultEuro BlackScholes(const Option& option, OptionStyle style) {
	auto S = option.spot();
	auto K = option.strike();
	auto r = option.inter();
	auto sigma = option.volat();
	auto T = option.time();
	auto type = option.type();
	resultEuro res{};

	double d1 = (std::log(S / K) + (r + std::pow(sigma, 2) / 2) * T) /
								(sigma * std::sqrt(T));

	double d2 = d1 - sigma * std::sqrt(T);

	double N1{};
	double N2{};
	double n1{};
	double n2{};
	if (type == OptionType::Call) {
		N1 = 0.5 + 0.5 * std::erf(d1 / std::sqrt(2));
		N2 = 0.5 + 0.5 * std::erf(d2 / std::sqrt(2));
		n1 = (1 / std::sqrt(std::numbers::pi * 2)) * std::exp(-std::pow(d1, 2) / 2);
		n2 = (1 / std::sqrt(std::numbers::pi * 2)) * std::exp(-std::pow(d2, 2) / 2);
		res.price_bs = S * N1 - K * std::exp(-r * T) * N2;
		res.delta = N1;
		res.gamma = n1 / (S * sigma * std::sqrt(T));
		res.rho = K * T * std::exp(-r * T) * N2;
		res.vega = S * n1 * std::sqrt(T);
		res.theta = (-S * n1 * sigma) / (2 * std::sqrt(T)) - r * K * std::exp(-r * T) * n2;
	}
	else {
		N1 = 0.5 + 0.5 * std::erf(-d1 / std::sqrt(2));
		N2 = 0.5 + 0.5 * std::erf(-d2 / std::sqrt(2));
		n1 = (1 / std::sqrt(std::numbers::pi * 2)) * std::exp(-std::pow(-d1, 2) / 2);
		n2 = (1 / std::sqrt(std::numbers::pi * 2)) * std::exp(-std::pow(-d2, 2) / 2);
		res.price_bs = K * std::exp(-r * T) * N2 - S * N1;
		res.delta = -N1;
		res.gamma = n1 / (S * sigma * std::sqrt(T));
		res.rho = -K * T * std::exp(-r * T) * N2;
		res.vega = S * n1 * std::sqrt(T);
		res.theta = res.theta = (-S * n1 * sigma) / (2 * std::sqrt(T)) + r * K * std::exp(-r * T) * n2;
	}
}