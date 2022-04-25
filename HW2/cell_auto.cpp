
#include "mpi.h"
#include <stdio.h>
#include <random>
#include <stdio.h>
#include <iostream>
#include <math.h>

//Design a program in such a way that any kind of rule can be easily inputted into the computations
int rule(int prev, int curr, int next) // rule 110
{
  int sum = prev + curr + next;
  if (prev == 1){
    if (sum == 2) return(1);
    else return (0);
  } else {
      if (sum > 0 ) return(1);
      else return(0);
  }

}

// periodic boundary conditions
void periodic(int * &Batch ,int * &New, int size, int rank, int N){
  int part = N/size;
  for (int i = 0; i < part; i++)
  {
    if (i == 0 && i != part-1 && (i + 1) != part-1)
      New[i] = rule(Batch[part-1], Batch[i], Batch[i+1]);
    else if (i == part-1 && i != 0 && (i - 1) != 0)
      New[i] = rule(Batch[i-1], Batch[i], Batch[0]);
    else if(i == 0 && (i + 1) == part-1)
      New[i] = rule(Batch[part-1], Batch[i], Batch[part-1]);
    else if(i == part-1 && i-1 == 0)
      New[i] = rule(Batch[0], Batch[i], Batch[0]);
    else if(i == 0 && i == part-1)
      New[i] = rule(Batch[i], Batch[i], Batch[i]);
    else
      New[i] = rule(Batch[i-1],Batch[i],Batch[i+1]);
  }
}

// constant boundary conditions
void constant(int * &Batch ,int * &New, int size, int rank, int N){
  int part = N/size;
  for (int i = 0; i < part; i++)
  {
    if (i == 0 && i != part-1 && (i + 1) != part-1)
      New[i] = rule(Batch[part-1], Batch[i], Batch[i+1]);
    else if (i == part-1 && i != 0 && (i - 1) != 0)
      New[i] = rule(Batch[i-1], Batch[i], Batch[0]);
    else if(i == 0 && (i + 1) == part-1)
      New[i] = rule(Batch[part-1], Batch[i], Batch[part-1]);
    else if(i == part-1 && i-1 == 0)
      New[i] = rule(Batch[0], Batch[i], Batch[0]);
    else if(i == 0 && i == part-1)
      New[i] = rule(Batch[i], Batch[i], Batch[i]);
    else
      New[i] = rule(Batch[i-1],Batch[i],Batch[i+1]);
  }
}

void print (int * CellularAutomata, int N){
  for (int i = 0; i < N; i++) printf("%d ", CellularAutomata[i]);
  printf("\n");  
}

int main(int ac, char **av)
{
  int N = 8; 
  int size;
  int rank;
 
  int *CellularAutomata = new int[N];
  int *Batch = new int[N];
  int *New = new int[N];
  int *Result = new int[N];
  
  MPI_Status status;
  MPI_Init(&ac, &av);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  double start = MPI_Wtime();
  if (rank == 0)
  {
    // initialization
    for (int i = 0; i < N; i++){
      if (i % 3) CellularAutomata[i] = 0;
      else CellularAutomata[i] = 1;
      }
    // end of initialization
   
    //printf("Initialize:\n");
    //print(CellularAutomata, N);

  }
  MPI_Scatter(CellularAutomata, N/size, MPI_INT, Batch, N/size, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Scatter(Result, N/size, MPI_INT, New, N/size, MPI_INT,  0, MPI_COMM_WORLD);

  periodic(Batch , New, size, rank, N);
 
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Gather(New, N/size, MPI_INT, Result, N/size, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    //printf("Result:\n");
    //print(Result, N);
  }
 
  MPI_Finalize();
  if (rank ==0){
    double end = MPI_Wtime();
    std::cout << (end-start);
    }
 }