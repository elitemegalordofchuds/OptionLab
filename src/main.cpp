#include <iostream>
#include <optional>
#include "../include/Result.hpp"
#include "../include/Tests/TestRunners.hpp"
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

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
						std::cout << "European tests passed successfully.\n";
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
				int allPass = EuTests::runTests() + AmerTests::runTests() + AsianTests::runTests();
				if (allPass < 3) {
					std::cerr << "Something went wrong...";
					return 1;
				}
				std::cout << "All tests passed successfully\n";
				return 0;
			}
		}
		else {
			std::cout << "Available commands : \n"
				<< "  OptionLab\n"
				<< "  OptionLab --test\n"
				<< "  OptionLab --test european\n"
				<< "  OptiopLab --test american\n"
				<< "  OptionLab --test asian\n";
			return 1;
		}
	}

	Option option{};
	std::cout << "Choose operating mode: \n1)Default\t\t2)Experiment\n";
	bool experiment = inputChoice(1, 2) - 1;


	printSeparator();
	std::cout << "Choose option style: \n1)American\t\t2)European\t\t3)Asian\n";
	OptionStyle style = static_cast<OptionStyle> (inputChoice(1, 3));
	option.setStyle(style);


	printSeparator();
	std::cout << "Choose option type: \n1)Call\t\t2)Put\n";
	OptionType type = static_cast<OptionType> (inputChoice(1, 2));
	option.setType(type);



	printSeparator();
	std::cout << "Input spot price: ";
	option.setSpot(inputValue(0, std::nullopt));


	printSeparator();
	std::cout << "Input strike price: ";
	option.setStrike(inputValue(0, std::nullopt));


	printSeparator();
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
	if (option.style() == OptionStyle::Asian) {
		printSeparator();
		std::cout << "Choose averaging rate: \n1)Daily\t\t2)Weekly\t\t3)Monthly\n";
		AveragingRate rate = static_cast<AveragingRate> (inputChoice(1, 3));
		option.setSteps(rate);
	}


	printSeparator();
	std::cout << "Input volatility constant: ";
	option.setVolat(inputValue(0, std::nullopt));


	printSeparator();
	std::cout << "Input risk free interest rate%: ";
	option.setInter(inputValue(0, 100) / 100);
	printSeparator();


	Result result{ option, experiment ? Mode::Experiment : Mode::Default };
	result.Print();
	if (experiment) {
		printSeparator();
		std::cout << "Do you wish to export the results as csv files? \n1)Yes\t\t2)No\n";
		bool noExport = inputChoice(1, 2) - 1;
		if (!noExport) {
			std::string filename{};
			std::cout << "Please name your results folder.\n";
			std::cin >> filename;
			while (result.Export(filename) != 0) {
				std::cout << "Something went wrong during the export process. Do you wish to try again?\n";
				noExport = inputChoice(1, 2) - 1;
				if (!noExport) {
					std::cout << "Please name your results folder.\n";
					std::cin >> filename;
				}
				else {
					return 0;
				}
			}
			printSeparator();
			std::cout << "Do you wish to graph the results? \n1)Yes\t\t2)No\n";
			bool noGraph = inputChoice(1, 2) - 1;
			if (!noGraph) {
				if (generateGraphs(filename) == 0) {
					std::cout << "Graphs generated successfully!\n";
				}
				else {
					std::cerr << "Graph generation failed. Check that Python 3 and Matplotlib are installed.\n";
					return 1;
				}
			}
		}
	}
	return 0;
}