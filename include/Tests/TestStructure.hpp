#include "../Result.hpp"

#ifndef TESTSTRUCTURE_HPP
#define TESTSTRUCTURE_HPP
enum class PricingMethod {
	ClosedForm = 1,
	FDM,
	Binomial,
	MonteCarlo
};

class Case {
private:
	Result C_result{};
	int C_code{};
	double C_expectedPrice{};

	static inline int Code{ 1 };

	static constexpr std::array<double, 26> pricesEu{
		10.450584, 5.573526,
		54.970140, 0.093083,
		18.022951, 13.145894,
		0.359630, 43.044044,
		13.269677, 3.753418,
		999.048771, 950.229425,
		0.000000, 0.000000,
		0.000000, 0.000000,
		41.611758, 41.611758,
		4.877058, 0.000000,
		69.057470, 64.180412,
		7.965567, 7.965567,
		19.629887, 1.502962
	};

	static constexpr std::array<double, 26> pricesAmer{
	10.450584, 6.090358,
	54.970140, 0.096444,
	18.022951, 13.667581,
	0.359630, 50.000000,
	13.269677, 4.816267,
	999.048771, 999.000000,
	0.000000, 0.000000,
	0.000000, 0.000000,
	41.611758, 41.611758,
	4.877058, 0.000000,
	69.057470, 65.173422,
	7.965567, 7.965567,
	19.629887, 3.221380
	};

	static constexpr std::array<double, 26> expectedPricesAsian{
	6.156031, 3.534471,
	51.494294, 0.000482,
	10.801167, 8.179606,
	0.006058, 44.945969,
	7.555993, 2.480092,
	976.493799, 950.251979,
	0.000000, 0.000000,
	0.000000, 0.000000,
	24.592773, 24.592773,
	2.621560, 0.000000,
	44.536917, 41.915357,
	4.888785, 4.888785,
	10.627382, 1.108447
	};
public:
	Case(const Option& option, Mode mode = Mode::Testing, bool base_case = false);
	const Result& result() const {
		return C_result;
	}
	int code() const {
		return C_code;
	}
	double expectedPrice() const {
		return C_expectedPrice;
	}
	bool test(double tolerance, PricingMethod method, std::string_view msg) const;
	bool testLessThan(const Case& case1, PricingMethod method, std::string_view msg) const;
};

template <OptionStyle style>
class Cases {
protected:
	Case resultC{ { style, OptionType::Call, 100.0, 100.0, 1.0, 0.05, 0.2, 12 }, Mode::Experiment, true };
	Case resultP{ { style, OptionType::Put, 100.0, 100.0, 1.0, 0.05, 0.2, 12 }, Mode::Experiment, true };

	Case resultC_bumped_spot{ { style, OptionType::Call, 150.0, 100.0, 1.0, 0.05, 0.2, 12 } };
	Case resultP_bumped_spot{ { style, OptionType::Put, 150.0, 100.0, 1.0, 0.05, 0.2, 12 } };

	Case resultC_bumped_volatility{ { style, OptionType::Call, 100.0, 100.0, 1.0, 0.05, 0.4, 12 } };
	Case resultP_bumped_volatility{ { style, OptionType::Put, 100.0, 100.0, 1.0, 0.05, 0.4, 12 } };

	Case resultC_bumped_strike{ { style, OptionType::Call, 100.0, 150.0, 1.0, 0.05, 0.2, 12 } };
	Case resultP_bumped_strike{ { style, OptionType::Put, 100.0, 150.0, 1.0, 0.05, 0.2, 12 } };

	Case resultC_bumped_interest{ { style, OptionType::Call, 100.0, 100.0, 1.0, 0.1, 0.2, 12 } };
	Case resultP_bumped_interest{ { style, OptionType::Put, 100.0, 100.0, 1.0, 0.1, 0.2, 12 } };

	Case resultC_ITM{ { style, OptionType::Call, 1000.0, 1.0, 1.0, 0.05, 0.2, 12 } };
	Case resultP_ITM{ { style, OptionType::Put, 1.0, 1000.0, 1.0, 0.05, 0.2, 12 } };

	Case resultC_OTM{ { style, OptionType::Call, 1.0, 1000.0, 1.0, 0.05, 0.2, 12 } };
	Case resultP_OTM{ { style, OptionType::Put, 1000.0, 1.0, 1.0, 0.05, 0.2, 12 } };

	Case resultC_zero_maturity{ { style, OptionType::Call, 100.0, 100.0, 0.0, 0.05, 0.2, 0 } };
	Case resultP_zero_maturity{ { style, OptionType::Put, 100.0, 100.0, 0.0, 0.05, 0.2, 0 } };

	Case resultC_long_maturity{ { style, OptionType::Call, 100.0, 100.0, 30.0, 0.0, 0.2, 360 } };
	Case resultP_long_maturity{ { style, OptionType::Put, 100.0, 100.0, 30.0, 0.0, 0.2, 360 } };

	Case resultC_zero_volatility{ { style, OptionType::Call, 100.0, 100.0, 1.0, 0.05, 0.0, 12 } };
	Case resultP_zero_volatility{ { style, OptionType::Put, 100.0, 100.0, 1.0, 0.05, 0.0, 12 } };

