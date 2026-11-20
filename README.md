[README.md](https://github.com/user-attachments/files/35187785/README.md)
# OptionLab

OptionLab is a C++20 application I developed to investigate numerical methods used in option pricing. The main objective of this project was not simply to implement several pricing algorithms and obtain option prices. I also wanted to understand how these methods work and examine how their accuracy and computational cost change as the numerical resolution increases.

The application supports European, American, and arithmetic Asian options. It implements analytical and numerical pricing methods, calculates the five main option Greeks, and includes separate testing and experiment modes. The project therefore combines the mathematical foundations of option pricing with their practical implementation and numerical investigation.

## Pricing methods

The available pricing methods depend on the style of option being priced:

- **European options:** the Black–Scholes closed-form formula, Monte Carlo simulation, the binomial tree method, and the finite-difference method.
- **American options:** the binomial tree method and the finite-difference method.
- **Arithmetic Asian options:** Monte Carlo simulation.

The finite-difference method uses the Crank–Nicolson scheme. For American options, the early-exercise condition is formulated as a linear complementarity problem and solved using projected successive over-relaxation (PSOR). The Monte Carlo implementation uses antithetic variates as a variance-reduction technique.

In addition to calculating option prices, the program calculates five Greeks, which describe the sensitivity of the option price to changes in its parameters:

- Delta
- Gamma
- Theta
- Vega
- Rho

## How the application works

OptionLab has three modes, each serving a different purpose.

**Default mode** is used to price an option. The user enters the required option parameters, selects the option style and type, and the program applies the corresponding default pricing method. The resulting option price is displayed alongside its Greeks.

**Experiment mode** is used to investigate the behaviour of the pricing methods. Rather than using only one resolution, the program runs the available methods for the selected option style with several numbers of paths or time steps. It records the resulting prices and runtimes, together with absolute errors and standard errors where these are available. The results can then be exported to CSV files for further analysis.

**Testing mode** runs the testing framework. The tests compare calculated prices and Greeks against reference values and examine several theoretical properties, including convergence and the behaviour of Monte Carlo standard errors. All test suites can be run together, or an individual suite can be selected.

## Experimental analysis and graph generation

One of the main objectives of this project was to investigate how numerical resolution affects the accuracy and computational performance of the implemented methods. For this reason, experiment mode exports its results to CSV files rather than limiting the investigation to the values displayed in the terminal.

The Python script located at `analysis/plot_results.py` reads these CSV files and generates three graphs for each pricing method: price convergence, runtime, and error convergence. The graphs are saved under the corresponding folder in `graphs/`. The script generates the individual graphs used to examine each method; the combined figures used in the technical report were assembled separately.

The graph-generation script requires Python 3 and Matplotlib. The application can launch the script after an experiment has been exported, provided that the required Python installation and dependency are available.

## Building the project

The project is built using CMake and requires a C++20-compatible compiler. The following commands use the Visual Studio 2022 generator on Windows. Run them from the root directory of the repository:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

With this generator, the executable is normally created at `build/Release/OptionLab.exe`.

## Running the application

To start the interactive application on Windows, run:

```powershell
.\build\Release\OptionLab.exe
```

The testing mode can be accessed using the `--test` command-line argument. To run all test suites:

```powershell
.\build\Release\OptionLab.exe --test
```

An individual test suite can also be selected:

```powershell
.\build\Release\OptionLab.exe --test european
.\build\Release\OptionLab.exe --test american
.\build\Release\OptionLab.exe --test asian
```

## Python requirements

The Python dependency used by the graph-generation script is listed in `requirements.txt`. On Windows, it can be installed using:

```powershell
py -3 -m pip install -r requirements.txt
```

The script can also be run directly from the repository's root directory by passing the name of the experiment folder created under `results/`:

```powershell
py -3 analysis\plot_results.py ExperimentFolder
```

For example, if the CSV files are stored in `results/European_Experiment`, replace `ExperimentFolder` with `European_Experiment`. The generated graphs will be saved in `graphs/European_Experiment`.

## Technical report

The technical report discusses the mathematical foundations of the pricing models, the numerical methods used in the application, the implementation and testing framework, and the experimental results in greater detail.

The report will be available here:

(docs/OptionLab_TechnicalReport.pdf)

## Assumptions and limitations

The pricing models are based on the Black–Scholes framework and therefore assume, among other things, constant volatility and a constant risk-free interest rate. The implementation also assumes that the underlying stock pays no dividends. These assumptions simplify the pricing problem but do not capture every feature of real financial markets.

There are also limitations in the numerical methods currently implemented. Arithmetic Asian options are priced using Monte Carlo simulation alone, so the application does not yet provide a comparison between several numerical methods for this option style. The American option experiment uses a high-resolution binomial-tree approximation as its reference price rather than an exact analytical value. For the Asian option, no independent reference price is used, so its experiment examines sampling uncertainty through the standard error rather than absolute pricing error.

Finally, runtime measurements depend on the machine and build configuration used. They should therefore be interpreted as measurements of the tested environment, rather than as universal performance figures.

Possible extensions include implementing additional variance-reduction techniques, adding alternative numerical methods for Asian options, and considering models with more realistic market assumptions.
