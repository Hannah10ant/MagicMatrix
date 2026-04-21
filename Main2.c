#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Enum to define task type - Type of cacluation that needs to be used
typedef enum {
    ROW,
    COLUMN,
    DIAG_MAIN,
    DIAG_SECONDARY
} TaskType;

// Struct for passing data to threads
typedef struct {
    int **matrix;
    int n;
    int index;      // used for row/column, ignored for diagonals as they have both secondary & primary
    int *result;
    TaskType type;
} ThreadData;

// ------------------- Thread functions ---------------------------

// sumLine()
// Implemented 20/04/26
// Used to compute the sum of a line in a matrix
//
// fucntions : Uses If statements to determine calculation which is dependant on TaskType Type
void *sumLine(void *arg) {
    ThreadData *data = (ThreadData *)arg; // confirming that arg* is ThreadData Struct and safe access to struct
    int sum = 0; // initalise sum

    // Row Calcualtions
    if (data->type == ROW) {
        for (int j = 0; j < data->n; j++) {
            sum += data->matrix[data->index][j];
        }
        data->result[data->index] = sum;
        printf("Row %d sum = %d (Thread %lu)\n", data->index, sum, pthread_self()); // confirmation print

    // Coloum Calculations
    } else if (data->type == COLUMN) {
        for (int i = 0; i < data->n; i++) {
            sum += data->matrix[i][data->index];
        }
        data->result[data->index] = sum;
        printf("Column %d sum = %d (Thread %lu)\n", data->index, sum, pthread_self()); // confirmation print

    // Diagonal (main) claculations
    } else if (data->type == DIAG_MAIN) {
        for (int i = 0; i < data->n; i++) {
            sum += data->matrix[i][i];
        }
        data->result[0] = sum;
        printf("Main diagonal sum = %d (Thread %lu)\n", sum, pthread_self()); // confirmation print
    
    // Diagonal (secondary) claculations
    } else if (data->type == DIAG_SECONDARY) {
        for (int i = 0; i < data->n; i++) {
            sum += data->matrix[i][data->n - i - 1];
        }
        data->result[0] = sum;
        printf("Secondary diagonal sum = %d (Thread %lu)\n", sum, pthread_self()); // confirmation print
    }
    pthread_exit(NULL);
}

// Create_threads()
// Implemented 20/4/26
// Reusable thread creator for rows/columns
//
// Function : uses both structure and pthread_create to iterativly create threads for each row and coloum
//            allowing for efficency

void create_threads(pthread_t *threads, ThreadData *dataArray,
                    int **matrix, int n, int *results, TaskType type) {

    for (int i = 0; i < n; i++) {
        dataArray[i].matrix = matrix;
        dataArray[i].n = n;
        dataArray[i].index = i;
        dataArray[i].result = results;
        dataArray[i].type = type;

        pthread_create(&threads[i], NULL, sumLine, &dataArray[i]);
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

    // Initalize varibles
    pthread_t threads[n];
    ThreadData data[n];

    int *rowResults = malloc(n * sizeof(int));
    int *colResults = malloc(n * sizeof(int));

    int diagMain[1];
    int diagSecondary[1];

    // Create Row Threads
    // printf("=== Row Threads ===\n"); 
    create_threads(threads, data, matrix, n, rowResults, ROW);

    // Create column Threads
    //printf("\n=== Column Threads ===\n");
    create_threads(threads, data, matrix, n, colResults, COLUMN);

    // Diagonals
    // Initalize
    pthread_t d1, d2;
    ThreadData dData1, dData2;

    // Create diagonal struct (main)
    dData1.matrix = matrix;
    dData1.n = n;
    dData1.result = diagMain;
    dData1.type = DIAG_MAIN;

    // Create thread
    pthread_create(&d1, NULL, sumLine, &dData1);
    pthread_join(d1, NULL);

    //create diagonal struct (secondary)
    dData2.matrix = matrix;
    dData2.n = n;
    dData2.result = diagSecondary;
    dData2.type = DIAG_SECONDARY;

    // create thread
    pthread_create(&d2, NULL, sumLine, &dData2);
    pthread_join(d2, NULL);

    // Final output - printing confirm
    printf("\nFinal Results:\n");
    for (int i = 0; i < n; i++) {
        printf("Row %d = %d | Column %d = %d\n",
               i, rowResults[i], i, colResults[i]);
    }

    printf("Main Diagonal = %d\n", diagMain[0]);
    printf("Secondary Diagonal = %d\n", diagSecondary[0]);

    // Free memory
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(rowResults);
    free(colResults);

    return 0;
}
