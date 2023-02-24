#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// defining the number of threads and number of iterations we shall use
// if the hardware is sequentially consistent, then the counter should be the same as the expected value we will get
#define number_of_threads 4
#define number_of_iterations 10000

// start the counter
int counter = 0;

// increment the counter
void *increment_counter(void *thread_id) 
{
    int tid = *(int *)thread_id; //thread ID
    int tmp = tid * number_of_iterations; // we increment by this variable for each iteration // 40000
    
    // distribute the work of incrementing the counter variable among multiple threads,
    for (int i = 0; i < number_of_iterations; i++) 
    {
        counter += tmp;
    }
    pthread_exit(NULL);
}

// main function
int main() 
{
    // keep thread identifiers
    pthread_t threads[number_of_threads];
    int thread_ids[number_of_threads];

    for (int i = 0; i < number_of_threads; i++) 
    {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, increment_counter, (void *)&thread_ids[i]); // passing in the thread ID to the increment counter function
    }

    // join the main thread
    for (int i = 0; i < number_of_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // find expected value
    long long int expected_value = (long long int) number_of_threads * number_of_iterations * (number_of_threads - 1) * number_of_iterations / 2;

    // print results
    printf("Final value of counter: %d\n", counter);
    printf("Expected value of counter: %lld\n", expected_value);

    if (counter == expected_value)
    {
        printf("This operation was carried out with sequential consistency.");
    }
    else
    {
        printf("This operation was carried out with out of order consistency.");
    }
    return 0;
}