	Case resultC_high_volatility{ { style, OptionType::Call, 100.0, 100.0, 1.0, 0.05, 2.0, 12 } };
	Case resultP_high_volatility{ { style, OptionType::Put, 100.0, 100.0, 1.0, 0.05, 2.0, 12 } };

	Case resultC_zero_interest{ { style, OptionType::Call, 100.0, 100.0, 1.0, 0.0, 0.2, 12 } };
	Case resultP_zero_interest{ { style, OptionType::Put, 100.0, 100.0, 1.0, 0.0, 0.2, 12 } };

	Case resultC_high_interest{ { style, OptionType::Call, 100.0, 100.0, 1.0, 0.2, 0.2, 12 } };
	Case resultP_high_interest{ { style, OptionType::Put, 100.0, 100.0, 1.0, 0.2, 0.2, 12 } };

	static inline bool fail = false;

	void basicTests(double precision, PricingMethod method) {
		std::string msgC{ "Error: inaccurate " };
		std::string msgP{ "Error: inaccurate " };

		switch (method) {
		case PricingMethod::ClosedForm:
			msgC += "closed form ";
			msgP += "closed form "; break;
		case PricingMethod::FDM:
			msgC += "FDM result for ";
			msgP += "FDM result for "; break;
		case PricingMethod::Binomial:
			msgC += "binomial ";
			msgP += "binomial "; break;
		case PricingMethod::MonteCarlo:
			msgC += "Monte Carlo ";
			msgP += "Monte Carlo ";
		}
		msgC += "pricing of ";
		msgP += "pricing of ";
		switch (style) {
		case OptionStyle::Euro:
			msgC += "European call. Error code - ";
			msgP += "European put. Error code - "; break;
		case OptionStyle::American:
			msgC += "American call. Error code - ";
			msgP += "American put. Error code - "; break;
		case OptionStyle::Asian:
			msgC += "Asian call. Error code - ";
			msgP += "Asian put. Error code - ";
		}
		fail |= !resultC.test(precision, method, msgC + std::to_string(resultC.code()));
		fail |= !resultP.test(precision, method, msgP + std::to_string(resultP.code()));

		fail |= !resultC_ITM.test(precision, method, msgC + std::to_string(resultC_ITM.code()));
		fail |= !resultP_ITM.test(precision, method, msgP + std::to_string(resultP_ITM.code()));

		fail |= !resultC_OTM.test(precision, method, msgC + std::to_string(resultC_OTM.code()));
		fail |= !resultP_OTM.test(precision, method, msgP + std::to_string(resultP_OTM.code()));

		fail |= !resultC_zero_interest.test(precision, method, msgC + std::to_string(resultC_zero_interest.code()));
		fail |= !resultP_zero_interest.test(precision, method, msgP + std::to_string(resultP_zero_interest.code()));

		fail |= !resultC_high_interest.test(precision, method, msgC + std::to_string(resultC_high_interest.code()));
		fail |= !resultP_high_interest.test(precision, method, msgP + std::to_string(resultP_high_interest.code()));

		fail |= !resultC_high_volatility.test(precision, method, msgC + std::to_string(resultC_high_volatility.code()));
		fail |= !resultP_high_volatility.test(precision, method, msgP + std::to_string(resultP_high_volatility.code()));

		fail |= !resultC_zero_volatility.test(precision, method, msgC + std::to_string(resultC_zero_volatility.code()));
		fail |= !resultP_zero_volatility.test(precision, method, msgP + std::to_string(resultP_zero_volatility.code()));

		fail |= !resultC_long_maturity.test(precision, method, msgC + std::to_string(resultC_long_maturity.code()));
		fail |= !resultP_long_maturity.test(precision, method, msgP + std::to_string(resultP_long_maturity.code()));

		fail |= !resultC_zero_maturity.test(precision, method, msgC + std::to_string(resultC_zero_maturity.code()));
		fail |= !resultP_zero_maturity.test(precision, method, msgP + std::to_string(resultP_zero_maturity.code()));


		fail |= !resultC.testLessThan(resultC_bumped_spot, method,
			msgC + std::to_string(resultC_bumped_spot.code()));

		fail |= !resultP_bumped_spot.testLessThan(resultP, method,
			msgP + std::to_string(resultP_bumped_spot.code()));

		fail |= !resultC.testLessThan(resultC_bumped_volatility, method,
			msgC + std::to_string(resultC_bumped_volatility.code()));

		fail |= !resultP.testLessThan(resultP_bumped_volatility, method,
			msgP + std::to_string(resultP_bumped_volatility.code()));

		fail |= !resultC.testLessThan(resultC_bumped_interest, method,
			msgC + std::to_string(resultC_bumped_interest.code()));

		fail |= !resultP_bumped_interest.testLessThan(resultP, method,
			msgP + std::to_string(resultP_bumped_interest.code()));

		fail |= !resultC_bumped_strike.testLessThan(resultC, method,
			msgC + std::to_string(resultC_bumped_strike.code()));

		fail |= !resultP.testLessThan(resultP_bumped_strike, method,
			msgP + std::to_string(resultP_bumped_strike.code()));
	}
};
#endif