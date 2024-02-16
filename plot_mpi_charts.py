import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# FILENAME = "data.csv"
FILENAME = "data_readme_part4_q4.csv"
SAME_ROUND = "SAME ROUND FOR EACH PROCESS"
DIVIDE_ROUND = "DIVIDE ROUND AMONG PROCESSES"
TRUE_PI = 3.141592653589793

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


def plot_runtime_versus_processor_count():
    # TODO: CHRIS
    pass

def compute_convergence_rate(x, y):
    log_x = np.log(x)
    log_y = np.log(y)
    slope, _ = np.polyfit(log_x, log_y, 1)
    return slope

def print_pi_convergence_rate_for_darts_used(run_type):
    df = pd.read_csv(FILENAME)
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
