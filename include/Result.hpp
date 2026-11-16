#include <iostream>
#include <iomanip>
#include <array>
#include <tuple>
#include <fstream>
#include <string_view>
#include <filesystem>
#include <chrono>
#include "Utilities.hpp"
#include "Option.hpp"


#ifndef RESULT_HPP
#define RESULT_HPP

enum class Mode {
    Default = 1,
    Expirement,
    Testing,
};

class Result {
private:
    double delta{}, gamma{}, theta{}, vega{}, rho{};
    double price_default{};
    OptionStyle style{};
    bool exp = false;

    std::array<std::tuple<double, double, double>, 5> price_time_error_monteC{};
    std::array<std::pair<double, double>, 5> price_time_FDM{};
    std::array<std::pair<double,double> , 5> price_time_binomial{};
    double reference_Eu{};
    double reference_Amer{};

    std::tuple<double, double, double> monteCarlo_Eu(const Option& option, int paths);
    std::pair<double, double> FDM_Eu(const Option& option, int steps);
    std::pair<double, double> binomial_Eu(const Option& option, int steps);
    double closedForm_Eu(const Option& option);
    void defaultPricer_Eu(const Option& option);


    std::pair<double, double> binomial_Amer(const Option& option, int steps);
    std::pair<double, double> FDM_Amer(const Option& option, int steps);
    void defaultPricer_Amer(const Option& option, int steps = 20000);

