
import csv
import math
import sys
from pathlib import Path
from statistics import median

import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter


GRAPH_TITLES = {
    "European_MonteCarlo": "European Call - Monte Carlo Simulation",
    "European_FDM": "European Call - Finite Difference Method",
    "European_Binomial": "European Call - Binomial Tree",
    "American_FDM": "American Put - Finite Difference Method",
    "American_Binomial": "American Put - Binomial Tree",
    "Asian_MonteCarlo": "Arithmetic Asian Call - Monte Carlo Simulation",
}


def read_experiment(csv_file: Path) -> dict:
    """Read and validate one OptionLab CSV file."""
    with csv_file.open("r", newline="", encoding="utf-8-sig") as stream:
        reader = csv.DictReader(stream)
        rows = list(reader)
        columns = reader.fieldnames or []

    if not rows:
        raise ValueError("the CSV file is empty")

    if "Paths" in columns:
        x_column = "Paths"
        logarithmic_x = True
    elif "Steps" in columns:
        x_column = "Steps"
        logarithmic_x = False
    else:
        raise ValueError("the CSV must contain either a 'Paths' or 'Steps' column")

    required = {x_column, "Price", "Runtime/ms"}
    missing = required.difference(columns)
    if missing:
        raise ValueError(f"missing required column(s): {', '.join(sorted(missing))}")

    def read_column(column_name: str) -> list[float]:
        try:
            values = [float(row[column_name]) for row in rows]
        except (ValueError, TypeError, KeyError) as error:
            raise ValueError(f"invalid data in column '{column_name}': {error}") from error

        if not all(math.isfinite(value) for value in values):
            raise ValueError(f"column '{column_name}' contains NaN or infinity")
        return values

    x = read_column(x_column)
    prices = read_column("Price")
    runtimes = read_column("Runtime/ms")

    if any(value <= 0 for value in x):
        raise ValueError(f"all values in '{x_column}' must be positive")
    if any(value < 0 for value in runtimes):
        raise ValueError("runtime values cannot be negative")

    absolute_errors = None
    standard_errors = None

    if "Absolute error" in columns:
        absolute_errors = read_column("Absolute error")
        if any(value < 0 for value in absolute_errors):
            raise ValueError("absolute errors cannot be negative")

    if "Standard error" in columns:
        standard_errors = read_column("Standard error")
        if any(value < 0 for value in standard_errors):
            raise ValueError("standard errors cannot be negative")

    return {
        "x_column": x_column,
        "logarithmic_x": logarithmic_x,
        "x": x,
        "prices": prices,
        "runtimes": runtimes,
        "absolute_errors": absolute_errors,
        "standard_errors": standard_errors,
    }


def infer_reference_price(prices: list[float], errors: list[float] | None) -> float | None:
    """Recover the constant reference price from rows containing absolute error.

    OptionLab exports |price - reference| rather than the reference itself.
    Each row therefore supplies two possible reference values: price + error
    or price - error. The best common value is selected by minimising the
    residual across all rows. If the rows do not support a consistent value,
    no reference line is drawn.
    """
    if not errors or len(prices) != len(errors):
        return None

    candidates = [candidate for price, error in zip(prices, errors)
                  for candidate in (price + error, price - error)]

    def residual(reference: float) -> float:
        return sum(abs(abs(price - reference) - error)
                   for price, error in zip(prices, errors)) / len(prices)

    best = min(candidates, key=residual)
    best_residual = residual(best)
    tolerance = max(1e-5, abs(best) * 1e-6)

    if best_residual > tolerance:
        return None

    cluster_tolerance = max(2e-6, abs(best) * 2e-7)
    cluster = [candidate for candidate in candidates
               if abs(candidate - best) <= cluster_tolerance]
    return median(cluster) if cluster else best


def configure_resolution_axis(ax, x_column: str, logarithmic_x: bool) -> None:
    ax.set_xlabel("Number of paths" if x_column == "Paths" else "Number of time steps")
    if logarithmic_x:
        ax.set_xscale("log")
        ax.xaxis.set_major_formatter(FuncFormatter(lambda value, _: f"{value:,.0f}"))
    else:
        ax.xaxis.set_major_formatter(FuncFormatter(lambda value, _: f"{value:,.0f}"))


def plot_price(ax, data: dict, reference_price: float | None, title: str) -> None:
    x = data["x"]
    prices = data["prices"]
    standard_errors = data["standard_errors"]

    if standard_errors is not None:
        confidence_half_widths = [1.96 * value for value in standard_errors]
        ax.errorbar(
            x,
            prices,
            yerr=confidence_half_widths,
            marker="o",
            capsize=3,
            label="Estimate (approx. 95% confidence interval)",
        )
    else:
        ax.plot(x, prices, marker="o", label="Numerical estimate")

    if reference_price is not None:
        ax.axhline(
            reference_price,
            linestyle="--",
            label=f"Reference price ({reference_price:.6f})",
        )

    ax.yaxis.set_major_formatter(FuncFormatter(lambda value, _: f"{value:.6f}"))
    ax.set_ylabel("Option price")
    ax.set_title(title)
    ax.grid(True, which="both", alpha=0.3)
    ax.legend()


