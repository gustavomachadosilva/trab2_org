from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt

EXCEL_FILE = "sim_data.xlsx"
OUTPUT_DIR = Path("graphs")

BENCHMARKS = [
    "bfs",
    "convolution",
    "mergesort"
]

CONFIG_NAMES = {
    "config1": "L1D_size",
    "config2": "fetchToDecodeDelay",
    "config3": "numPhysIntRegs",
    "config4": "L1D_assoc",
    "config5": "L1D_responseLatency"
}


def parse_filename(filename):
    name = filename.replace(".txt", "")

    benchmark = None

    for b in BENCHMARKS:
        if name.startswith(b):
            benchmark = b
            break

    config = name[len(benchmark) + 1:]

    return benchmark, config


def load_data():
    df = pd.read_excel(EXCEL_FILE)

    parsed = df["File"].apply(parse_filename)

    df["benchmark"] = parsed.apply(lambda x: x[0])
    df["config"] = parsed.apply(lambda x: x[1])

    df["system.cpu.ipc"] = pd.to_numeric(df["system.cpu.ipc"])
    df["sim_seconds"] = pd.to_numeric(df["sim_seconds"])

    return df


def plot_parameter(df, config_number):

    levels = [
        f"{config_number}_a",
        f"{config_number}_b",
        f"{config_number}_c"
    ]

    parameter_name = CONFIG_NAMES[config_number]

    subset = df[df["config"].isin(levels)]

    plt.figure(figsize=(8, 5))

    for benchmark in BENCHMARKS:

        temp = subset[subset["benchmark"] == benchmark]

        temp = temp.set_index("config").reindex(levels)

        plt.plot(
            ["A", "B", "C"],
            temp["system.cpu.ipc"].to_numpy(),
            marker="o",
            label=benchmark
        )

    plt.title(f"IPC x {parameter_name}")
    plt.xlabel(parameter_name)
    plt.ylabel("IPC")
    plt.grid(True)
    plt.legend()

    plt.tight_layout()

    plt.savefig(
        OUTPUT_DIR / f"{config_number}_ipc.png",
        dpi=300
    )

    plt.close()

    plt.figure(figsize=(8, 5))

    for benchmark in BENCHMARKS:

        temp = subset[subset["benchmark"] == benchmark]

        temp = temp.set_index("config").reindex(levels)

        plt.plot(
            ["A", "B", "C"],
            temp["sim_seconds"].to_numpy(),
            marker="o",
            label=benchmark
        )

    plt.title(f"Tempo x {parameter_name}")
    plt.xlabel(parameter_name)
    plt.ylabel("Tempo (s)")
    plt.grid(True)
    plt.legend()

    plt.tight_layout()

    plt.savefig(
        OUTPUT_DIR / f"{config_number}_time.png",
        dpi=300
    )

    plt.close()


def generate_average_ipc(df):

    rows = []

    for config in CONFIG_NAMES.keys():

        levels = [
            f"{config}_a",
            f"{config}_b",
            f"{config}_c"
        ]

        subset = df[df["config"].isin(levels)]

        averages = (
            subset
            .groupby("config")["system.cpu.ipc"]
            .mean()
        )

        for level, value in averages.items():

            rows.append({
                "Parameter": config,
                "Level": level,
                "Average IPC": value
            })

    pd.DataFrame(rows).to_excel(
        OUTPUT_DIR / "ipc_average.xlsx",
        index=False
    )


def main():

    OUTPUT_DIR.mkdir(exist_ok=True)

    df = load_data()

    for config in CONFIG_NAMES.keys():
        plot_parameter(df, config)

    generate_average_ipc(df)

    print("Done")


if __name__ == "__main__":
    main()