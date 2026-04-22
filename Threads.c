// Thread.c - File containing thread related functions for assignment
// implemented 23.4.26
#include <pthread.h>
#include "threads.h"
#include "Worker.h"

// ------------------- Thread functions ---------------------------

// Create_threads()
// Implemented 20/4/26
// Reusable thread creator for Rows/Coloums
void create_threads(pthread_t *threads, ThreadData *dataArray,
                    int **matrix, int n,
                    int *sumResults, int *validResults,
                    int magicConstant, TaskType type) {

    for (int i = 0; i < n; i++) {
        dataArray[i].matrix = matrix;
        dataArray[i].n = n;
        dataArray[i].index = i;
        dataArray[i].sumResult = sumResults;
        dataArray[i].validResult = validResults;
        dataArray[i].magicConstant = magicConstant;
        dataArray[i].type = type;

        pthread_create(&threads[i], NULL, worker, &dataArray[i]);
    }
}