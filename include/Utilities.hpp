#include <random>
#include <thread>
#include <optional>
#include <iostream>
#include <cmath>
#include <array>
#include <tuple>
#include <chrono>
#include <future>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#ifndef UTILS_HPP
#define UTILS_HPP

class Rand {
private:
	std::random_device rd{};
	std::normal_distribution<long double> Z{ 0.0, 1.0 };
	std::seed_seq seed = { static_cast<unsigned int> (std::hash<std::thread::id>{}(std::this_thread::get_id())), rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
	std::mt19937_64 mt{ seed };
public:
	Rand() {};
	auto getZ() {
		return Z(mt);
	}
};

inline int inputChoice(int min, int max) {
	int x{};
	while (!(std::cin >> x) || x < min || x > max) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		std::cout << "Please choose from options " << min << " to " << max << "\n";
	}
	return x;
}

inline double inputValue(std::optional<int> min, std::optional<int> max) {
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
			std::cout << "Please enter a value greater than " << min.value() << "\n"; break;
		case 2:
			std::cout << "Please enter a value less than " << max.value() << "\n"; break;
		case 3:
			std::cout << "Please enter a value between " << min.value() << " and " << max.value() << "\n";
		}
	}
	return x;
}
inline std::size_t getTerminalWidth() {

#ifdef _WIN32

	CONSOLE_SCREEN_BUFFER_INFO info{};
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	if (console != INVALID_HANDLE_VALUE &&
		GetConsoleScreenBufferInfo(console, &info)) {

		return static_cast<std::size_t>(
			info.srWindow.Right - info.srWindow.Left + 1
			);
	}

#else

	struct winsize size {};

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 &&
		size.ws_col > 0) {

		return size.ws_col;
	}

#endif

	return 80;
}
inline void printSeparator() {
	std::cout << std::string(getTerminalWidth(), '=') << '\n';
}
inline int generateGraphs(const std::string& folderName) {

	const std::filesystem::path scriptPath =
		std::filesystem::path(OPTIONLAB_PROJECT_DIR)
		/ "analysis" / "plot_results.py";

	if (!std::filesystem::is_regular_file(scriptPath)) {
		std::cerr << "Could not find Python plotting script.\n";
		return 1;
	}

	const std::string script = scriptPath.string();
	const std::string folder = folderName;

	std::cout << "\nGenerating graphs...\n";

#ifdef _WIN32

	const char* const args[] = {
		"py",
		"-3",
		script.c_str(),
		folder.c_str(),
		nullptr
	};

	const auto exitCode = _spawnvp(_P_WAIT, "py", args);

	if (exitCode == -1) {
		std::perror("Failed to launch Python");
		return 1;
	}

	return static_cast<int>(exitCode);

#elif defined(__linux__) || defined(__APPLE__)

	std::string executable = "python3";

	char* const args[] = {
		executable.data(),
		const_cast<char*>(script.c_str()),
		const_cast<char*>(folder.c_str()),
		nullptr
	};

	pid_t processID{};

	const int spawnError = posix_spawnp(
		&processID,
		executable.c_str(),
		nullptr,
		nullptr,
		args,
		environ
	);

	if (spawnError != 0) {
		std::cerr << "Failed to launch Python: "
			<< std::strerror(spawnError) << '\n';
		return 1;
	}

	int status{};
	pid_t result{};

	do {
		result = waitpid(processID, &status, 0);
	} while (result == -1 && errno == EINTR);

	if (result == -1) {
		std::perror("Failed to wait for Python");
		return 1;
	}

	if (WIFEXITED(status)) {
		return WEXITSTATUS(status);
	}

	if (WIFSIGNALED(status)) {
		std::cerr << "Python terminated by signal "
			<< WTERMSIG(status) << '\n';
		return 1;
	}

	return 1;

#else

#error Unsupported operating system

#endif
}
#endif