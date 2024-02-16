#!/bin/bash
mpicxx -o ser_pi_calc ser_pi_calc.cpp -std=c++17 -O3

# Check if file exists, if exist delete it
if [ -f "data.csv" ]; then
    rm data.csv
fi
# 1E3 1E6 1E9 darts
for num_darts in 1000 1000000 1000000000
do
    # 1 2 4 8 16 32 64 ranks
    for rank_count in 1 2 4 8 16 32 64
    do
        for num_rounds in 100
        do
            echo "Running with $rank_count ranks, $num_darts darts, and $num_rounds rounds"
            mpiexec -n $rank_count ./ser_pi_calc $num_rounds $num_darts
            echo "Finished "
        done
    done
done