#include <random>
#include <thread>

#ifndef RNG
#define RNG
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
#endif