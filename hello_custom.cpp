#include <mpi.h>
#include <iostream>
using namespace std;



int main(int argc, char **argv) 
{
    MPI_Status Stat;
    int dest;
    int source;
    const char* imsg = "Hello dude";
    const char* outmsg = "Hello world";
    int tag = 1;
    int rc;
    int count;
    int numtasks;
    int rank;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    if(rank == 0) {
        dest = 1;
        source = 1;
        rc = MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        cout << "I send out msg to 1 rank: " << outmsg << endl;
	    rc = MPI_Recv(&imsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
    }
    else if (rank == 1) {
        dest = 0;
        source = 0;
        rc = MPI_Recv(&imsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
        cout << "I receive msg from 0 rank guy: " << imsg << endl;
	    rc = MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    }
    //cout << "Hello, World!" << endl;


    MPI_Finalize();
    return 0;
}