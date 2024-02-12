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
constexpr int VECTOR_SIZE=10;
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

double getMean(std::vector<int> &vec){
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
    #include <cstdio> // Add missing include statement

    int numtasks;
    int rank;

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    std::vector<int> randomVector(VECTOR_SIZE);

    if(rank == 0) {
        printf("[%d] Number of tasks= %d | Running on %s\n\tWill wait a little...", rank, numtasks, processor_name);
        sleep(2);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0) {
        
        int iter = 0;

        for (int i=0; i<MAX_ITER; i++){
            std::cout << "[" << rank << "] Sending a random vector[" << i << "] to all processes" << std::endl; // Replace printf with std::cout

            randomVector = generateRandomVector(10);
            MPI_Error_Check(MPI_Bcast(&randomVector[0], VECTOR_SIZE, MPI_INT, 0, MPI_COMM_WORLD));
        }


    } else{
        MPI_Status stat;
        std::vector<int> randomVector(VECTOR_SIZE * MAX_ITER);
        
        for (int i=0; i<MAX_ITER; i++){
            MPI_Error_Check(MPI_Bcast(&randomVector[0], VECTOR_SIZE, MPI_INT, 0, MPI_COMM_WORLD));
            const auto mean = getMean(randomVector);
            printf("[%d] Mean of the vector[%d] is: %f \t[", rank, i, mean);
            for (int i = 0; i < VECTOR_SIZE; i++){
                printf("%d ", randomVector[i]);
            }
            printf("]\n");
        }
        

    }

    MPI_Finalize();

    // printf("End of MPI program\n");
    return 0;
}
