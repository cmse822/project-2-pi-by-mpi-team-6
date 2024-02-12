
mpic++ -o ser_pi_calc ser_pi_calc.cpp -std=c++17

# Check if file exists, if exist delete it
if [ -f "data.csv" ]; then
    rm data.csv
fi

for num_darts in 100000 1000000 10000000 100000000 1000000000
do
    for rank_count in 1 2 4 8 16 32 64
    do
        for num_rounds in 100 1000
        do
            echo "Running with $rank_count ranks, $num_darts darts, and $num_rounds rounds"
            mpirun -np $rank_count ./ser_pi_calc $num_rounds $num_darts 
            echo "Finished "

        done
    done
done