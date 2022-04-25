
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "time.h"

// function to pass the ball to different reciever
bool ball_was_passed(int reciever, int* order, int iter_num){
    for (int i = 0; i < iter_num; ++i) {
        if (order[i] == reciever) return true;
    }
    return false;
}

int main(int argc, char ** argv){ 
    
    int rank;
    int size;
    int iter_num = 0;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(0));
    char name = 'a' + rand() % 26;  // random names for processes

    int *order = new int[size];   //opder of processes
    char *names = new char[size]; //names of processes

    int reciever = 0;
    int N = size;   // make N = number of processes

    if (rank == 0){ // start the play
                       
        order[rank] = rank;
        names[rank] = name;
        iter_num ++;
        while(reciever == 0){  // choose different reciever, but not 0 process
            reciever = rand() % size;
        }
        MPI_Ssend(&iter_num, 1, MPI_INT, reciever, 123, MPI_COMM_WORLD);
        MPI_Ssend(order, size, MPI_INT, reciever, 456, MPI_COMM_WORLD);
        MPI_Ssend(names, size, MPI_CHAR, reciever, 789, MPI_COMM_WORLD);
    }
    else {
        MPI_Recv(&iter_num, 1, MPI_INT, MPI_ANY_SOURCE, 123, MPI_COMM_WORLD, &status);
        MPI_Recv(order, size, MPI_INT, MPI_ANY_SOURCE, 456, MPI_COMM_WORLD, &status);
        MPI_Recv(names, size, MPI_CHAR, MPI_ANY_SOURCE, 789, MPI_COMM_WORLD, &status);
        std::cout << "Rank " << rank << " got ball from rank " << status.MPI_SOURCE << std::endl;

        order[iter_num] = rank;
        names[iter_num] = name;
        iter_num ++;

        if (iter_num < N){  // repeat sending N times
            reciever = rand() % size;

            while(ball_was_passed(reciever, order, iter_num)){  // to have different reciever
                reciever = rand() % size;
            }
                
            MPI_Ssend(&iter_num, 1, MPI_INT, reciever, 123, MPI_COMM_WORLD);
            MPI_Ssend(order, iter_num, MPI_INT, reciever, 456, MPI_COMM_WORLD);
            MPI_Ssend(names, iter_num, MPI_CHAR, reciever, 789, MPI_COMM_WORLD);
        }
        
    }
    MPI_Finalize();

    return 0;
}