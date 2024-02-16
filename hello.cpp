#include <iostream>
#include <mpi.h>
using namespace std;

int main(int argc, char **argv) 
{

    cout << "Hello, before Init!" << endl;

    MPI_Init(&argc,&argv);

    cout << "Hello, between Init and Finalize!" << endl;

    MPI_Finalize();

    cout << "Hello, after Finalize!" << endl;

    return 0;
}