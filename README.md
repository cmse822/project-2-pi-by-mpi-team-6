[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=13660700&assignment_repo_type=AssignmentRepo)
# Project 2: Getting Started with MPI

## Due: EOD, 9 February

## Learning goals

In this project you will explore using basic MPI collectives on HPCC. After finishing this project, you should

- understand how to parallelize a simple serial code with MPI,
- understand how to use basic MPI collectives,
- be able to run MPI-parallel applications on HPCC,
- develop your theoretical understanding of key parallel computing concepts such as:
  - functional parallelism, 
  - collective communication, 
  - parallel scaling, and 
  - parallel efficiency.

## Part 1: Warm-up Exercises

As a group, complete the following exercises from [HPSC](../assets/EijkhoutIntroToHPC2020.pdf). 

- Exercise 2.18 -- HUNTER
- Exercise 2.19 -- HUNTER
- Exercise 2.21 -- HUNTER
- Exercise 2.22 -- HUNTER
- Exercise 2.23 -- HUNTER
- Exercise 2.27 -- ONUR

Include your responses to these exercises in your project write-up.

## Part 2: Setup on HPCC

The following is a very quick tutorial on the basics of using HPCC for this class. 

1. Log in to the HPCC gateway:

    ```shell
    ssh <netid>@hpcc.msu.edu
    ```

    Done.

2. Then log in to the AMD-20 cluster from the gateway:

    ```shell
    module load powertools
    amd20
    ```

    Done.

3. The default environment and software stack should be sufficient for this exercise, but if you run into issues compiling and running the code try issuing the following commands.

    ```shell
    module purge
    module load intel/2021a
    ```

    Done.

4. When using the HPCC for development and exercises in this class please do NOT just use the head node, `dev-amd20`. We will swamp the node and no one will get anything done. Instead, request an interactive job using the SLURM scheduler. An easy way to do this is to set up an alias command like so:

    ```shell
    alias devjob='salloc -n 4 --time 1:30:00'
    ```

    Done.

5. Run `devjob`, then to request 4 tasks for 90 minutes. This should be sufficient for most of the stuff we do during class, though for your projects you will at times require more resources. The above `module` and `alias` commands can be added to your `.bashrc` so that they are automatically executed when you log in.

    Done.

## Part 3: MPI Basics

1. Clone your Project 2 repo from GitHub on HPCC.

    Done.

2. In the project directory you will find a simple "Hello World!" source file in C++. Compile and run the code. E.g.,

    ```shell
    g++ hello.cpp
    ./a.out
    ```

    Done. Says "Hello, World!".

3. Now run the executable `a.out` using the MPI parallel run command and explain the output:

    ```shell
    mpiexec -n 4 ./a.out 
    ```

    Output:
    ```
    Hello, World!
    Hello, World!
    Hello, World!
    Hello, World!
    ```

    The output is printed 4 times because you are executing a serial application on 4 processes. Each application runs through the same "Hello World!" output.

4. Add the commands `MPI_Init` and `MPI_Finalize` to your code. Put three different print statements in your code: one before the init, one between init and finalize, and one after the finalize. Recompile and run the executable, both in serial and with `mpiexec`, and explain the output.

    When running the executable in serial, it prints the following:

    ```
    Hello, before Init!
    Hello, between Init and Finalize!
    Hello, after Finalize!
    ```

    This executes the three statements once because it runs the executable on a single process.

    When running the executable in parallel, using `mpiexec -n 4 ./a.out`, it prints the following:

    ```
    Hello, before Init!
    Hello, before Init!
    Hello, before Init!
    Hello, before Init!
    Hello, between Init and Finalize!
    Hello, between Init and Finalize!
    Hello, between Init and Finalize!
    Hello, between Init and Finalize!
    Hello, after Finalize!
    Hello, after Finalize!
    Hello, after Finalize!
    Hello, after Finalize!
    ```

    This output is because the program now runs in parallel, on four separate processes. Each process executes the same code independently.

5. Complete Exercises 2.3, 2.4, and 2.5 in the [Parallel Programing](../assets/EijkhoutParallelProgramming.pdf) book. -- ALL ONUR

## Part 4: Eat Some Pi

Pi is the ratio of a circle's circumference to its diameter. As such, the value of pi can be computed as follows. Consider a circle of radius `r` inscribed in a square of side length `r`. Randomly generate points within the square. Determine the number of points in the square that are also in the circle. If `f=nc/ns` is the number of points in the circle divided by the number of points in the square then `pi` can be approximated as `pi ~ 4f`. Note that the more points generated, the better the approximation.

