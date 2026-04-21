// Testingpthreads.c - Testing file to ensure that POSIX libary pthreads works on code
// Last Edited 19/04/26 - Hannah Tennant
// Compile using gcc -lpthread Testingpthreads.c -o Testingpthreads

// Notes taken from GeeksforGeeks.org

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// pthreads can only pass one vlaue 
//pass more must be a data structure


// Creating a thread
void* hello_thread(void* arg) { // passes pointer
    int thread_num = *(int*)arg;
    printf("\nHello from thread %d!\n", thread_num);
    sleep(1);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    int t1 = 1;
    int t2 = 2;

    // Create first thread - thread 1 exucutes hello_thread and takes a interger ID for 1
    printf("Starting thread 1\n");
    if (pthread_create(&thread1, NULL, hello_thread, &t1) != 0) {
        perror("Failed to create thread 1");
        return 1;
    }
    printf("Starting Thread 2\n");
    // Create second thread
    if (pthread_create(&thread2, NULL, hello_thread, &t2) != 0) {
        perror("Failed to create thread 2");
        return 1;
    }

    // Wait for BOTH threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    sleep(1);
    // Final completion message
    printf("\nBoth threads have completed execution!\n");

    return 0;
}
// working 9:28pm 19/04/26