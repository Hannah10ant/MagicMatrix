// Main2.c - Main file for Assignment
// Last Edited 22/04/26 - Hannah Tennant
// Compile using gcc -lpthread Testingpthreads.c -o Testingpthreads
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// ---------------- GLOBAL SCORE & MUTEX ----------------
int globalScore = 0; // global shared score
pthread_mutex_t scoreMutex; // mutex to ensure only one thread updates score at a time

// counters for proper completion detection
int rowDone = 0;
int colDone = 0;
int diagDone = 0;
int uniqueDone = 0;

// ---------------- ENUM ----------------
typedef enum {
    ROW,
    COLUMN,
    DIAG_MAIN,
    DIAG_SECONDARY,
    UNIQUENESS
} TaskType;

// ---------------- STRUCT ----------------
typedef struct {
    int **matrix;
    int n;
    int index;  // used for row/column, ignored for diagonals as they have both secondary & primary
    int *sumResult;
    int *validResult;
    int magicConstant;
    TaskType type;
} ThreadData;

// ------------------- Thread functions ---------------------------
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

// Create_threads()
// Implemented 20/4/26
// Reusable thread creator for Rows/Coloums
void create_threads(pthread_t *threads, ThreadData *dataArray, int **matrix, int n, int *sumResults, int *validResults, int magicConstant, TaskType type) {

    for (int i = 0; i < n; i++) {
        dataArray[i].matrix = matrix;
        dataArray[i].n = n;
        dataArray[i].index = i;
        dataArray[i].sumResult = sumResults;
        dataArray[i].validResult = validResults;
        dataArray[i].magicConstant = magicConstant;
        dataArray[i].type = type;
        // create new worker/thread for each i, passing in the individual Array.
        pthread_create(&threads[i], NULL, worker, &dataArray[i]);
    }
}

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

    // initialize mutex
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
                printf("ERROR: issue reading value at [%d][%d]\n", i, j); // notifies user
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