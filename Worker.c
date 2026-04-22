// Worker.c - File containing worker threads
// 
// last edited 23.4.26 - Hannah Tennant

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Worker.h"

// worker()
// Implemented 21/04/26
//
// fucntions : Uses If statements to determine calculation which is dependant on TaskType Type
// Issue --- Threads do not take the 1,2,3,4 ID...
void *worker(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int sum = 0;

    // If Type = ROW
    if (data->type == ROW) {
        for (int j = 0; j < data->n; j++)
            sum += data->matrix[data->index][j];

        sleep(1);

        data->sumResult[data->index] = sum;
        data->validResult[data->index] = (sum == data->magicConstant);

        pthread_mutex_lock(&scoreMutex);
        if (data->validResult[data->index]) globalScore++;
        else if (globalScore > 0) globalScore--;
        pthread_mutex_unlock(&scoreMutex);
    }

    // If Type = COLOUM
    else if (data->type == COLUMN) {
        for (int i = 0; i < data->n; i++)
            sum += data->matrix[i][data->index];

        sleep(1);

        data->sumResult[data->index] = sum;
        data->validResult[data->index] = (sum == data->magicConstant);

        pthread_mutex_lock(&scoreMutex);
        if (data->validResult[data->index]) globalScore++;
        else if (globalScore > 0) globalScore--;
        pthread_mutex_unlock(&scoreMutex);
    }

    // If Type = Diganonal (main)
    else if (data->type == DIAG_MAIN) {
        for (int i = 0; i < data->n; i++)
            sum += data->matrix[i][i];

        sleep(1);

        data->sumResult[0] = sum;
        data->validResult[0] = (sum == data->magicConstant);

        pthread_mutex_lock(&scoreMutex);
        if (data->validResult[0]) globalScore++;
        else if (globalScore > 0) globalScore--;
        pthread_mutex_unlock(&scoreMutex);
    }

    // If Type = Diagnonal (secondary)
    else if (data->type == DIAG_SECONDARY) {
        for (int i = 0; i < data->n; i++)
            sum += data->matrix[i][data->n - i - 1];

        sleep(1);

        data->sumResult[0] = sum;
        data->validResult[0] = (sum == data->magicConstant);

        pthread_mutex_lock(&scoreMutex);
        if (data->validResult[0]) globalScore++;
        else if (globalScore > 0) globalScore--;
        pthread_mutex_unlock(&scoreMutex);
    }

    // UNIQUENESS TEST
    else if (data->type == UNIQUENESS) {
        int size = data->n * data->n;
        int *seen = calloc(size + 1, sizeof(int));
        int valid = 1;

        for (int i = 0; i < data->n; i++) {
            for (int j = 0; j < data->n; j++) {
                int val = data->matrix[i][j];

                if (val < 1 || val > size || seen[val]) {
                    valid = 0;
                }
                seen[val] = 1;
            }
        }

        sleep(1);

        data->validResult[0] = valid;

        pthread_mutex_lock(&scoreMutex);
        if (valid) globalScore++;
        else if (globalScore > 0) globalScore--;
        pthread_mutex_unlock(&scoreMutex);

        free(seen);
    }

    // thread completion logs (only once per category) - testing
    pthread_mutex_lock(&scoreMutex);

    if (data->type == ROW) {
        rowDone++;
        if (rowDone == data->n) {
            printf("Thread ID-%lu: Row checks completed.\n", pthread_self());
        }
    }
    else if (data->type == COLUMN) {
        colDone++;
        if (colDone == data->n) {
            printf("Thread ID-%lu: Column checks completed.\n", pthread_self());
        }
    }
    else if (data->type == DIAG_MAIN || data->type == DIAG_SECONDARY) {
        diagDone++;
        if (diagDone == 2) {
            printf("Thread ID-%lu: Diagonal checks completed.\n", pthread_self());
        }
    }
    else if (data->type == UNIQUENESS) {
        uniqueDone++;
        if (uniqueDone == 1) {
            printf("Thread ID-%lu: Uniqueness check completed.\n", pthread_self());
        }
    }

    pthread_mutex_unlock(&scoreMutex);

    pthread_exit(NULL);
}
