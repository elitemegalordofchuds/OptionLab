#include "Types.hpp"
#include <iostream>
#include "Option.hpp"

class BaseResult {
protected:
	double delta{};
	double gamma{};
	double theta{};
	double vega{};
	double rho{};
	double price{};
	void printGreeks() const {
		std::cout << "Delta: " << delta << '\t' << "Gamma: " << gamma << '\n' 
			<< "Theta: " << theta << '\t' << "Vega: " << vega << '\n' 
			<< "Rho: " << rho << '\t' << "Price: " << price << '\n';
	}
};

template <OptionStyle style>
class Result;

template <>
class Result<OptionStyle::Euro> : public BaseResult {
private:
	double price_bs{};
	double price_monteC{};
	double price_grid{};
	void BlackScholes(const Option& option);
	void MonteCarlo_eu(const Option& option);
	void GridSolve_eu(const Option& option);
public:
	Result(Option option) {
		BlackScholes(option);
		MonteCarlo_eu(option);
		GridSolve_eu(option);
	}
	void Print() const {
		printGreeks();
		std::cout << "Price - Black-Scholes (Closed Form): " << price_bs << '\n';
		std::cout << "Price - Monte Carlo (100,000 Paths): " << price_monteC << '\n';
		std::cout << "Price - Black-Scholes (Grid Solve Approximation): " << price_grid << '\n';
	}
};

template <>
class Result<OptionStyle::American> : public BaseResult {
private:
	double price_tree{};
	double price_grid{};
	void GridSolve_Amer(const Option& option);
	void BinaryTree_Amer(const Option& option);
public:
	Result(Option option) {
		GridSolve_Amer(option);
		BinaryTree_Amer(option);
	}
	void Print() const {
		printGreeks();
		std::cout << "Price - Black-Scholes (Grid Solve Approximation): " << price_grid << '\n';
		std::cout << "Price - Black-Scholes (Binary Tree Approximation): " << price_tree << '\n';
	}
};

template <>
class Result<OptionStyle::Asian> : public BaseResult {
private:
	double price_monteC{};
	void MonteCarlo_Asian(const Option& option);
public:
	Result(Option option) {
		MonteCarlo_Asian(option);
	}
	void Print() const{
		printGreeks();
		std::cout << "Price - Monte Carlo (100,000 Paths): " << price_monteC << '\n';
	}
};