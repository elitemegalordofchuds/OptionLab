#include <cstdint>

#ifndef OPTION_HPP
#define OPTION_HPP

enum class OptionStyle : uint8_t {
	American = 1,
	Euro,
	Asian,
};
enum class AveragingRate : uint8_t {
	Daily = 1,
	Weekly,
	Monthly,
};
enum class OptionType : uint8_t {
	Call = 1,
	Put,
};
enum class TimeUnit : uint8_t {
	Hours = 1,
	Days,
	Weeks,
	Months,
	Years,
};

class Option {
private:
	OptionStyle op_style{};
	OptionType op_type{};
	double op_spot{};
	double op_strike{};
	double op_time{};
	double op_interest{};
	double op_volatility{};
	int asian_steps{};

public:
	Option(OptionStyle style = OptionStyle::Euro, OptionType type = OptionType::Call, double spot = 0, double strike = 0, double time = 0,double interest = 0, double volatility = 0, int steps = 0) :
		op_style{ style }, op_type{ type }, op_spot{ spot }, op_strike{ strike }, op_time{ time }, op_interest{ interest }, op_volatility{ volatility },
		asian_steps{ steps } {}
	void setStyle(OptionStyle style) {
		switch (style) {
		case OptionStyle::American:
			op_style = OptionStyle::American; break;
		case OptionStyle::Euro:
			op_style = OptionStyle::Euro; break;
		case OptionStyle::Asian:
			op_style = OptionStyle::Asian;
		}
	}
	void setType(OptionType type) {
		switch (type) {
		case OptionType::Call:
			op_type = OptionType::Call; break;
		case OptionType::Put:
			op_type = OptionType::Put;
		}
	}
	void setSpot(double s) {
		op_spot = s;
	}
	void setStrike(double k) {
		op_strike = k;
	}
	void setTime(TimeUnit u, double t) {
		switch (u) {
		case TimeUnit::Hours:
			op_time = (t / 24) / 365; break;
		case TimeUnit::Days:
			op_time = t / 365; break;
		case TimeUnit::Weeks:
			op_time = (t * 7) / 365; break;
		case TimeUnit::Months:
			op_time = (t * 30) / 365; break;
		case TimeUnit::Years:
			op_time = t;
		}
	}
	void setInter(double r) {
		op_interest = r;
	}
	void setVolat(double sigma) {
		op_volatility = sigma;
	}
	void setSteps(AveragingRate s) {
		switch (s) {
		case AveragingRate::Daily:
			asian_steps = 252 * op_time; break;
		case AveragingRate::Weekly:
			asian_steps = 52 * op_time; break;
		case AveragingRate::Monthly:
			asian_steps = 12 * op_time;
		}
	}

	int steps() const {
		return asian_steps;
	}
	OptionStyle style() const {
		return op_style;
	}
	OptionType type() const{
		return op_type;
	}
	double spot() const {
		return op_spot;
	}
	double strike() const {
		return op_strike;
	}
	double time() const {
		return op_time;
	}
	double inter() const {
		return op_interest;
	}
	double volat() const {
		return op_volatility;
	}
};
#endif