def plot_runtime(ax, data: dict, title: str) -> None:
    ax.plot(data["x"], data["runtimes"], marker="o")
    ax.yaxis.set_major_formatter(FuncFormatter(lambda value, _: f"{value:.3f}"))
    ax.set_ylabel("Runtime (ms)")
    ax.set_title(title)
    ax.grid(True, which="both", alpha=0.3)


def plot_errors(ax, data: dict, title: str) -> None:
    x = data["x"]
    plotted_any = False

    if data["absolute_errors"] is not None:
        ax.plot(x, data["absolute_errors"], marker="o", label="Absolute error")
        plotted_any = True

    if data["standard_errors"] is not None:
        ax.plot(x, data["standard_errors"], marker="o", label="Standard error")
        plotted_any = True

    if not plotted_any:
        ax.text(0.5, 0.5, "No error data available", ha="center", va="center",
                transform=ax.transAxes)
    else:
        all_errors = []
        if data["absolute_errors"] is not None:
            all_errors.extend(data["absolute_errors"])
        if data["standard_errors"] is not None:
            all_errors.extend(data["standard_errors"])

        if all(value > 0 for value in all_errors):
            ax.set_yscale("log")
        else:
            positive = [value for value in all_errors if value > 0]
            linthresh = min(positive) / 10 if positive else 1e-12
            ax.set_yscale("symlog", linthresh=linthresh)

        ax.legend()

    ax.set_ylabel("Error")
    ax.set_title(title)
    ax.grid(True, which="both", alpha=0.3)


def create_individual_graphs(csv_file: Path, graphs_dir: Path, data: dict,
                             reference_price: float | None, title: str) -> int:
    """Save the three individual plots for one CSV."""
    x_column = data["x_column"]
    logarithmic_x = data["logarithmic_x"]
    stem = csv_file.stem

    fig, ax = plt.subplots(figsize=(8, 5))
    plot_price(ax, data, reference_price, f"{title}: Price convergence")
    configure_resolution_axis(ax, x_column, logarithmic_x)
    fig.tight_layout()
    fig.savefig(graphs_dir / f"{stem}_price.png", dpi=300, bbox_inches="tight")
    plt.close(fig)

    fig, ax = plt.subplots(figsize=(8, 5))
    plot_runtime(ax, data, f"{title}: Runtime")
    configure_resolution_axis(ax, x_column, logarithmic_x)
    fig.tight_layout()
    fig.savefig(graphs_dir / f"{stem}_runtime.png", dpi=300, bbox_inches="tight")
    plt.close(fig)

    fig, ax = plt.subplots(figsize=(8, 5))
    plot_errors(ax, data, f"{title}: Error convergence")
    configure_resolution_axis(ax, x_column, logarithmic_x)
    fig.tight_layout()
    fig.savefig(graphs_dir / f"{stem}_error.png", dpi=300, bbox_inches="tight")
    plt.close(fig)

    return 3


def main() -> int:
    if len(sys.argv) != 2:
        print("Usage: plot_results.py <results_folder>")
        return 1

    folder_name = sys.argv[1]

    if folder_name in {"", ".", ".."} or "/" in folder_name or "\\" in folder_name:
        print("Invalid results folder name.")
        return 1

    project_dir = Path(__file__).resolve().parent.parent
    results_dir = project_dir / "results" / folder_name
    graphs_dir = project_dir / "graphs" / folder_name

    if not results_dir.is_dir():
        print(f"Results folder not found: {results_dir}")
        return 1

    csv_files = sorted(results_dir.glob("*.csv"))
    if not csv_files:
        print(f"No CSV files found in: {results_dir}")
        return 1

    graphs_dir.mkdir(parents=True, exist_ok=True)
    generated = 0

    for csv_file in csv_files:
        print(f"Processing {csv_file.name}...")

        try:
            data = read_experiment(csv_file)
        except (OSError, ValueError) as error:
            print(f"Could not process {csv_file.name}: {error}")
            return 1

        title = GRAPH_TITLES.get(
            csv_file.stem,
            csv_file.stem.replace("_", " "),
        )

        reference_price = infer_reference_price(
            data["prices"],
            data["absolute_errors"],
        )

        if data["absolute_errors"] is not None and reference_price is None:
            print(
                f"Warning: could not reliably infer a reference price from "
                f"{csv_file.name}; its price graph will not include a reference line."
            )

        try:
            generated += create_individual_graphs(
                csv_file, graphs_dir, data, reference_price, title
            )
        except (OSError, ValueError) as error:
            print(f"Could not generate graphs for {csv_file.name}: {error}")
            return 1

        print(f"Generated individual graphs for {csv_file.name}.")

    print(f"\nGraph generation complete. Generated {generated} image(s).")
    print(f"Graphs saved in: {graphs_dir}")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception as error:
        print(f"Graph generation failed: {error}")
        sys.exit(1)
