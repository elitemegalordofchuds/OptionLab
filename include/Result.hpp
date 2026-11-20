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
    Experiment,
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
    void defaultPricer_Amer(const Option& option, int steps = 25000);

    std::tuple<double, double, double> monteCarlo_Asian(const Option& option, int paths);
    void defaultPricer_Asian(const Option& option, int paths = 5000000);

public:
    Result() = default;
    Result(const Option& option, Mode mode, bool base_case = false) : exp{ mode == Mode::Experiment }, style{ option.style() } {
        if (mode == Mode::Experiment) {
            if (option.style() == OptionStyle::Euro) {
                if (base_case) defaultPricer_Eu(option);
                else {
                    reference_Eu = closedForm_Eu(option); 
                    monteCarlo_Eu(option, 100000); 
                }
            }
            else if (option.style() == OptionStyle::American) {
                if (base_case) defaultPricer_Amer(option);
                else {
                    if (option.type() == OptionType::Call) reference_Amer = closedForm_Eu(option);
                    else reference_Amer = binomial_Amer(option, 25000).first;

                }
            }
            else {
                if (base_case) defaultPricer_Asian(option);
                else monteCarlo_Asian(option, 1000000);
            }
            for (int i = 0; i < 5; ++i) {
                if (option.style() == OptionStyle::Euro) {
                    price_time_error_monteC[i] = monteCarlo_Eu(option, static_cast<int>(std::pow(10, i + 2)));
                    price_time_FDM[i] = FDM_Eu(option, 400 * (i + 1));
                    price_time_binomial[i] = binomial_Eu(option, 4000 * (i + 1));
                }
                else if (option.style() == OptionStyle::American) {
                    price_time_FDM[i] = FDM_Amer(option, 800 * (i + 1));
                    price_time_binomial[i] = binomial_Amer(option, 4000 * (i + 1));
                }
                else if (option.style() == OptionStyle::Asian) {
                    price_time_error_monteC[i] = monteCarlo_Asian(option, static_cast<int>(std::pow(10, i + 2)));
                }
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
                price_time_binomial[4] = binomial_Amer(option, 25000);
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
            std::cout << std::fixed << "Delta: " << delta << '\t' << "Gamma: " << gamma << '\n'
                << "Theta: " << theta << '\t' << "Vega: " << vega << '\n'
                << "Rho: " << rho << "\nPrice: " << price_default << '\n';
            return;
        }

        if (style == OptionStyle::Euro || style == OptionStyle::Asian) {
            std::cout << "Monte-Carlo approximation:\n";
            std::cout << std::left
                << std::setw(20) << "Paths"
                << std::setw(20) << "Price"
                << std::setw(20) << "Runtime/ms"
                << std::setw(20) << "Standard error"
                << std::setw(20) << (style == OptionStyle::Euro ? "Absolute error" : "") << "\n";
            for (int i = 0; i < 5; ++i) {
                std::cout
                    << std::setw(20) << static_cast<int>(std::pow(10, i + 2))
                    << std::setw(20) << std::fixed << std::get<0>(price_time_error_monteC[i])
                    << std::setw(20) << std::setprecision(3) << std::get<1>(price_time_error_monteC[i]) << std::setprecision(6)
                    << std::setw(20) << std::defaultfloat << std::scientific << std::get<2>(price_time_error_monteC[i]);
                if (style == OptionStyle::Euro) {
                std::cout 
                    << std::setw(20) << std::abs(std::get<0>(price_time_error_monteC[i]) - reference_Eu);
                }

                std::cout 
                    << std::defaultfloat << "\n";
            }
        }
        if (style == OptionStyle::Euro || style == OptionStyle::American) {
            std::cout << "\nFDM approximation: \n";
            std::cout << std::left 
                << std::setw(20) << "Steps" 
                << std::setw(20) << "Price" 
                << std::setw(20) << "Runtime/ms" 
                << std::setw(20) << "Absolute error" << "\n";
            for (int i = 0; i < 5; ++i) {
                std::cout
                    << std::setw(20) << (style == OptionStyle::American ? 800 : 400) * (i + 1)
                    << std::setw(20) << std::fixed << price_time_FDM[i].first
                    << std::setw(20) << std::setprecision(3) << price_time_FDM[i].second << std::setprecision(6)
                    << std::setw(20) << std::defaultfloat << std::scientific << (style == OptionStyle::Euro ? std::abs(price_time_FDM[i].first - reference_Eu) : std::abs(price_time_FDM[i].first - reference_Amer)) << std::defaultfloat << "\n";
            }

            std::cout << "\nBinomial approximation: \n";
            std::cout << std::left 
                << std::setw(20) << "Steps" 
                << std::setw(20) << "Price" 
                << std::setw(20) << "Runtime/ms" 
                << std::setw(20) << "Absolute error" << "\n";
            for (int i = 0; i < 5; ++i) {
                std::cout
                    << std::setw(20) << 4000 * (i + 1)
                    << std::setw(20) << std::fixed << price_time_binomial[i].first
                    << std::setw(20) << std::setprecision(3) << price_time_binomial[i].second << std::setprecision(6)
                    << std::setw(20) << std::defaultfloat << std::scientific << (style == OptionStyle::Euro ? std::abs(price_time_binomial[i].first - reference_Eu) : std::abs(price_time_binomial[i].first - reference_Amer)) << std::defaultfloat << "\n";
            }
        }
    }
    int Export(std::string name) const {
        std::error_code ec{};
        std::filesystem::path path = std::filesystem::path(OPTIONLAB_PROJECT_DIR) / "results" / name;
        bool exists = !std::filesystem::create_directory(std::filesystem::path(OPTIONLAB_PROJECT_DIR) / "results" / name, ec);
        if (ec) {
            std::cerr << "Error creating results folder: " << ec.message() << "\n";
            return 1;
        }
        if (exists) {
            std::cerr << "Error, file directory already exists.\n";
            return 1;
        }
        if (style == OptionStyle::Euro || style == OptionStyle::American) {
            std::ofstream file1{ path / (style == OptionStyle::Euro ? "European_Binomial.csv" : "American_Binomial.csv") };
            if (!file1.is_open()) {
                std::cerr << "Error creating " << (style == OptionStyle::Euro ? "European_binomial.csv" : "American_binomial.csv") << " results file\n";
                return 1;
            }
            else {
                file1 << "Steps,Price,Runtime/ms,Absolute error\n";
                for (int i{}; i < 5; ++i) {
                    file1
                        << 4000 * (i + 1) << ","
                        << std::fixed << price_time_binomial[i].first << ","
                        << std::setprecision(3) << price_time_binomial[i].second << std::setprecision(6) << ","
                        << std::defaultfloat << std::scientific << std::abs(price_time_binomial[i].first - (style == OptionStyle::Euro ? reference_Eu : reference_Amer)) << std::defaultfloat << "\n";
                }
            }
            file1.close();
            if (!file1) {
                std::cerr << "Error while writing to " << (style == OptionStyle::Euro ? "European_binomial.csv" : "American_binomial.csv") << " results file\n";
                return 1;
            }
            std::ofstream file2{ path / (style == OptionStyle::Euro ? "European_FDM.csv" : "American_FDM.csv") };
            if (!file2.is_open()) {
                std::cerr << "Error creating " << (style == OptionStyle::Euro ? "European_FDM.csv" : "American_FDM.csv") << " results file\n";
                return 1;
            }
            else {
                file2 << "Steps,Price,Runtime/ms,Absolute error\n";
                for (int i{}; i < 5; i++) {
                    file2
                        << (style == OptionStyle::Euro ? 400 : 800) * (i + 1) << ","
                        << std::fixed << price_time_FDM[i].first << ","
                        << std::setprecision(3) << price_time_FDM[i].second << std::setprecision(6) << ","
                        << std::defaultfloat << std::scientific << std::abs(price_time_FDM[i].first - (style == OptionStyle::Euro ? reference_Eu : reference_Amer)) << std::defaultfloat << "\n";
                }
            }
            file2.close();
            if (!file2) {
                std::cerr << "Error while writing to " << (style == OptionStyle::Euro ? "European_FDM.csv" : "American_FDM.csv") << " results file\n";
                return 1;
            }
        }
        if (style == OptionStyle::Euro || style == OptionStyle::Asian) {
            std::ofstream file1{ path / (style == OptionStyle::Euro ? "European_MonteCarlo.csv" : "Asian_MonteCarlo.csv") };
            if (!file1.is_open()) {
                std::cerr << "Error creating " << (style == OptionStyle::Euro ? "European_MonteCarlo.csv" : "Asian_MonteCarlo.csv") << " results file\n";
                return 1;
            }
            else {
                file1 << "Paths,Price,Runtime/ms,Standard error" << (style == OptionStyle::Euro ? ",Absolute error\n" : "\n");
                for (int i{}; i < 5; ++i) {
                    file1
                        << static_cast<int>(std::pow(10, i + 2)) << ","
                        << std::fixed << std::get<0>(price_time_error_monteC[i]) << ","
                        << std::setprecision(3) << std::get<1>(price_time_error_monteC[i]) << std::setprecision(6) << ","
                        << std::defaultfloat << std::scientific << std::get<2>(price_time_error_monteC[i])
                        << (style == OptionStyle::Euro ? "," + std::to_string(std::abs(std::get<0>(price_time_error_monteC[i]) - reference_Eu)) + "\n" : "\n") << std::defaultfloat;
                }
                file1.close();
                if (!file1) {
                    std::cerr << "Error while writing to " << (style == OptionStyle::Euro ? "European_MonteCarlo.csv" : "Asian_MonteCarlo.csv") << " results file\n";
                    return 1;
                }
            }
        }
        return 0;
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