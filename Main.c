// Main.c - Main file for Assignment
// Last Edited 23.04.26 - Hannah Tennant
//
// Compile using gcc Main.c Threads.c Worker.c -lpthread -o Main
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "shared.h"
#include "threads.h"
#include "worker.h"

// ---------------- GLOBAL SCORE & MUTEX ----------------
int globalScore = 0; // global shared score
pthread_mutex_t scoreMutex; // mutex to ensure only one thread updates score at a time

// counters for proper completion detection
int rowDone = 0;
int colDone = 0;
int diagDone = 0;
int uniqueDone = 0;

// ------------------------------ MAIN FUCNTION ----------------------------------------------
int main(int argc, char *argv[]) {

    // Arguments input if not explict
    if (argc < 2) {
        printf("Usage: ./Main <input_file>\n", argv[0]);
        printf("Please retry");
        return 1;
    }

    FILE *MagicFile;
    int n;

    // initialize mutex - Die.net - Linux Man Page
    // works as global varible - used in worker() (Worker.c)
    pthread_mutex_init(&scoreMutex, NULL);

    // Open file
    MagicFile = fopen(argv[1], "r");
    if (MagicFile == NULL) {
        printf("Not able to open the file.\n");
        return 1;
    }

    // Read File Contents
    fscanf(MagicFile, "%d", &n);

    // Allocate matrix for size of n
    int **matrix = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        matrix[i] = malloc(n * sizeof(int));

    // Read values
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (fscanf(MagicFile, "%d", &matrix[i][j]) != 1) { // add failure checking - cant read
                printf("ERROR: issue with value at [%d][%d]\n", i, j); // notifies user
                for (int i = 0; i < n; i++) free(matrix[i]);
                free(matrix);
                printf("Please check text file");
                return 1;
}

    fclose(MagicFile);

    // Magic constant - Found from https://en.wikipedia.org/wiki/Magic_constant
    int magicConstant = (n * (n * n + 1)) / 2;
    int maxScore = (2 * n) + 3; // defines what the total vaild scre should equal to
                                // n=3 : (3 * 2(rows & coloums)) + 3(diagonal score total) = 9

    // Thread setup
    pthread_t rowThreads[n];
    pthread_t colThreads[n];

    ThreadData rowData[n];
    ThreadData colData[n];

    // Intialize worker varibles
    int *rowSum = malloc(n * sizeof(int));
    int *colSum = malloc(n * sizeof(int));
    int *rowValid = malloc(n * sizeof(int));
    int *colValid = malloc(n * sizeof(int));

    int diagMainSum[1], diagSecSum[1];
    int diagMainValid[1], diagSecValid[1];
    int uniquenessValid[1];

    // Create ALL threads first (true concurrency)
    create_threads(rowThreads, rowData, matrix, n, rowSum, rowValid, magicConstant, ROW);
    create_threads(colThreads, colData, matrix, n, colSum, colValid, magicConstant, COLUMN);

    pthread_t d1, d2;
    ThreadData dData1 = {matrix, n, 0, diagMainSum, diagMainValid, magicConstant, DIAG_MAIN};
    ThreadData dData2 = {matrix, n, 0, diagSecSum, diagSecValid, magicConstant, DIAG_SECONDARY};

    pthread_create(&d1, NULL, worker, &dData1);
    pthread_create(&d2, NULL, worker, &dData2);

    pthread_t uThread;
    ThreadData uData = {matrix, n, 0, NULL, uniquenessValid, magicConstant, UNIQUENESS};

    pthread_create(&uThread, NULL, worker, &uData);

    // JOIN all threads
    for (int i = 0; i < n; i++) pthread_join(rowThreads[i], NULL);
    for (int i = 0; i < n; i++) pthread_join(colThreads[i], NULL);

    pthread_join(d1, NULL);
    pthread_join(d2, NULL);

    pthread_join(uThread, NULL);

    // ---------------- REPORT ---------------- 
    sleep(2);
    printf("\n--- Magic Square Report ---\n");

    int allRowsValid = 1;
    for (int i = 0; i < n; i++) {
        if (!rowValid[i]) {  // if rowVaild[row] is not 1 then print invalid note for specfic row
            printf("Rows:   Row %d Invalid\n", i + 1);
            allRowsValid = 0;
        }
    }
    // print seprate statement for all vaild
    if (allRowsValid) printf("Rows:   All Valid\n");

    //Coloumn Report
    int allColsValid = 1;
    for (int i = 0; i < n; i++) {
        if (!colValid[i]) {
            printf("Cols:   Col %d Invalid\n", i + 1);
            allColsValid = 0;
        }
    }
    if (allColsValid) printf("Cols:   All Valid\n");

    // Diagonal Report
    int diagFail = 0;
    if (!diagMainValid[0]) {
        printf("Diags:  Main Diag Invalid\n");
        diagFail = 1;
    }
    if (!diagSecValid[0]) {
        printf("Diags:  Secondary Diag Invalid\n");
        diagFail = 1;
    }
    if (!diagFail) printf("Diags:  All Valid\n");

    // Uniqueness Report
    if (uniquenessValid[0])
        printf("Unique: Passed\n");
    else
        printf("Unique: Failed (Duplicates found)\n");

    // final global score vs max calc score 
    printf("Final Score:  %d / %d\n", globalScore, maxScore);

    // Overall Valid
    if (globalScore == maxScore)
        printf("RESULT: VALID MAGIC SQUARE\n");
    else
        printf("RESULT: INVALID MAGIC SQUARE\n");

    // Free Memory
    for (int i = 0; i < n; i++) free(matrix[i]);
    free(matrix);

    free(rowSum);
    free(colSum);
    free(rowValid);
    free(colValid);

    // destroy mutex to free memory
    pthread_mutex_destroy(&scoreMutex);

    return 0;
}