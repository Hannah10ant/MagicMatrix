#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// creating a struct to pass in results - grouping restults

// Enum to define task type 
typedef enum { ROW, COLUMN } TaskType;
// Struct for passing data to threads 
typedef struct {
     int **matrix; 
     int n; 
     int index; // row or column index 
     int *result; // result array 
     TaskType type; // ROW or COLUMN 
     } ThreadData;

int main() {
    FILE *MagicFile;
    int i, j;
    int n;

    // Open file
    MagicFile = fopen("Magic.txt", "r"); // Chnage this to be command line based

    if (MagicFile == NULL) {
        printf("Not able to open the file.\n");
        return 1;
    }

    // Read n
    fscanf(MagicFile, "%d", &n);

    // Dynamically allocate n x n matrix
    int **matrix = malloc(n * sizeof(int *)); // n X int
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(n * sizeof(int)); // n X int
    }

    // Read matrix values
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(MagicFile, "%d", &matrix[i][j]);

            // Validate values (must be 1–9) - dunno if needed yet
            //if (matrix[i][j] < 1 || matrix[i][j] > 9) {
            //    printf("Invalid value %d at position [%d][%d]\n", matrix[i][j], i, j);
            //}
        }
    }

    // Print matrix (to check) - workS !
    //printf("Matrix (%dx%d):\n", n, n);
    //for (int i = 0; i < n; i++) {
    //    for (int j = 0; j < n; j++) {
    //        printf("%d ", matrix[i][j]);
    //    }
    //    printf("\n");
    //}

    // Free memory
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);

    fclose(MagicFile);

    return 0;
}