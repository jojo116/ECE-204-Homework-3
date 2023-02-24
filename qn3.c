#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#define _2D(i,j, size) ((i)*(size)+(j))

int N;

// matrix multiplication function
void matrix_multiplication(int *a, int *c, int n, int low, int high, int rank, int num_processes)
{
    for (int i = low; i < high; i++) 
    {
        for (int j = 0; j < n; j++) 
        {
            int sum = 0;

            for (int k = 0; k < n; k++) 
            {
                sum += a[_2D(i,k, N)] * a[_2D(k,j, N)];
            }

            c[_2D(i,j, N)] = sum;
        }
    }
}

// main function
int main(int argc, char* argv[])
{
    int rank, num_processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    // Error handling in case the user does not provide the number of threads to use
    if (argc < 2)
    {
        if (rank == 0) printf("Please specify the number of processes to use.\n");
        MPI_Finalize();
        return -1; //Kill the program
    }

    // Read matrix from input file
    FILE *fp;
    fp = fopen("input.txt", "r"); // input.txt contains the matrix
    
    // Obtaining the matrix size and saving it in N
    // This is done by reading the first 
    fscanf(fp, "%d%d", &N, &N);

    // Memory allocation
    int* a = (int*) malloc(N*N*sizeof(int));
    int* c = (int*) malloc(N*N*sizeof(int));
    int thread_count = atoi(argv[1]);
    int temp;
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            fscanf(fp, "%d", &temp);
            a[_2D(i, j, N)] = temp;
        }
    }
    fclose(fp);

    int step = N / num_processes;
    int low = rank * step;
    int high = (rank + 1) * step;

    // Fix the last process to handle the remaining rows
    if (rank == num_processes - 1) 
    {
        high = N;
    }

    // Matrix multiplication on each process
    matrix_multiplication(a, c, N, low, high, rank, num_processes);

    // Gather the results
    int* recv_buffer = NULL;
    if (rank == 0)
    {
        recv_buffer = (int*) malloc(N*N*sizeof(int));
    }

    // this function brings all the results together
    MPI_Gather(&c[_2D(low, 0, N)], (high-low)*N, MPI_INT, recv_buffer, (high-low)*N, MPI_INT, 0, MPI_COMM_WORLD);

    // Print the final result 
    // This is done by the root process as rank = 0
    if (rank == 0)
    {
        printf("After multiplication, we get:\n");
        for (int i = 0; i < N; i++) 
    
        {
            printf("%d ",  c[_2D(i,j, N)]);
        }
        printf("\n");
    }

    return 0;

}