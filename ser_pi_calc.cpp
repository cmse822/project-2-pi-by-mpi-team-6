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

// SIde note: Reduce VS code version to 1.85 if having trouble using ssh tunnel
void srandom (unsigned seed);  
double dboard (int darts);

#define DARTS 10000   	/* number of throws at dartboard */
#define ROUNDS 100    	/* number of times "darts" is iterated */
#define THRESHOLD 1

int main(int argc, char *argv[])
{
double pi;          	/* average of pi after "darts" is thrown */
double avepi;       	/* average pi value for all iterations */
int i, n;
MPI_Status Stat;
int dest;
int source;
int rc;
int count;
int numtasks;
int rank;

MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

printf("Starting serial version of pi calculation using dartboard algorithm...\n");
srandom (5);            /* seed the random number generator */


avepi = 0;
double overall_avepi = 0;
// darts / num_tasks = number for each thread = 10000/4 = 2500
// each process wil do 2500 dart throws at board and get average pi
// MPI_SUM average pi's from each process
for (i = 0; i < ROUNDS; i++) {
   /* Perform pi calculation on serial processor */
   pi = dboard(DARTS / numtasks);
   avepi = ((avepi * i) + pi)/(i + 1); 
   // MPI_REDUCE HERE WITH MPI_SUM as ARGUMENT
   MPI_Reduce(&avepi, &overall_avepi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
   printf("   After %3d throws, average value of pi = %10.8f\n",
         (DARTS * (i + 1)),avepi);
   }    
printf("\nReal value of PI: 3.1415926535897 \n");
}



/*****************************************************************************
 * dboard
 *****************************************************************************/
#define sqr(x)	((x)*(x))
long random(void);

// double get_norm(double x1, double y1) {
//    return sqrt(sqr(x1) + sqr(y1));
// }


// two vectors
// for loop over one of the vectors, same size
// call get_norm
// start with 0

// int get_total_inner_points(std::vector<int> x1, std::vector<int> y1) {
//    int number_of_points = 0;
//    for (int i = 0; i < x1.size(); i++) {
//       if(get_norm(x1[i], y1[i]) < THRESHOLD) {
//          number_of_points++;
//       }
//    }
//    return number_of_points;
// }

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
      r = (double)random()/cconst;
      x_coord = (2.0 * r) - 1.0;
      r = (double)random()/cconst;
      y_coord = (2.0 * r) - 1.0;

      /* if dart lands in circle, increment score */
      if ((sqr(x_coord) + sqr(y_coord)) <= 1.0)
         score++;
      }

   /* calculate pi */
   pi = 4.0 * (double)score/(double)darts;
   return(pi);
} 
