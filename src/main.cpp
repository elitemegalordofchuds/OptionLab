#include <iostream>
#include "../include/Option.hpp"
#include <optional>
#include "../include/input.hpp"
#include "../include/Result.hpp"

int main() {
	Option option{};
	std::cout << "===============================================================================================================================\n";
	std::cout << "Chose option style: \n1)American\t\t2)European\t\t3)Asian\n";


	OptionStyle style= static_cast<OptionStyle> ( inputChoice (1, 3) );
	switch (style) {
	case OptionStyle::American:
		option.setStyle(OptionStyle::American); break;
	case OptionStyle::Euro:
		option.setStyle(OptionStyle::Euro); break;
	case OptionStyle::Asian:
		option.setStyle(OptionStyle::Asian);
	}

	std::cout << "===============================================================================================================================\n";
	std::cout << "Choose option type: \n1)Call\t\t2)Put\n";

	OptionType type = static_cast<OptionType> ( inputChoice (1, 2) );
	switch (type) {
	case OptionType::Call:
		option.setType(OptionType::Call); break;
	case OptionType::Put:
		option.setType(OptionType::Put);
	}

	std::cout << "===============================================================================================================================\n";
	std::cout << "Input spot price: ";
	option.setSpot( inputValue (0, std::nullopt) );

	std::cout << "===============================================================================================================================\n";
	std::cout << "Input strike price: ";
	option.setStrike( inputValue (0, std::nullopt) );

	std::cout << "===============================================================================================================================\n";
	std::cout << "Choose unit of time: \n1)Hours\t\t2)Days\t\t3)Weeks\t\t4)Months\t\t5)Years\n";
	TimeUnit unit = static_cast<TimeUnit>( inputChoice (1, 5) );
	std::cout << "Enter number of ";
	switch (unit) {
	case TimeUnit::Hours:
		std::cout << "hours"; break;
	case TimeUnit::Days:
		std::cout << "days"; break;
	case TimeUnit::Weeks:
		std::cout << "weeks"; break;
	case TimeUnit::Months:
		std::cout << "months"; break;
	case TimeUnit::Years:
		std::cout << "years";
	}
	std::cout << " till expiration\n";
	option.setTime(unit, inputValue (0, std::nullopt));

	std::cout << "===============================================================================================================================\n";
	std::cout << "Input volatility constant: ";
	option.setVolat( inputValue (0, std::nullopt) );

	std::cout << "===============================================================================================================================\n";
	std::cout << "Input risk free interest rate%: ";
	option.setInter( inputValue (0, 100) );

	if (option.style() == OptionStyle::Asian) {
		std::cout << "===============================================================================================================================\n";
		std::cout << "Choose averaging rate: \n1)Daily\t\t2)Weekly\t\t3)Monthly\n";
		option.setSteps(static_cast<AveragingRate>(inputChoice(1, 3)));
	}

	switch (option.style()) {
		case OptionStyle::Asian:
			Result<OptionStyle::Asian> res(option); break; res.Print();
		case OptionStyle::American:
			Result<OptionStyle::American> res(option); break; res.Print();
		case OptionStyle::Euro:
			Result<OptionStyle::Euro> res(option); res.Print();
	}
}