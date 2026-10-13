#include <cstdint>

#ifndef Types
#define Types

enum class OptionStyle : uint8_t {
	American = 1,
	Euro,
	Asian,
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
public:
	Option() = default;
	void setStyle(OptionStyle style) {
		switch (style) {
		case OptionStyle::American:
			op_style = OptionStyle::American;
		case OptionStyle::Euro:
			op_style = OptionStyle::Euro;
		case OptionStyle::Asian:
			op_style = OptionStyle::Asian;
		}
	}
	void setType(OptionType type) {
		switch (type) {
		case OptionType::Call:
			op_type = OptionType::Call;
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
			op_time = (t / 24) / 365;
		case TimeUnit::Days:
			op_time = t / 365;
		case TimeUnit::Weeks:
			op_time = (t * 7) / 365;
		case TimeUnit::Months:
			op_time = (t * 30) / 365;
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

struct resultEuro {
	double delta{};
	double gamma{};
	double theta{};
	double vega{};
	double rho{};
	double price_bs{};
	double price_monteC{};
	double price_grid{};
};

struct resultAsian {
	 double delta{};
	 double gamma{};
	 double theta{};
	 double vega{};
	 double rho{};
	 double price_monteC{};
};

struct resultAmer {
	double delta{};
	double gamma{};
	double theta{};
	double vega{};
	double rho{};
	double price_tree{};
	double price_grid{};
};

#endif
