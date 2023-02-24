#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#define _2D(i,j, size) ((i)*(size)+(j))

int N;
struct args
{
    int (*a);
    int (*b);
    int (*c);
    int n;
    int start_idx;
    int end_idx;
};

void* matrix_multiplication(void* args)
{
    struct args* thread_args = (struct args*) args;
    int (*a) = thread_args->a;
    int (*b) = thread_args->b;
    int (*c) = thread_args->c;
    int n = thread_args->n;

    // The starting and ending indices of the elements each thread will process.
    int start_idx = thread_args->start_idx;
    int end_idx = thread_args->end_idx;

    for (int k = start_idx; k < end_idx; k++) 
    {
        int i = k / n; // row index of the element in output matrix
        int j = k % n; // calculates the column index
        c[_2D(i,j, N)] = 0;

        for (int l = 0; l < n; l++) 
        {
            c[_2D(i,j, N)] += a[_2D(i,l, N)] * b[_2D(l,j, N)];
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    // Error handling incase the user does not provide the number of threads to use
    if (argc < 2)
    {
        printf("Please specify the number of threads to use.\n");
        return -1;
    }

    FILE *fp;
    fp = fopen("input.txt", "r"); // input.txt contains the file we are reading from


    // Obtaining the matrix size and saving it in N
    // This is done by reading the first 
    fscanf(fp, "%d%d", &N, &N);

    int* a = (int*) malloc(N*N*sizeof(int));
    int* b = (int*) malloc(N*N*sizeof(int));
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

    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            fscanf(fp, "%d", &temp);
            b[_2D(i, j, N)] = temp;
        }
    }
    fclose(fp);

    struct args thread_args[thread_count];
    pthread_t thread[thread_count];

    // Divide the total number of elements by the number of threads to obtain the number of elements to be processed by each thread 
    // We can call this the workload per thread
    int work_per_thread = ceil((float)(N*N) / thread_count);

    // Distribute said workload among threads
    for (int i = 0; i < thread_count; i++)
    {
        int start_idx = i * work_per_thread;
        int end_idx = fmin((i+1) * work_per_thread, N*N);

        struct args temp = {a, b, c, N, start_idx, end_idx};
        thread_args[i] = temp;
        pthread_create(&thread[i], NULL, matrix_multiplication, &thread_args[i]);

    }
    // Joining threads
    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(thread[i], NULL);
    }
    
    // Print results
    printf("After multiplication, we get:\n");
    for (int i = 0; i < N; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            printf("%d ",  c[_2D(i,j, N)]);
        }
        printf("\n");
    }

    return 0;
}
