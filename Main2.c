#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

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
void *worker(void *arg) {
    ThreadData *data = (ThreadData *)arg;   // confirming that arg* is ThreadData Struct and safe access to struct
    int sum = 0;

    // If Type = ROW
    if (data->type == ROW) {
        for (int j = 0; j < data->n; j++)
            sum += data->matrix[data->index][j];

        data->sumResult[data->index] = sum;
        data->validResult[data->index] = (sum == data->magicConstant);
    }
    // If Type = COLOUM
    else if (data->type == COLUMN) {
        for (int i = 0; i < data->n; i++)
            sum += data->matrix[i][data->index];

        data->sumResult[data->index] = sum;
        data->validResult[data->index] = (sum == data->magicConstant);
    }

    // If Type = Diganonal (main)
    else if (data->type == DIAG_MAIN) {
        for (int i = 0; i < data->n; i++)
            sum += data->matrix[i][i];

        data->sumResult[0] = sum;
        data->validResult[0] = (sum == data->magicConstant);
    }

    // If Type = Diagnonal (secondary)
    else if (data->type == DIAG_SECONDARY) {
        for (int i = 0; i < data->n; i++)
            sum += data->matrix[i][data->n - i - 1];

        data->sumResult[0] = sum;
        data->validResult[0] = (sum == data->magicConstant);
    }
    // UNIQUENESS TEST
    else if (data->type == UNIQUENESS) {
        //Initalize Varibles
        int size = data->n * data->n; // Size is N
        int *seen = calloc(size + 1, sizeof(int)); // dynamically allocate memory on the heap, initializing all bits to zero - GeeksForGeeks
        int valid = 1; 

        // For each index check int has already been seen, if so, change valuid to 0.
        for (int i = 0; i < data->n; i++) {
            for (int j = 0; j < data->n; j++) {
                int val = data->matrix[i][j];

                if (val < 1 || val > size || seen[val]) {
                    valid = 0;
                }
                seen[val] = 1;
            }
        }

        data->validResult[0] = valid;
        free(seen);
    }

    pthread_exit(NULL);
}

// Create_threads()
// Implemented 20/4/26
// Reusable thread creator for all validations
void create_threads(pthread_t *threads, ThreadData *dataArray, int **matrix, int n, int *sumResults, int *validResults, int magicConstant, TaskType type) {

    for (int i = 0; i < n; i++) {
        dataArray[i].matrix = matrix;
        dataArray[i].n = n;
        dataArray[i].index = i;
        dataArray[i].sumResult = sumResults;
        dataArray[i].validResult = validResults;
        dataArray[i].magicConstant = magicConstant;
        dataArray[i].type = type;
        // create new worker
        pthread_create(&threads[i], NULL, worker, &dataArray[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
}

// ------------------------------ MAIN FUCNTION ----------------------------------------------

int main() {
    FILE *MagicFile;
    int n;

    // Open file
    MagicFile = fopen("Magic.txt", "r"); // This will be changed to be done via command line
    if (MagicFile == NULL) {
        printf("Not able to open the file.\n");
        return 1;
    }

    fscanf(MagicFile, "%d", &n);

    // Allocate matrix for size of n
    int **matrix = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(n * sizeof(int));
    }

    // Read values
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(MagicFile, "%d", &matrix[i][j]);
        }
    }

    fclose(MagicFile);

     // Magic constant - Found from https://en.wikipedia.org/wiki/Magic_constant
    int magicConstant = (n * (n * n + 1)) / 2;

    // Thread setup
    pthread_t threads[n];
    ThreadData data[n];

    int *rowSum = malloc(n * sizeof(int));
    int *colSum = malloc(n * sizeof(int));

    int *rowValid = malloc(n * sizeof(int));
    int *colValid = malloc(n * sizeof(int));

    int diagMainSum[1], diagSecSum[1];
    int diagMainValid[1], diagSecValid[1];

    int uniquenessValid[1];

    // Create worker threads
    create_threads(threads, data, matrix, n, rowSum, rowValid, magicConstant, ROW);
    create_threads(threads, data, matrix, n, colSum, colValid, magicConstant, COLUMN);

    // Diagonal threads
    pthread_t d1, d2;
    ThreadData dData1 = {matrix, n, 0, diagMainSum, diagMainValid, magicConstant, DIAG_MAIN};
    ThreadData dData2 = {matrix, n, 0, diagSecSum, diagSecValid, magicConstant, DIAG_SECONDARY};

    pthread_create(&d1, NULL, worker, &dData1);
    pthread_create(&d2, NULL, worker, &dData2);

    pthread_join(d1, NULL);
    pthread_join(d2, NULL);

    // Uniqueness thread
    pthread_t uThread;
    ThreadData uData = {matrix, n, 0, NULL, uniquenessValid, magicConstant, UNIQUENESS};

    pthread_create(&uThread, NULL, worker, &uData);
    pthread_join(uThread, NULL);

    // ---------------- REPORT ---------------- 
    // not complete just used for confimations
    printf("\n=== Validation Report ===\n");

    for (int i = 0; i < n; i++) {
    printf("Row %d: Sum = %d | %s\n",
           i, rowSum[i],
           rowValid[i] ? "PASS" : "FAIL");
    printf("Column %d: Sum = %d | %s\n",
           i, colSum[i],
           colValid[i] ? "PASS" : "FAIL");
}
    printf("Main Diagonal: Sum = %d | %s\n",
       diagMainSum[0],
       diagMainValid[0] ? "PASS" : "FAIL");
    printf("Secondary Diagonal: Sum = %d | %s\n",
       diagSecSum[0],
       diagSecValid[0] ? "PASS" : "FAIL");
    printf("Uniqueness: %s\n",
       uniquenessValid[0] ? "PASS" : "FAIL");

    // Overall result
    int overall = uniquenessValid[0];
    for (int i = 0; i < n; i++) {
        if (!rowValid[i] || !colValid[i])
            overall = 0;
    }
    if (!diagMainValid[0] || !diagSecValid[0])
        overall = 0;
    printf("\nOverall Result: %s\n",
        overall ? "VALID MAGIC SQUARE" : "INVALID");

        // Free memory
        for (int i = 0; i < n; i++)
            free(matrix[i]);
        free(matrix);

        free(rowSum);
        free(colSum);
        free(rowValid);
        free(colValid);

        return 0;
    }
