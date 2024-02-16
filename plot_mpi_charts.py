import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# FILENAME = "data.csv"
FILENAME = "data_readme_part4_q4.csv"
SAME_ROUND = "SAME ROUND FOR EACH PROCESS"
DIVIDE_ROUND = "DIVIDE ROUND AMONG PROCESSES"
TRUE_PI = 3.141592653589793
df = pd.read_csv(FILENAME)

def plot_pi_error_for_darts_used(run_type):
    df = pd.read_csv(FILENAME)
    df_run_type = df[df['run_type'] == run_type]
    df_run_type['error'] = abs(df_run_type['pi_est'] - TRUE_PI)
    grouped = df_run_type.groupby('ranks')
    plt.figure(figsize=(10, 6))
    marker = None

    for name, group in grouped:
        # TODO: total_darts or darts_per_task?
        if group['total_darts'].iloc[0] == 1000 or group['total_darts'].iloc[0] == 1000000:
            marker = 'o'
        plt.loglog(group['total_darts'], group['error'], label=f"Ranks: {name}", marker=marker)

    plt.title("Errors in Computed Values of Pi: " + run_type)
    plt.xlabel("Total Darts Used to Compute Pi")
    plt.ylabel("Error in Computed Value of Pi")
    plt.legend(title="Processor Count (Ranks)")
    plt.grid(True)
    if run_type == "SAME ROUND FOR EACH PROCESS":
        plt.savefig('plots/pi_error_for_darts_used_same_round.png')
    else:
        plt.savefig('plots/pi_error_for_darts_used_divided_round.png')

def plot_runtime_versus_processor_count(run_type, total_darts):
    plt.figure(figsize=(10, 6))
    
    # Filter the data for the specified run type and total darts
    df_run_type_darts = df[(df['run_type'] == run_type) & (df['total_darts'] == total_darts)]
    
    plt.plot(df_run_type_darts['ranks'], df_run_type_darts['time_taken'], label=f"{total_darts} Darts", marker='o')

    actual_runtimes = df_run_type_darts.groupby('total_darts')['time_taken'].sum().values
    ideal_scaling_line = df_run_type_darts['time_taken'].iloc[0] / df_run_type_darts['ranks']
    ideal_runtimes = ideal_scaling_line.sum()
    efficiency = calculate_scaling_efficiency(actual_runtimes, ideal_runtimes)
    print(f"Run Type: {run_type}, Total Darts: {total_darts}, Overall Efficiency: {efficiency[0]:.2f}%")

    plt.plot(df_run_type_darts['ranks'], ideal_scaling_line, '--', label=f"Ideal Scaling ({total_darts} Darts)")

    plt.title(f"{total_darts} Darts Runtime vs. Processor Count: {run_type}")
    plt.xlabel("Processor Count (Ranks)")
    plt.ylabel("Runtime (seconds)")
    plt.legend()
    plt.grid(True)
    if run_type == "SAME ROUND FOR EACH PROCESS":
        plt.savefig('plots/pi_runtime_versus_processor_count_' + str(total_darts) + '_darts_same_round.png')
    else:
        plt.savefig('plots/pi_runtime_versus_processor_count_' + str(total_darts) + '_darts_divided_round.png')

def compute_convergence_rate(x, y):
    log_x = np.log(x)
    log_y = np.log(y)
    slope, _ = np.polyfit(log_x, log_y, 1)
    return slope

def print_pi_convergence_rate_for_darts_used(run_type):
    df_run_type = df[df['run_type'] == run_type]
    df_run_type['error'] = abs(df_run_type['pi_est'] - TRUE_PI)
    grouped = df_run_type.groupby('ranks')
    
    print("Convergence Rate for each Processor Count [" + run_type + "]:")
    for name, group in grouped:
        x = group['total_darts']
        y = group['error']
        convergence_rate = compute_convergence_rate(x, y)
        print(f"Processor Count (Ranks): {name}, Convergence Rate: {convergence_rate}")


def calculate_scaling_efficiency(actual_runtimes, ideal_runtimes):
    efficiencies = (ideal_runtimes / actual_runtimes) * 100
    return efficiencies

if __name__ == "__main__":
    # plot_pi_error_for_darts_used(SAME_ROUND)
    # plot_pi_error_for_darts_used(DIVIDE_ROUND)
    # print_pi_convergence_rate_for_darts_used(SAME_ROUND)
    # print_pi_convergence_rate_for_darts_used(DIVIDE_ROUND)

    dart_counts = df['total_darts'].unique()
    run_types = df['run_type'].unique()

    for run_type in run_types:
        for total_darts in dart_counts:
            plot_runtime_versus_processor_count(run_type, total_darts)
