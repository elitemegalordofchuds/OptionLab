#include "../include/Tests/TestStructure.hpp"

Case::Case(const Option& option, Mode mode, bool base_case)
	: C_result{ option, mode, base_case },
	C_code{ Code },
	C_expectedPrice{
		(option.style() == OptionStyle::American)
			? pricesAmer[C_code - 1]
			: (option.style() == OptionStyle::Euro)
				? pricesEu[C_code - 1]
				: expectedPricesAsian[C_code - 1] } {
	Code++;
	if (Code == 27)
		Code = 1;
}

bool Case::test(double tolerance, PricingMethod method, std::string_view msg) const {
	double actualPrice{};

	switch (method) {
	case PricingMethod::ClosedForm:
		actualPrice = C_result.price();
		break;

	case PricingMethod::FDM:
		actualPrice = C_result.FDMResults()[4].first;
		break;

	case PricingMethod::Binomial:
		actualPrice = C_result.binomialResults()[4].first;
		break;

	case PricingMethod::MonteCarlo:
		actualPrice = std::get<0>(C_result.monteCarloResults()[4]);
		tolerance = 3 * std::get<2>(C_result.monteCarloResults()[4]) + 1e-6;
		break;

	default:
		return false;
	}

	double error = std::abs(actualPrice - C_expectedPrice);

	if (!(error < tolerance)) {
		std::cerr << msg
			<< "\nActual price: " << std::setprecision(15) << actualPrice
			<< "\nExpected price: " << C_expectedPrice
			<< "\nAbsolute error: " << error
			<< "\nAllowed error: " << tolerance << "\n\n";

		return false;
	}

	return true;
}

bool Case::testLessThan(const Case& case1, PricingMethod method, std::string_view msg) const {
	switch (method) {
	case PricingMethod::ClosedForm:
		if (!(C_result.price() < case1.result().price())) {
			std::cerr << msg << "\n";
			return false;
		}
		break;
	case PricingMethod::FDM:
		if (!(C_result.FDMResults()[4].first < case1.result().FDMResults()[4].first)) {
			std::cerr << msg << "\n";
			return false;
		}
		break;
	case PricingMethod::Binomial:
		if (!(C_result.binomialResults()[4].first < case1.result().binomialResults()[4].first)) {
			std::cerr << msg << "\n";
			return false;
		}
		break;
	case PricingMethod::MonteCarlo:
		if (!(std::get<0>(C_result.monteCarloResults()[4]) < std::get<0>(case1.result().monteCarloResults()[4]))) {
			std::cerr << msg << "\n";
			return false;
		}
		break;
	default:
		return false;
	}
	return true;
}