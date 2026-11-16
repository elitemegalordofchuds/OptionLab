#include "../include/Tests/TestRunners.hpp"

void AsianTests::asian_testMonteCarlo() {
	auto resC = resultC.result().monteCarloResults();
	if (!(std::abs(std::get<0>(resC[0]) - resultC.expectedPrice()) > std::abs(std::get<0>(resC[4]) - resultC.expectedPrice()))) {
		std::cerr << "Error: Monte Carlo results do not converge for Asian call. Error code 1\n";
		fail = true;
	}

	auto resP = resultP.result().monteCarloResults();
	if (!(std::abs(std::get<0>(resP[0]) - resultP.expectedPrice()) > std::abs(std::get<0>(resP[4]) - resultP.expectedPrice()))) {
		std::cerr << "Error: Monte Carlo results do not converge for Asian put. Error code 1\n";
		fail = true;
	}

	basicTests(0, PricingMethod::MonteCarlo);
}
void AsianTests::asian_testGreeks() {
		auto [deltaC, gammaC, thetaC, vegaC, rhoC] = resultC.result().greeks();
	if (!(std::abs(deltaC - 0.59388) < 1e-3)) {
		std::cerr << "Error: inaccurate delta calculation for Asian call. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(gammaC - 0.03059) < 2e-3)) {
		std::cerr << "Error: inaccurate gamma calculation for Asian call. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(thetaC - -3.64157) < 0.015)) {
		std::cerr << "Error: inaccurate theta calculation for Asian call. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(vegaC - 23.0331) < 0.02)) {
		std::cerr << "Error: inaccurate vega calculation for Asian call. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(rhoC - 26.7651) < 0.05)) {
		std::cerr << "Error: inaccurate rho calculation for Asian call. Error code 1\n";
		fail = true;
	}

	auto [deltaP, gammaP, thetaP, vegaP, rhoP] = resultP.result().greeks();
	if (!(std::abs(deltaP - -0.38356) < 1e-3)) {
		std::cerr << "Error: inaccurate delta calculation for Asian put. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(gammaP - 0.03059) < 2e-3)) {
		std::cerr << "Error: inaccurate gamma calculation for Asian put. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(thetaP - -1.10518) < 0.015)) {
		std::cerr << "Error: inaccurate theta calculation for Asian put. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(vegaP - 23.0331) < 0.02)) {
		std::cerr << "Error: inaccurate vega calculation for Asian put. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(rhoP - -23.9627) < 0.05)) {
		std::cerr << "Error: inaccurate rho calculation for  Asian put. Error code 1\n";
		fail = true;
	}

	std::cout << std::setprecision(12);

	std::cout << "\nAsian Call Greeks:\n"
		<< "Delta: " << deltaC << '\n'
		<< "Gamma: " << gammaC << '\n'
		<< "Theta: " << thetaC << '\n'
		<< "Vega: " << vegaC << '\n'
		<< "Rho: " << rhoC << "\n";

	std::cout << "\nAsian Put Greeks:\n"
		<< "Delta: " << deltaP << '\n'
		<< "Gamma: " << gammaP << '\n'
		<< "Theta: " << thetaP << '\n'
		<< "Vega: " << vegaP << '\n'
		<< "Rho: " << rhoP << "\n\n";
}
int AsianTests::runTests() {
	std::cout << "Starting Asian tests...\n";
	AsianTests tests{};
	tests.asian_testMonteCarlo();
	tests.asian_testGreeks();
	std::cout << "Asian tests run.\n";
	return !fail;
}
