#include <iostream>
#include <optional>
#include "../include/Result.hpp"
#include "../include/Tests/TestRunners.hpp"

int main(int argc, char* argv[]) {
	if (argc > 1) {
		std::cout << "Starting tests...\n";
		std::string_view arg{ argv[1] };
		if (arg == "--test") {
			if (argc > 2) {
				std::string arg2{ argv[2] };
				std::transform(arg2.begin(), arg2.end(), arg2.begin(), [](unsigned char c) {return std::tolower(c);});
				if (arg2 == "european") {
					if (EuTests::runTests()) {
						std::cout << "Asian tests passed successfully.\n";
						return 0;
					}
					else return 1;
				}
				if (arg2 == "american") {
					if (AmerTests::runTests()) {
						std::cout << "American tests passed successfully.\n";
						return 0;
					}
					else return 1;
				}
				if (arg2 == "asian") {
					if (AsianTests::runTests()) {
						std::cout << "Asian tests passed successfully.\n";
						return 0;
					}
					else return 1;
				}
				std::cout << "Available commands : \n"
					<< "  option_pricer\n"
					<< "  option_pricer --test\n"
					<< "  option_pricer --test european\n"
					<< "  option_pricer --test american\n"
					<< "  option_pricer --test asian\n";
				return 1;
			}
			else {
				bool fail = EuTests::runTests() + AmerTests::runTests() + AsianTests::runTests();
				if (fail) {
					std::cerr << "Something went wrong...";
					return 1;
				}
				std::cout << "All tests passed successfully\n";
				return 0;
			}
		}
		else {
			std::cout << "Available commands : \n"
				<< "  option_pricer\n"
				<< "  option_pricer --test\n"
				<< "  option_pricer --test european\n"
				<< "  option_pricer --test american\n"
				<< "  option_pricer --test asian\n";
			return 1;
		}
	}

	Option option{};
	std::cout << "Chose operating mode: \n1)Default\t\t2)Experiment\n";
	bool expirement = inputChoice(1, 2) - 1;


	std::cout << "===============================================================================================================================\n";
	std::cout << "Chose option style: \n1)American\t\t2)European\t\t3)Asian\n";
	OptionStyle style = static_cast<OptionStyle> (inputChoice(1, 3));
	option.setStyle(style);


	std::cout << "===============================================================================================================================\n";
	std::cout << "Choose option type: \n1)Call\t\t2)Put\n";
	OptionType type = static_cast<OptionType> (inputChoice(1, 2));
	option.setType(type);



	std::cout << "===============================================================================================================================\n";
	std::cout << "Input spot price: ";
	option.setSpot(inputValue(0, std::nullopt));


	std::cout << "===============================================================================================================================\n";
	std::cout << "Input strike price: ";
	option.setStrike(inputValue(0, std::nullopt));


	std::cout << "===============================================================================================================================\n";
	std::cout << "Choose unit of time: \n1)Hours\t\t2)Days\t\t3)Weeks\t\t4)Months\t\t5)Years\n";
	int unit = inputChoice(1, 5);
	std::cout << "Enter number of ";
	switch (unit) {
	case 1:
		std::cout << "hours"; break;
	case 2:
		std::cout << "days"; break;
	case 3:
		std::cout << "weeks"; break;
	case 4:
		std::cout << "months"; break;
	case 5:
		std::cout << "years";
	}
	std::cout << " till expiration\n";
	option.setTime(static_cast<TimeUnit> (unit), inputValue(0, std::nullopt));


	std::cout << "===============================================================================================================================\n";
	std::cout << "Input volatility constant: ";
	option.setVolat(inputValue(0, std::nullopt));


	std::cout << "===============================================================================================================================\n";
	std::cout << "Input risk free interest rate%: ";
	option.setInter(inputValue(0, 100));


	Result result{ option, expirement ? Mode::Expirement : Mode::Default };
	result.Print();
	std::cout << "===============================================================================================================================\n";
	std::cout << "Do you wish to export the results as a csv file? \n1)Yes 2)No";
	bool noExport = inputChoice(1, 2) - 1;
	if (!noExport) {
		std::cout << "Please name you results file: ";
		std::string filename{};
		std::cin >> filename;
		result.Export(filename);
	}
}