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


// Calculate ax + by + cz = d

// #define VERBOSE 1
int COEFFICIENT_TAG = 1;
int NUM_COEFFICIENTS = 3;

int VARIABLE_TAG = 2;
int NUM_VARIABLES = 3;

int CONSTANT_TAG = 3;
int NUM_CONSTANTS = 1;

int DISTRUBITOR_RANK = 0;

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


/**
 * Sends an array of coefficients to a specified destination rank using MPI_Send.
 *
 * @param rank The rank of the calling process.
 * @param dest The rank of the destination process.
 * @param a The array of coefficients to be sent.
 * @param comm The MPI communicator.
 */
void SendCoefficient(int rank, int dest, double* a, MPI_Comm comm) {
    MPI_Error_Check(MPI_Send(a, NUM_COEFFICIENTS, MPI_DOUBLE, dest, COEFFICIENT_TAG, comm));
#ifdef VERBOSE
    printf("[%d]: I send out msg to [%d] rank: <%f>:%d\n", rank, dest, a[0], COEFFICIENT_TAG);
#endif
}

/**
 * Receives the coefficient array from a specific rank using MPI_Recv.
 * 
 * @param rank The rank of the current process.
 * @param src The rank of the source process.
 * @param a Pointer to the coefficient array to be received.
 * @param comm The MPI communicator.
 */
void ReceiveCoefficient(int rank, int src, double* a, MPI_Comm comm) {
    MPI_Status status;
    MPI_Error_Check(MPI_Recv(a, NUM_COEFFICIENTS, MPI_DOUBLE, src, COEFFICIENT_TAG, comm, &status));
#ifdef VERBOSE
    printf("[%d]: I receive msg from [%d] rank guy: <%f>:%d\n", rank, src, a[0], COEFFICIENT_TAG);
#endif
}


/**
 * Sends an array of variables to a specified destination rank using MPI_Send.
 * 
 * @param rank The rank of the calling process.
 * @param dest The rank of the destination process.
 * @param a The array of variables to be sent.
 * @param comm The MPI communicator.
 */
void SendVariables(int rank, int dest, double* a, MPI_Comm comm) {
    MPI_Error_Check(MPI_Send(a, NUM_VARIABLES, MPI_DOUBLE, dest, VARIABLE_TAG, comm));
#ifdef VERBOSE
    printf("[%d]: I send out msg to [%d] rank: <%f>:%d\n", rank, dest, a[0], VARIABLE_TAG);
#endif
}

/**
 * @brief Receives variables from a specific rank using MPI_Recv.
 * 
 * This function receives variables of type double from a specific rank using MPI_Recv.
 * The received variables are stored in the array 'a'.
 * 
 * @param rank The rank of the current process.
 * @param src The rank of the source process from which to receive the variables.
 * @param a Pointer to the array where the received variables will be stored.
 * @param comm The MPI communicator.
 */
void ReceiveVariables(int rank, int src, double* a, MPI_Comm comm) {
    MPI_Status status;
    MPI_Error_Check(MPI_Recv(a, NUM_VARIABLES, MPI_DOUBLE, src, VARIABLE_TAG, comm, &status));

#ifdef VERBOSE
    printf("[%d]: I receive msg from [%d] rank guy: <%f, %f, %f>:%d\n", rank, src, a[0], a[1], a[2], VARIABLE_TAG);
#endif
}

/**
 * Sends an array of constants to a specified destination rank using MPI_Send.
 * 
 * @param rank The rank of the calling process.
 * @param dest The rank of the destination process.
 * @param a The array of constants to be sent.
 * @param comm The MPI communicator.
 */
void SendConstant(int rank, int dest, double* a, MPI_Comm comm) {
    MPI_Error_Check(MPI_Send(a, NUM_CONSTANTS, MPI_DOUBLE, dest, CONSTANT_TAG, comm));
#ifdef VERBOSE
    printf("[%d]: I send out msg to [%d] rank: <%f>:%d\n", rank, dest, a[0], CONSTANT_TAG);
#endif
}

/**
 * Receives constants from a specific source rank using MPI communication.
 * If more than one constant is received, it takes the mean of the received constants.
 *
 * @param rank The rank of the current process.
 * @param src The source rank from which the constants are received.
 * @param a Pointer to the array where the received constants will be stored.
 * @param comm The MPI communicator.
 */