    std::tuple<double, double, double> monteCarlo_Asian(const Option& option, int paths);
    void defaultPricer_Asian(const Option& option, int paths = 5000000);

public:
    Result() = default;
    Result(const Option& option, Mode mode, bool base_case = false) : exp{ mode == Mode::Expirement }, style{ option.style() } {
        if (mode == Mode::Expirement) {
            for (int i = 0; i < 5; ++i) {
                if (option.style() == OptionStyle::Euro) {
                    price_time_error_monteC[i] = monteCarlo_Eu(option, static_cast<int>(std::pow(10, i + 2)));
                    price_time_FDM[i] = FDM_Eu(option, 400 * (i + 1));
                    price_time_binomial[i] = binomial_Eu(option, 4000 * (i + 1));
                }
                else if (option.style() == OptionStyle::American) {
                    price_time_FDM[i] = FDM_Amer(option, 400 * (i + 1));
                    price_time_binomial[i] = binomial_Amer(option, 4000 * (i + 1));
                }
                else if (option.style() == OptionStyle::Asian) {
                    price_time_error_monteC[i] = monteCarlo_Asian(option, static_cast<int>(std::pow(10, i + 2)));
                }
            }
            if (option.style() == OptionStyle::Euro) {
                if (base_case) defaultPricer_Eu(option);
                else reference_Eu = closedForm_Eu(option);
            }
            else if (option.style() == OptionStyle::American) {
                if (base_case) defaultPricer_Amer(option);
                else reference_Amer = binomial_Amer(option, 20000).first;
            }
            else {
                if (base_case) defaultPricer_Asian(option);
            }
        }
        if (mode == Mode::Testing) {
            if (option.style() == OptionStyle::Euro) {
                price_time_error_monteC[4] = monteCarlo_Eu(option, static_cast<int>(std::pow(10, 4 + 2)));
                price_time_FDM[4] = FDM_Eu(option, 2000);
                price_time_binomial[4] = binomial_Eu(option, 20000);
                price_default = closedForm_Eu(option);
            }
            else if (option.style() == OptionStyle::American) {
                price_time_FDM[4] = FDM_Amer(option, 4000);
                price_time_binomial[4] = binomial_Amer(option, 20000);
                price_default = binomial_Amer(option, 20000).first;
            }
            else if (option.style() == OptionStyle::Asian) {
                price_time_error_monteC[4] = monteCarlo_Asian(option, 1000000);
                price_default = std::get<0>(monteCarlo_Asian(option, 1000000));
            }
        }
        if (mode == Mode::Default) {
            if (option.style() == OptionStyle::Euro) {
                defaultPricer_Eu(option);
            }
            else if (option.style() == OptionStyle::American) {
                defaultPricer_Amer(option);
            }
            else {
                defaultPricer_Asian(option);
            }
        }
    }
    void Print() const {
        if (!exp) {
            std::cout << "Delta: " << delta << '\t' << "Gamma: " << gamma << '\n'
                << "Theta: " << theta << '\t' << "Vega: " << vega << '\n'
                << "Rho: " << rho << "\nPrice: " << price_default << '\n';
            return;
        }

        if (style == OptionStyle::Euro || style == OptionStyle::Asian) {
            std::cout << "Monte-Carlo approximation: \n";
            std::cout << std::left 
                << std::setw(10) << "Paths" 
                << std::setw(10) << "Price" 
                << std::setw(10) << "Runtime" 
                << std::setw(10) << "Standard error";
            if (style == OptionStyle::Euro) {
                std::cout 
                    << std::setw(10) << "Absolute error" << "\n";
            }
            else { std::cout << "\n"; }

            for (int i = 0; i < 5; ++i) {
                std::cout 
                    << std::setw(10) << static_cast<int>(std::pow(10, i + 2)) 
                    << std::setw(10) << std::fixed << std::get<0>(price_time_error_monteC[i]) 
                    << std::setw(10) << std::setprecision(3) << std::get<1>(price_time_error_monteC[i]) 
                    << std::setw(10) << std::defaultfloat << std::get<2>(price_time_error_monteC[i]);
                if (style == OptionStyle::Euro) {
                    std::cout 
                        << std::setw(10) << std::abs(std::get<0>(price_time_error_monteC[i]) - reference_Eu) << "\n";
                }
                else { std::cout << "\n"; }
            }
        }

        if (style == OptionStyle::Euro || style == OptionStyle::American) {
            std::cout << "\nFDM approximation: \n";
            std::cout << std::left 
                << std::setw(10) << "Steps" 
                << std::setw(10) << "Price" 
                << std::setw(10) << "Runtime" 
                << std::setw(10) << "Absolute error" << "\n";
            for (int i = 0; i < 5; ++i) {
                std::cout 
                    << std::setw(10) << 400 * (i + 1) 
                    << std::setw(10) << std::fixed << price_time_FDM[i].first 
                    << std::setw(10) << std::setprecision(3) << price_time_FDM[i].second;
                if (style == OptionStyle::Euro) {
                    std::cout 
                        << std::setw(10) << std::defaultfloat << std::abs(price_time_FDM[i].first - reference_Eu) << "\n";
                }
                else {
                    std::cout 
                        << std::setw(10) << std::defaultfloat << std::abs(price_time_FDM[i].first - reference_Amer) << "\n";
                }
            }

            std::cout << "\nBinomial approximation: \n";
            std::cout << std::left 
                << std::setw(10) << "Steps" 
                << std::setw(10) << "Price" 
                << std::setw(10) << "Runtime" 
                << std::setw(10) << "Absolute error";
            for (int i = 0; i < 5; ++i) {
                std::cout 
                    << std::setw(10) << 3000 * (i + 2) 
                    << std::setw(10) << std::fixed << price_time_binomial[i].first 
                    << std::setw(10) << std::setprecision(3) << price_time_binomial[i].second;
                if (style == OptionStyle::Euro) {
                    std::cout 
                        << std::setw(10) << std::defaultfloat << std::abs(price_time_binomial[i].first - reference_Eu) << "\n";
                }
                else {
                    std::cout 
                        << std::setw(10) << std::defaultfloat << std::abs(price_time_binomial[i].first - reference_Amer) << "\n";
                }
            }
        }
    }
    int Export(std::string name) const {
        name = name + ".csv";
        if (std::filesystem::exists(name)) {
			std::cout << "File already exists. Do you want to overwrite it? \n1)Yes 2)No: ";
			bool overwrite = inputChoice(1, 2) - 1;
            if (!overwrite) {
                std::cout << "Export cancelled.\n";
                return 1;
            }
            else {
                goto write;
			}
        }
        else {
            goto write;
        }
    write:
        std::ofstream file(name);
        if (!file.is_open()) {
            std::cerr << "Error opening file for writing.\n";
            return 1;
		}
        else {
			file << "Paths,Price,Runtime,Standard error,Absolute error\n";
            if (style == OptionStyle::Euro || style == OptionStyle::Asian) {
                for (int i = 0; i < 5; ++i) {
                    file 
                        << static_cast<int>(std::pow(10, i + 2)) << "," 
                        << std::fixed << std::get<0>(price_time_error_monteC[i]) << "," 
                        << std::setprecision(3) << std::get<1>(price_time_error_monteC[i]) << "," 
                        << std::defaultfloat << std::get<2>(price_time_error_monteC[i]) << ",";
                    if (style == OptionStyle::Euro) {
                        file 
                            << std::abs(std::get<0>(price_time_error_monteC[i]) - reference_Eu) << "\n";
                    }
                    else { file << "\n"; }
                }
			}
            if (style == OptionStyle::Euro || style == OptionStyle::American) {
                file << "\nSteps,Price,Runtime,Absolute error\n";
                for (int i = 0; i < 5; ++i) {
                    file 
                        << 400 * (i + 1) << "," 
                        << std::fixed << price_time_FDM[i].first << "," 
                        << std::setprecision(3) << price_time_FDM[i].second << ",";
                    if (style == OptionStyle::Euro) {
                        file 
                            << std::defaultfloat << std::abs(price_time_FDM[i].first - reference_Eu) << "\n";
                    }
                    else {
                        file 
                            << std::defaultfloat << std::abs(price_time_FDM[i].first - reference_Amer) << "\n";
                    }
                }
                file << "\nSteps,Price,Runtime,Absolute error\n";
                for (int i = 0; i < 5; ++i) {
                    file 
                        << 3000 * (i + 2) << "," 
                        << std::fixed << price_time_binomial[i].first << "," 
                        << std::setprecision(3) << price_time_binomial[i].second << ",";
                    if (style == OptionStyle::Euro) {
                        file 
                            << std::defaultfloat << std::abs(price_time_binomial[i].first - reference_Eu) << "\n";
                    }
                    else {
                        file 
                            << std::defaultfloat << std::abs(price_time_binomial[i].first - reference_Amer) << "\n";
                    }
                }
			}
        }
    }
    double price() const {
        return price_default;
	}
    std::tuple<double, double, double, double, double> greeks() const {
        return { delta, gamma, theta, vega, rho };
	}
    auto monteCarloResults() const {
        return price_time_error_monteC;
    }
    auto FDMResults() const {
        return price_time_FDM;
	}
    auto binomialResults() const {
        return price_time_binomial;
	}
};
#endif