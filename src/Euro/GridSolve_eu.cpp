#include "../include/Option.hpp"
#include <cmath>
#include "../include/RNG.hpp"
#include "../../include/Result.hpp"

void Result<OptionStyle::Euro>::GridSolve_eu(const Option& option) {
	double S0 = option.spot();
	double r = option.inter();
	double sigma = option.volat();
	double T = option.time();
	double K = option.strike();
	

}