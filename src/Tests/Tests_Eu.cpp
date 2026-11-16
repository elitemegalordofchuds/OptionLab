#include "../include/Tests/TestRunners.hpp"

void EuTests::eu_testClosedForm() {
	Cases<OptionStyle::Euro>::basicTests(1e-6, PricingMethod::ClosedForm);
}

void EuTests::eu_testFDM() {
	auto resC = resultC.result().FDMResults();
	if (!(std::abs(resC[0].first - resultC.expectedPrice()) > std::abs(resC[4].first - resultC.expectedPrice()))) {
		std::cerr << "Error: FDM results do not converge for European call\n";
		fail = true;
	}

	auto resP = resultP.result().FDMResults();
	if (!(std::abs(resP[0].first - resultP.expectedPrice()) > std::abs(resP[4].first - resultP.expectedPrice()))) {
		std::cerr << "Error: FDM results do not converge for European put\n";
		fail = true;
	}

	Cases<OptionStyle::Euro>::basicTests(1e-4, PricingMethod::FDM);
}

void EuTests::eu_testBinomial() {
	auto resC = resultC.result().binomialResults();
	if (!(std::abs(resC[0].first - resultC.expectedPrice()) > std::abs(resC[4].first - resultC.expectedPrice()))) {
		std::cerr << "Error: binomial results do not converge for European call. Error code 1\n";
		fail = true;
	}

	auto resP = resultP.result().binomialResults();
	if (!(std::abs(resP[0].first - resultP.expectedPrice()) > std::abs(resP[4].first - resultP.expectedPrice()))) {
		std::cerr << "Error: binomial results do not converge for European put.Error code 1\n";
		fail = true;
	}

	basicTests(1e-3, PricingMethod::Binomial);
}

void EuTests::eu_testMonteCarlo() {
	auto resC = resultC.result().monteCarloResults();
	if (!(std::get<2>(resC[0]) > std::get<2>(resC[4]))) {
		std::cerr << "Error: Monte Carlo results do not converge for European call. Error code 1\n";
		fail = true;
	}

	auto resP = resultP.result().monteCarloResults();
	if (!(std::get<2>(resP[0]) > std::get<2>(resP[4]))) {
		std::cerr << "Error: Monte Carlo results do not converge for European put. Error code 1\n";
		fail = true;
	}

	Cases<OptionStyle::Euro>::basicTests(0, PricingMethod::MonteCarlo);
}

void EuTests::eu_testGreeks() {
	auto [deltaC, gammaC, thetaC, vegaC, rhoC] = resultC.result().greeks();
	if (!(std::abs(deltaC - 0.6368306512) < 1e-6)) {
		std::cerr << "Error: inaccurate delta calculation for a European call\n";
		fail = true;
	}
	if (!(std::abs(gammaC - 0.0187620173) < 1e-6)) {
		std::cerr << "Error: inaccurate gamma calculation for a European call\n";
		fail = true;
	}
	if (!(std::abs(thetaC - -6.4140275464) < 1e-6)) {
		std::cerr << "Error: inaccurate theta calculation for a European call\n";
		fail = true;
	}
	if (!(std::abs(vegaC - 37.5240346917) < 1e-6)) {
		std::cerr << "Error: inaccurate vega calculation for a European call\n";
		fail = true;
	}
	if (!(std::abs(rhoC - 53.2324815454) < 1e-6)) {
		std::cerr << "Error: inaccurate rho calculation for a European call\n";
		fail = true;
	}

	auto [deltaP, gammaP, thetaP, vegaP, rhoP] = resultP.result().greeks();
	if (!(std::abs(deltaP - -0.3631693488) < 1e-6)) {
		std::cerr << "Error: inaccurate delta calculation for a European put\n";
		fail = true;
	}
	if (!(std::abs(gammaP - 0.0187620173) < 1e-6)) {
		std::cerr << "Error: inaccurate gamma calculation for a European put\n";
		fail = true;
	}
	if (!(std::abs(thetaP - -1.6578804239) < 1e-6)) {
		std::cerr << "Error: inaccurate theta calculation for a European put\n";
		fail = true;
	}
	if (!(std::abs(vegaP - 37.5240346917) < 1e-6)) {
		std::cerr << "Error: inaccurate vega calculation for a European put\n";
		fail = true;
	}
	if (!(std::abs(rhoP - -41.8904609047) < 1e-6)) {
		std::cerr << "Error: inaccurate rho calculation for a European put\n";
		fail = true;
	}
	std::cout << std::setprecision(12);

	std::cout << "\European Call Greeks:\n"
		<< "Delta: " << deltaC << '\n'
		<< "Gamma: " << gammaC << '\n'
		<< "Theta: " << thetaC << '\n'
		<< "Vega: " << vegaC << '\n'
		<< "Rho: " << rhoC << "\n";

	std::cout << "\European Put Greeks:\n"
		<< "Delta: " << deltaP << '\n'
		<< "Gamma: " << gammaP << '\n'
		<< "Theta: " << thetaP << '\n'
		<< "Vega: " << vegaP << '\n'
		<< "Rho: " << rhoP << "\n\n";
}
int EuTests::runTests() {
	std::cout << "Starting Eu tests...\n";
	EuTests tests;
	tests.eu_testClosedForm();
	tests.eu_testFDM();
	tests.eu_testBinomial();
	tests.eu_testMonteCarlo();
	tests.eu_testGreeks();
	std::cout << "Eu tests done.\n";
	return !fail;
}