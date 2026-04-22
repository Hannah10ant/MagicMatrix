// Threads.h - Header file for Threads.c - Containing all thread files
// Implemented 23.4.16 - Hannah Tennant


#ifndef THREADS_H
#define THREADS_H

#include "Shared.h"

void create_threads(pthread_t *threads, ThreadData *dataArray,
                    int **matrix, int n,
                    int *sumResults, int *validResults,
                    int magicConstant, TaskType type);

#endif