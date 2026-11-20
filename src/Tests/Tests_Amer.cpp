#include "../include/Tests/TestRunners.hpp"

void AmerTests::amer_testFDM() {
	auto resC = resultC.result().FDMResults();
	if (!(std::abs(resC[0].first - resultC.expectedPrice()) > std::abs(resC[4].first - resultC.expectedPrice()))) {
		std::cerr << "Error: FDM results do not converge for American call. Error code 1\n";
		fail = true;
	}

	auto resP = resultP.result().FDMResults();
	if (!(std::abs(resP[0].first - resultP.expectedPrice()) > std::abs(resP[4].first - resultP.expectedPrice()))) {
		std::cerr << "Error: FDM results do not converge for American put. Error code 1\n";
		fail = true;
	}

	basicTests(1e-3, PricingMethod::FDM);
}
void AmerTests::amer_testBinomial() {
	auto resC = resultC.result().binomialResults();
	if (!(std::abs(resC[0].first - resultC.expectedPrice()) > std::abs(resC[4].first - resultC.expectedPrice()))) {
		std::cerr << "Error: binomial results do not converge for American call. Error code 1\n";
		fail = true;
	}

	auto resP = resultP.result().binomialResults();
	if (!(std::abs(resP[0].first - resultP.expectedPrice()) > std::abs(resP[4].first - resultP.expectedPrice()))) {
		std::cerr << "Error: binomial results do not converge for American put. Error code 1\n";
		fail = true;
	}
	basicTests(2.5e-3, PricingMethod::Binomial);
}
void AmerTests::amer_testGreeks() {
	auto [deltaC, gammaC, thetaC, vegaC, rhoC] = resultC.result().greeks();
	if (!(std::abs(deltaC - 0.6368306512) < 1e-4)) {
		std::cerr << "Error: inaccurate delta calculation for American call. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(gammaC - 0.0187620173) < 1e-5)) {
		std::cerr << "Error: inaccurate gamma calculation for American call. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(thetaC - -6.4140275464) < 1e-3)) {
		std::cerr << "Error: inaccurate theta calculation for American call. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(vegaC - 37.5240346917) < 1e-3)) {
		std::cerr << "Error: inaccurate vega calculation for American call. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(rhoC - 53.2324815454) < 1e-3)) {
		std::cerr << "Error: inaccurate rho calculation for American call. Error code 1\n";
		fail = true;
	}

	auto [deltaP, gammaP, thetaP, vegaP, rhoP] = resultP.result().greeks();
	if (!(std::abs(deltaP - -0.4110608) < 1e-4)) {
		std::cerr << "Error: inaccurate delta calculation for American put. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(gammaP - 0.0229891) < 1e-5)) {
		std::cerr << "Error: inaccurate gamma calculation for American put. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(thetaP - -2.2379976) < 1e-3)) {
		std::cerr << "Error: inaccurate theta calculation for American put. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(vegaP - 37.4876986) < 1e-3)) {
		std::cerr << "Error: inaccurate vega calculation for American put. Error code 1\n";
		fail = true;
	}
	if (!(std::abs(rhoP - -30.2167339) < 1e-3)) {
		std::cerr << "Error: inaccurate rho calculation for American put. Error code 1\n";
		fail = true;
	}

	std::cout << std::setprecision(12);

	std::cout << "\nAmerican Call Greeks:\n"
		<< "Delta: " << deltaC << '\n'
		<< "Gamma: " << gammaC << '\n'
		<< "Theta: " << thetaC << '\n'
		<< "Vega: " << vegaC << '\n'
		<< "Rho: " << rhoC << "\n";

	std::cout << "\nAmerican Put Greeks:\n"
		<< "Delta: " << deltaP << '\n'
		<< "Gamma: " << gammaP << '\n'
		<< "Theta: " << thetaP << '\n'
		<< "Vega: " << vegaP << '\n'
		<< "Rho: " << rhoP << "\n\n";
}
int AmerTests::runTests() {
	std::cout << "Starting American tests...\n";
	AmerTests tests{};
	tests.amer_testFDM();
	tests.amer_testGreeks();
	tests.amer_testBinomial();
	std::cout << "American tests run.\n";
	return !fail;
}




