/***************************************************************************
 * FILE: ser_pi_calc.c
 * DESCRIPTION:  
 *   Serial pi Calculation - C Version
 *   This program calculates pi using a "dartboard" algorithm.  See
 *   Fox et al.(1988) Solving Problems on Concurrent Processors, vol.1
 *   page 207.  
 * AUTHOR: unknown
 * REVISED: 02/23/12 Blaise Barney
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <mpi.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <stdlib.h>

std::vector<std::string> run_types = {"SAME ROUND FOR EACH PROCESS", "DIVIDE ROUND AMONG PROCESSES"};

struct RunData {
   std::string run_type;
   int ranks;
   int total_rounds;
   int rounds_per_task;
   int total_darts;
   int darts_per_task;
   double time_taken;
   double pi_est;
};
void WriteToCSV(RunData data);
void srand (unsigned seed);  
double dboard (int darts);

int DARTS=10000;   	/* number of throws at dartboard */
int ROUNDS=100;    	/* number of times "darts" is iterated */
int THRESHOLD=1;     /* default is 1, e.g. maximum distance from (0,0) to be considered inside the circle */

int main(int argc, char *argv[])
{
double pi;          	/* average of pi after "darts" is thrown */
int n;
MPI_Status Stat;
int dest;
int source;
int rc;
int count;
int numtasks;
int rank;

// Get number of ROUNDS and DARTS from command line
if(argc == 3) {
   ROUNDS = atoi(argv[1]);
   DARTS = atoi(argv[2]);
} else {
   printf("Usage: %s <number of rounds> <number of darts>\n", argv[0]);
   exit(1);
}

MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

if(rank == 0)
   printf("Starting serial version of pi calculation using dartboard algorithm...\n");


// darts / num_tasks = number for each thread = 10000/4 = 2500
// each process wil do 2500 dart throws at board and get average pi
// MPI_SUM average pi's from each process

// SAME ROUND FOR EACH PROCESS
{
   srand (5);                 /* seed the random number generator */
   double avepi=0;       	   /* average pi value for all iterations */
   double overall_avepi = 0;
   
   auto now = MPI_Wtime();

   if(rank == 0){
      printf("=====================================================\n");
      printf("SAME ROUND FOR EACH PROCESSS\n");
      printf("\tDarts per process: %d\n", DARTS / numtasks);
   }

   for (int i = 0; i < ROUNDS; i++) {
      /* Perform pi calculation on serial processor */
      pi = dboard(DARTS / numtasks);
      avepi = ((avepi * i) + pi)/(i + 1); 
   }    

   /* Take avepi from each process and sums to overall_avepi. */
   MPI_Reduce(&avepi, &overall_avepi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

   if(rank==0){
      /* Divide by num processes to get the avepi over all processes  */
      printf("\nFound %f, Real value of PI: 3.1415926535897 \n", overall_avepi/numtasks);
      
      auto time_taken = MPI_Wtime() - now;
      printf("Time taken: %f\n", time_taken);

      printf("FINISHED DIVIDE ROUND AMONG PROCESSES\n");
      printf("=====================================================\n");
      RunData data;
      data.run_type = run_types[0];
      data.ranks = numtasks;
      data.total_rounds = ROUNDS;
      data.rounds_per_task = -1;
      data.total_darts = DARTS;
      data.darts_per_task = DARTS / numtasks;
      data.time_taken = time_taken;
      data.pi_est = overall_avepi/numtasks;
      WriteToCSV(data);
   }

}
// END OF SAME ROUND FOR EACH PROCESS

MPI_Barrier(MPI_COMM_WORLD);

///////////////////////////////
// DIVIDE ROUND AMONG PROCESSES
///////////////////////////////
{
   srand (5);            /* seed the random number generator */

   double avepi= 0;
   double overall_avepi = 0;

   auto now = MPI_Wtime();
   int rounds_per_proc = ROUNDS/numtasks;
   if(rank == 0){
      printf("=====================================================\n");
      printf("DIVIDE ROUND AMONG PROCESSES\n");
      printf("\tRounds per process: %d\n", rounds_per_proc);
      printf("\tDarts per process: %d\n", DARTS / numtasks);

   }

   for (int i = 0; i < rounds_per_proc; i++) {
      pi = dboard(DARTS / numtasks);
      avepi = ((avepi * i) + pi)/(i + 1);
   }
   
   MPI_Reduce(&avepi, &overall_avepi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

   // Create data for CSV
   if(rank==0){

      printf("\nFound %f, Real value of PI: 3.1415926535897 \n", overall_avepi/numtasks);
      
      auto time_taken = MPI_Wtime() - now;
      printf("Time taken: %f\n", time_taken);

      printf("FINISHED DIVIDE ROUND AMONG PROCESSES\n");
      printf("=====================================================\n");
      RunData data;
      data.run_type = run_types[1];
      data.ranks = numtasks;
      data.total_rounds = ROUNDS;
      data.rounds_per_task = rounds_per_proc;
      data.total_darts = DARTS;
      data.darts_per_task = DARTS / numtasks;
      data.time_taken = time_taken;
      data.pi_est = overall_avepi/numtasks;
      WriteToCSV(data);
   }

}
//////////////////////////////////////
// END OF DIVIDE ROUND AMONG PROCESSES
//////////////////////////////////////


   MPI_Finalize();
}


void WriteToCSV(RunData data) {
   std::ofstream file;
   
   // Check if file exists
   if (!std::filesystem::exists("data.csv")) {
      file.open("data.csv");
      file << "run_type,ranks,total_rounds,rounds_per_task,total_darts,darts_per_task,time_taken,pi_est\n";
      file.close();
   }



   file.open("data.csv", std::ios_base::app);
   file <<  data.run_type << "," <<
            data.ranks << "," <<
            data.total_rounds << "," <<
            data.rounds_per_task << "," <<
            data.total_darts << "," <<
            data.darts_per_task << "," <<
            data.time_taken << "," << data.pi_est << "\n";

   file.close();
}

/*****************************************************************************
 * dboard
 *****************************************************************************/
#define sqr(x)	((x)*(x))

double dboard(int darts)
{
   double x_coord,       /* x coordinate, between -1 and 1  */
          y_coord,       /* y coordinate, between -1 and 1  */
          pi,            /* pi  */
          r;             /* random number scaled between 0 and 1  */
   int score,            /* number of darts that hit circle */
       n;
   unsigned int cconst;  /* must be 4-bytes in size */
/*************************************************************************
 * The cconst variable must be 4 bytes. We check this and bail if it is
 * not the right size
 ************************************************************************/
   if (sizeof(cconst) != 4) {
      printf("Wrong data size for cconst variable in dboard routine!\n");
      printf("See comments in source file. Quitting.\n");
      exit(1);
      }
   /* 2 bit shifted to MAX_RAND later used to scale random number between 0 and 1 */
   cconst = 2 << (31 - 1);
   score = 0;

/***********************************************************************
 * Throw darts at board.  Done by generating random numbers
 * between 0 and 1 and converting them to values for x and y
 * coordinates and then testing to see if they "land" in
 * the circle."  If so, score is incremented.  After throwing the
 * specified number of darts, pi is calculated.  The computed value
 * of pi is returned as the value of this function, dboard.
 ************************************************************************/

   for (n = 1; n <= darts; n++) {
      /* generate random numbers for x and y coordinates */
      r = (double)rand()/cconst;
      x_coord = (2.0 * r) - 1.0;
      r = (double)rand()/cconst;
      y_coord = (2.0 * r) - 1.0;

      /* if dart lands in circle, increment score */
      if ((sqr(x_coord) + sqr(y_coord)) <= 1.0)
         score++;
      }

   /* calculate pi */
   pi = 4.0 * (double)score/(double)darts;
   return(pi);
} 
