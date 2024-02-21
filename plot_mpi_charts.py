import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# FILENAME = "data.csv"
FILENAME = "data/data_readme_part4_q4.csv"
SAME_ROUND = "SAME ROUND FOR EACH PROCESS"
DIVIDE_ROUND = "DIVIDE ROUND AMONG PROCESSES"
DEFAULT_ROUNDS = 100
TRUE_PI = 3.141592653589793
df = pd.read_csv(FILENAME)

def plot_pi_error_for_darts_used(run_type):
    """
    Plot the error in the estimation of pi based on the number of darts used, for a given run type.

    Parameters:
    - run_type: str, the type of run for which the error is being plotted

    Returns:
    - None
    """
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
    """
    Plots the runtime versus processor count for a specified run type and total darts.

    Args:
        run_type (str): The type of run
        total_darts (int): The total number of darts

    Returns:
        None
    """
    plt.figure(figsize=(10, 6))
    
    # Filter the data for the specified run type and total darts
    df_run_type_darts = df[(df['run_type'] == run_type) & (df['total_darts'] == total_darts)]
    
    plt.plot(df_run_type_darts['ranks'], df_run_type_darts['time_taken'], label=f"{total_darts} Darts", marker='o')
    ideal_scaling_line = df_run_type_darts['time_taken'].iloc[0] / df_run_type_darts['ranks']
    speedup = df_run_type_darts['time_taken'].iloc[0] / df_run_type_darts['time_taken']
    if run_type == DIVIDE_ROUND:
        rounds_per_task_factor = DEFAULT_ROUNDS /df_run_type_darts['rounds_per_task']
        ideal_scaling_line = ideal_scaling_line / rounds_per_task_factor
        speedup = df_run_type_darts['time_taken'].iloc[0] / df_run_type_darts['time_taken']
        speedup = speedup / rounds_per_task_factor
        efficiency = (speedup / df_run_type_darts['ranks'])
    else:
        efficiency = speedup / df_run_type_darts['ranks']

    overall_efficiency = efficiency.sum() / df_run_type_darts['ranks'].count()
    rounded_efficiency = round(overall_efficiency, 3)
    print(f"Run Type: {run_type}, Total Darts: {total_darts}, Overall Efficiency: {rounded_efficiency}")

    plt.plot(df_run_type_darts['ranks'], ideal_scaling_line, '--', label=f"Ideal Scaling ({total_darts} Darts)")

    plt.title(f"{total_darts} Darts Runtime vs. Processor Count: {run_type}")
    plt.xlabel("Processor Count (Ranks)")
    plt.ylabel("Runtime (seconds)")
    plt.legend()
    plt.grid(True)
    if run_type == SAME_ROUND:
        plt.savefig('plots/pi_runtime_versus_processor_count_' + str(total_darts) + '_darts_same_round.png')
    else:
        plt.savefig('plots/pi_runtime_versus_processor_count_' + str(total_darts) + '_darts_divided_round.png')

def compute_convergence_rate(x, y):
    """
    Compute the convergence rate of the given data points.

    Parameters:
    x: The input data points for the x-axis.
    y: The input data points for the y-axis.

    Returns:
    float: The computed convergence rate.
    """
    log_x = np.log(x)
    log_y = np.log(y)
    slope, _ = np.polyfit(log_x, log_y, 1)
    return slope

def print_pi_convergence_rate_for_darts_used(run_type):
    """
    Calculate the convergence rate for the given run type.

    Parameters:
    run_type (str): The type of run to calculate the convergence rate for.

    Returns:
    None
    """
    df_run_type = df[df['run_type'] == run_type]
    df_run_type['error'] = abs(df_run_type['pi_est'] - TRUE_PI)
    grouped = df_run_type.groupby('ranks')
    
    print("Convergence Rate for each Processor Count [" + run_type + "]:")
    for name, group in grouped:
        x = group['total_darts']
        y = group['error']
        convergence_rate = compute_convergence_rate(x, y)
        print(f"Processor Count (Ranks): {name}, Convergence Rate: {convergence_rate}")

if __name__ == "__main__":
    plot_pi_error_for_darts_used(SAME_ROUND)
    plot_pi_error_for_darts_used(DIVIDE_ROUND)
    print_pi_convergence_rate_for_darts_used(SAME_ROUND)
    print_pi_convergence_rate_for_darts_used(DIVIDE_ROUND)

    dart_counts = df['total_darts'].unique()
    run_types = df['run_type'].unique()
    for run_type in run_types:
        for total_darts in dart_counts:
            plot_runtime_versus_processor_count(run_type, total_darts)
