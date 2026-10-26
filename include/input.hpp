#include <iostream>
#include <optional>

#ifndef INPUT
#define INPUT

inline int inputChoice(int min, int max) {
	int x{};
	while (!std::cin >> x) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		std::cout << "Please choose from options " << min << " to " << max << "\n";
	}
	return x;
}


inline double inputValue(std::optional<double> min, std::optional<double> max) {
	double x{};
	int msg{};
	if (!min && !max) {
		msg = 0;
	}
	else if (min && !max) {
		msg = 1;
	}
	else if (max && !min) {
		msg = 2;
	}
	else {
		msg = 3;
	}
	while (!(std::cin >> x) || (min && x < min.value()) || (max && x > max.value())) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (msg) {
		case 0:
			std::cout << "Please enter a valid value\n"; break;
		case 1:
			std::cout << "Please enter a value greate than " << min.value() << "\n"; break;
		case 2:
			std::cout << "Please enter a value less than " << max.value() << "\n"; break;
		case 3:
			std::cout << "Please enter a value between " << min.value() << " and " << min.value() << "\n";
		}
	}
	return x;
}
#endif