1. Look at the C program `ser_pi_calc`. Extend this program using collective MPI routines to compute `pi` in parallel using the method described above. Feel free to use C++, if you prefer, of course.

    Done.

2. For the first iteration, perform the same number of "rounds" on each MPI rank. Measure the total runtime using `MPI_WTIME()`. Vary the number of ranks used from 1 to 4. How does the total runtime change?

    Below is the table displaying the total runtime results for each rank (1 through 4). Keep in mind, we use the default **10,000 darts** and **100 rounds**. We start the first `MPI_WTIME()` before the for loop that performs the pi calculations and start the second `MPI_WTIME()` after the pi calculations and overall average pi is calculated.

    | Ranks | Total Runtime (seconds)   |
    |-------|-------------------------- |
    |   1   |   0.0259022               |
    |   2   |   0.0129883               |
    |   3   |   0.00874055              |
    |   4   |   0.00653383              |

    As the number of ranks (processes) goes up, the total runtime drops. This makes sense, as the number of darts per process also as the number of ranks increases. This means less dart board scoring or iterations needed per process, or an overall reduction in cpu time.

3. Now, divide the number of "rounds" up amongst the number of ranks using the appropriate MPI routines to decide how to distribute the work. Again, run the program on 1 to 4 ranks. How does the runtime vary now?

    With dividing the rounds up per process, here is the total runtime results for each rank (1 through 4). Again, we use the default **10,000 darts** and **100 rounds**:

    | Ranks | Total Runtime (seconds)   |
    |-------|-------------------------- |
    |   1   |   0.0259121               |
    |   2   |   0.00648351              |
    |   3   |   0.00290411              |
    |   4   |   0.00163438              |

    Like question 2, as the number of ranks increases, the total runtime drops. Unlike question 2, however, the number of rounds are also split by process. That means for four processes in parallel, each process will only have to do 25 rounds of "throwing darts" instead of the full 100. That is why ranks 2, 3 and 4 are lower in runtime compared to ranks 2, 3 and 4 in question 2 above. Rank 1 is around the same runtime as rank 1 in question 2, and that makes sense. There's only one process running, so that sole process runs 100 rounds just like it does in question 2.

4. Now let's change the number of "darts" and ranks. Use your MPI program to compute `pi` using total numbers of "darts" of 1E3, 1E6, and 1E9\. For each dart count, run your code on HPCC with processor counts of 1, 2, 4, 8, 16, 32, and 64\. Keep track of the resulting value of `pi` and the runtimes. Use non-interactive jobs and modify the `submitjob.sb` script as necessary.

    TODO -- CHRIS

5. For each processor count, plot the resulting errors in your computed values of `pi` compared to the true value as functions of the number of darts used to compute it. Use log-log scaling on this plot. What is the rate of convergence of your algorithm for computing `pi`? Does this value make sense? Does the error or rate of convergence to the correct answer vary with processor count? Should it?

    TODO -- CHRIS
    PLOTTING NEEDED
   
6. For each dart count, make a plot of runtime versus processor count. Each line represents a "strong scaling" study for your code. For each dart count, also plot the "ideal" scaling line. Calculate the parallel scaling efficiency of your code for each dart count. Does the parallel performance vary with dart count? Explain your answer.

    TODO -- CHRIS
    ADDITIONAL PLOTTING NEEDED

7. Going further. Try running your code on different node types on HPCC with varied core counts. In particular, try to ensure that your runs utilize multiple nodes so that the communication network is used. Do you see a change in the communication cost when the job is run on more than one node?

    TODO -- CHRIS

## What to turn-in

To your git project repo, commit your final working code for the above exercises and a concise write-up including responses to the warm-up exercises, performance and accuracy data for your calculations of `pi`, the plots made in Part 4, and detailed responses to the questions posed concerning your results. 



## Project Notes for Those Running It

On HPCC you need to run the following:

```shell
module load GCC/9.3.0
module load intel-compilers/2021.2.0
```

This will allow you to compile:

```shell
mpicxx -o ser_pi_calc ser_pi_calc.cpp -std=c++17 -O3
```

And execute our program:

```shell
mpiexec -n 4 ./ser_pi_calc 100 10000
```
