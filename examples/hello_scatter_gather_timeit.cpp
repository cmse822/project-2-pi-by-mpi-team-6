/**
 * @file hello.cpp
 * @brief A parallel MPI program to calculate the distance of a point from multiple planes.
 * 
 * This program uses MPI (Message Passing Interface) to distribute the calculation of plane distances
 * across multiple processes. The program calculates the distance of a given point from multiple planes
 * defined by their coefficients (a, b, c) and a constant (d) using the formula ax + by + cz = d.
 * 
 * The program consists of the following functions:
 * - SendCoefficient: Sends the coefficients of the plane from one process to another.
 * - ReceiveCoefficient: Receives the coefficients of the plane from another process.
 * - SendVariables: Sends the variables (x, y, z) of the point from one process to another.
 * - ReceiveVariables: Receives the variables (x, y, z) of the point from another process.
 * - SendConstant: Sends the constant (d) of the plane from one process to another.
 * - ReceiveConstant: Receives the constant (d) of the plane from another process.
 * - calcPlaneDist: Calculates the distance of the point from the plane using the given formula.
 * - handleRank: Handles the computation for each process by receiving the variables, coefficients, and constant,
 *   calculating the distance, and sending it back to the main process.
 * - main: The main function that initializes MPI, creates the necessary arrays, and distributes the computation
 *   among the processes.
 * 
 * The program follows a master-worker model, where the main process (rank 0) distributes the variables, coefficients,
 * and constant to the worker processes (rank 1 to n-1). The worker processes calculate the distance and send it back
 * to the main process. The main process then prints the distances received from the worker processes.
 * Running the program:
 * mpic++ -o hello hello.cpp && mpirun -n 5 ./hello
 */
#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>


// Calculate ax + by + cz = d

// #define VERBOSE 1
constexpr int VECTOR_SIZE=10000;
constexpr int MAX_ITER=2;
// MPI Error Check define

// Macro for wrapping MPI calls with error handling
#define MPI_Error_Check(call){ \
    int error = call; \
    if (error != MPI_SUCCESS) { \
        char error_string[MPI_MAX_ERROR_STRING]; \
        int length_of_error_string; \
        MPI_Error_string(error, error_string, &length_of_error_string); \
        fprintf(stderr, "Error at line %d in file %s: %s\n", __LINE__, __FILE__, error_string);\
        MPI_Abort(MPI_COMM_WORLD, error); \
    } \
}

// Random number vector generator
std::vector<int> generateRandomVector(int size) {
    std::vector<int> randomVector;
    for (int i = 0; i < size; i++) {
        randomVector.push_back(rand() % 100);
    }
    return randomVector;
}

template <typename T>
double getMean(std::vector<T> &vec){
    double sum = 0;
    for (int i = 0; i < vec.size(); i++){
        sum += vec[i];
    }
    return sum / vec.size();
}

int main(int argc, char **argv) 
{
    MPI_Init(&argc,&argv);
    MPI_Status Stat;
    int dest;
    int source;
    int tag = 1;
    int rc;

    int numtasks;
    int rank;

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    std::vector<int> randomVector;

    if(rank == 0) {
        printf("[%d] Number of tasks= %d | Running on %s\n\tWill wait a little...\n", rank, numtasks, processor_name);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    int num_child_processes = numtasks;
    int number_of_elements = VECTOR_SIZE / (num_child_processes);
    if(rank == 0) {
        randomVector = generateRandomVector(VECTOR_SIZE);
        printf("[%d] Sending a random vector[%d] to all processes[%d], each has [%d] elements\n", rank, VECTOR_SIZE, num_child_processes, number_of_elements);
    }

    // Time it
    double start;
    if(rank == 0) 
        start = MPI_Wtime();

    std::vector<int> subRandomVector(number_of_elements);
    MPI_Scatter(randomVector.data(), subRandomVector.size(), MPI_INT, subRandomVector.data(), subRandomVector.size(), MPI_INT, 0, MPI_COMM_WORLD);

    double sub_mean = 0;
    std::vector<double> sub_mean_vec(num_child_processes, 0.0f);

    if(rank!=0){
        sub_mean = getMean(subRandomVector);
#ifdef VERBOSE
        printf("[%d] Received a random vector[%ld] from process[%d], Mean: %f\n", rank, subRandomVector.size(), 0, sub_mean);
#endif
    }

    MPI_Gather(&sub_mean, 1, MPI_DOUBLE, sub_mean_vec.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        printf("[%d] Received sub mean vector[%ld] from all processes, discarding master\n", rank, sub_mean_vec.size());

        std::vector<double> trimmed_sub_mean_vec(sub_mean_vec.size()-1);
        for (int i = 1; i < sub_mean_vec.size(); i++){
            trimmed_sub_mean_vec[i-1] = sub_mean_vec[i];
        }
        printf("[");
        for (int i = 0; i < trimmed_sub_mean_vec.size(); i++)
            printf("%f, ", trimmed_sub_mean_vec[i]);
        printf("]\n");
        
        const double mean = getMean(trimmed_sub_mean_vec);
        printf("[%d] The mean of means is %f\n", rank, mean);
    }

    if(rank == 0) {
        printf("[%d] Time elapsed: %f secs\n", rank, MPI_Wtime() - start);
    }
    MPI_Finalize();

    // printf("End of MPI program\n");
    return 0;
}