void ReceiveConstant(int rank, int src, double* a, MPI_Comm comm) {
    MPI_Status status;
    // Check how many constants are received, if more than 1, then take the mean
    
    MPI_Error_Check(MPI_Probe(src, CONSTANT_TAG, comm, &status));
    int count;
    MPI_Error_Check(MPI_Get_count(&status, MPI_DOUBLE, &count));

    if (count != NUM_CONSTANTS) {

        printf("[%d] Received %d constants, expected %d, taking mean\n", rank, count, NUM_CONSTANTS);        
        double temp[count];
        MPI_Error_Check(MPI_Recv(temp, count, MPI_DOUBLE, src, CONSTANT_TAG, comm, &status));
        double sum = 0;
        printf("[%d]\t[", rank);
        for (int i = 0; i < count; i++) {
            sum += temp[i];
            printf("[%d] %f, ", rank, temp[i]);
        }
        printf("]\n");

        a[0] = sum / count;
        delete temp;
        return;
    }     

    MPI_Error_Check(MPI_Recv(a, NUM_CONSTANTS, MPI_DOUBLE, src, CONSTANT_TAG, comm, &status));


#ifdef VERBOSE
    printf("[%d]: I receive msg from [%d] rank guy: <%f>:%d\n", rank, src, a[0], CONSTANT_TAG);
#endif
}

/**
 * Calculates the distance between a point and a plane in 3D space.
 * 
 * @param a The x-coordinate of the plane's normal vector.
 * @param b The y-coordinate of the plane's normal vector.
 * @param c The z-coordinate of the plane's normal vector.
 * @param x The x-coordinate of the point.
 * @param y The y-coordinate of the point.
 * @param z The z-coordinate of the point.
 * @param d The constant term in the plane equation.
 * @return The distance between the point and the plane.
 */
double calcPlaneDist(double a, double b, double c, double x, double y, double z, double d) {
    return (a*x + b*y + c*z - d) / sqrt(a*a + b*b + c*c);
}
/**
 * Handles the computation for a specific rank in the MPI program.
 * Receives variables, coefficients, and a constant from the distributor rank.
 * Calculates the distance using the received variables, coefficients, and constant.
 * Sends the calculated distance to rank 4.
 *
 * @param rank The rank of the current process.
 * @param comm The MPI communicator.
 */
void handleRank(int rank, MPI_Comm comm) {
    double a[3];
    double x[3];
    double d[1];

    ReceiveVariables(rank, DISTRUBITOR_RANK, x, comm);
    ReceiveCoefficient(rank, DISTRUBITOR_RANK, a, comm);
    ReceiveConstant(rank, DISTRUBITOR_RANK, d, comm);

    #ifdef VERBOSE
    printf("[%d] Received Variables(x,y,z): %f, %f, %f\n",rank, x[0], x[1], x[2]);
    printf("[%d] Received Coeffs(a,b,c): %f, %f, %f\n",rank, a[0], a[1], a[2]);
    printf("[%d] Received Constant(d): %f\n",rank, d[0]);
    #endif

    auto distance = calcPlaneDist(a[0], a[1], a[2], x[0], x[1], x[2], d[0]);

    SendConstant(rank, 4, &distance, comm);
}

/**
 * @brief The main function of the MPI program.
 * 
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return int The exit status of the program.
 */
int main(int argc, char **argv) 
{
    MPI_Init(&argc,&argv);
    MPI_Status Stat;
    int dest;
    int source;
    int tag = 1;
    int rc;
    int count;
    int numtasks;
    int rank;

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);


    // Create double arrays
    double x[3] = {1.13, 2.13, 3.13};
    double y[3] = {4.13, 5.13, 6.13};
    double z[3] = {7.13, 8.13, 9.13};

    double a[3] = {1.0, 2.0, 3.0};
    double b[3] = {4.0, 5.0, 6.0};
    double c[3] = {7.0, 8.0, 9.0};
    
    double d[1] = {10.0};


    if(rank == 0) {

        // Send the variable packets to the other ranks

        for (int i = 0; i < 3; i++) {
            double coeffs_dst[3] = {a[i], b[i], c[i]};
            double vars_dst[3] = {x[i], y[i], z[i]};
            double consts_dst[1] = {d[i]};
            SendCoefficient(rank, i+1, coeffs_dst, MPI_COMM_WORLD);
            SendVariables(rank, i+1, vars_dst, MPI_COMM_WORLD);
            SendConstant(rank, i+1, consts_dst, MPI_COMM_WORLD);
        }

    }  else if (rank == 4) {
        std::vector<double> distances;
        for (int i = DISTRUBITOR_RANK+1; i < numtasks-1; i++) {
            double d[1];
            ReceiveConstant(rank, i, d, MPI_COMM_WORLD);
            distances.push_back(d[0]);
        }

        for (int i = 0; i < distances.size(); i++) {
            printf("[%d] Distance from rank %d: %f\n", rank, i+1, distances[i]);
        }



    } else{
        handleRank(rank, MPI_COMM_WORLD);  
    }


    MPI_Finalize();

    // printf("End of MPI program\n");
    return 0;
}
