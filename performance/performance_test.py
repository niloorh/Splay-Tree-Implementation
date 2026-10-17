import argparse
import csv
import random
import statistics
import subprocess
import time
from pathlib import Path

import matplotlib.pyplot as plt

SIZES = (10_000, 50_000, 100_000, 250_000, 500_000)


def make_workload(distribution, count, seed):
    rng = random.Random(seed)
    seen = set()
    values = []
    lines = [str(count)]
    for _ in range(count):
        if distribution == "uniform":
            value = round(rng.uniform(1, 100))
            if value not in seen:
                seen.add(value)
                values.append(value)
                lines.append(f"add {value}")
            else:
                # The specification searches for a randomly selected existing value.
                lines.append(f"find {rng.choice(values)}")
        else:
            # Gaussian variance = 25, so standard deviation = 5.
            value = round(rng.gauss(50, 5))
            if value not in seen:
                seen.add(value)
                values.append(value)
                lines.append(f"add {value}")
            else:
                lines.append(f"find {value}")
    return ("\n".join(lines) + "\n").encode()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", required=True, type=Path)
    parser.add_argument("--repeats", type=int, default=3)
    parser.add_argument("--sizes", type=int, nargs="+", default=list(SIZES))
    parser.add_argument("--output", type=Path, default=Path("performance/results"))
    parser.add_argument("--seed", type=int, default=2023)
    args = parser.parse_args()
    if args.repeats < 1 or any(n < 10_000 or n > 500_000 for n in args.sizes):
        parser.error("Use at least one repeat and sample sizes from 10000 to 500000")
    executable = args.executable.resolve()
    if not executable.is_file():
        parser.error(f"Executable not found: {executable}")
    args.output.mkdir(parents=True, exist_ok=True)
    records = []
    for distribution in ("uniform", "gaussian"):
        for n in args.sizes:
            times = []
            for repeat in range(args.repeats):
                workload = make_workload(distribution, n, args.seed + repeat)
                start = time.perf_counter()
                completed = subprocess.run([str(executable)], input=workload,
                                           stdout=subprocess.DEVNULL,
                                           stderr=subprocess.PIPE, check=False)
                duration = time.perf_counter() - start
                if completed.returncode != 0:
                    raise RuntimeError(f"C++ program failed: {completed.stderr.decode(errors='replace')}")
                times.append(duration)
            mean = statistics.mean(times)
            records.append((distribution, n, args.repeats, mean, min(times), max(times)))
            print(f"{distribution:8s} n={n:6d}: {mean:.6f} s (mean of {args.repeats})")

    with (args.output / "benchmark_results.csv").open("w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(("distribution", "operations", "repeats", "mean_seconds", "min_seconds", "max_seconds"))
        writer.writerows(records)

    for distribution in ("uniform", "gaussian"):
        subset = [row for row in records if row[0] == distribution]
        plt.plot([row[1] for row in subset], [row[3] for row in subset], marker="o", label=distribution.title())
    plt.xlabel("Number of operations")
    plt.ylabel("Mean execution time (seconds)")
    plt.title("Splay Tree Performance")
    plt.legend()
    plt.grid(alpha=0.3)
    plt.tight_layout()
    plt.savefig(args.output / "performance_comparison.png", dpi=180)
    plt.close()


if __name__ == "__main__":
    main()
