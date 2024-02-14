# Original project darts: 10000
# Original project rounds: 100

# Below for Onur
# mpic++ -o ser_pi_calc ser_pi_calc.cpp -std=c++17 -O3

# Below for HPCC
mpicxx -o ser_pi_calc ser_pi_calc.cpp -std=c++17 -O3

# Check if file exists, if exist delete it
if [ -f "data.csv" ]; then
    rm data.csv
fi

for num_darts in 10000 # 100000 1000000 10000000 100000000 1000000000
do
    for rank_count in 1 2 3 4 # 8 16 32 64
    do
        for num_rounds in 100 # 1000
        do
            echo "Running with $rank_count ranks, $num_darts darts, and $num_rounds rounds"
            # Below for Onur
            # mpirun -np $rank_count ./ser_pi_calc $num_rounds $num_darts 
            mpiexec -n $rank_count ./ser_pi_calc $num_rounds $num_darts
            echo "Finished "

        done
    done
done