// Shared.h - Contains all shared Data
// Implemented 23.4.26 - Hannah Tennant

#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>

// ---------------- GLOBALS ----------------
extern int globalScore;
extern pthread_mutex_t scoreMutex;

extern int rowDone;
extern int colDone;
extern int diagDone;
extern int uniqueDone;

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
    int index;
    int *sumResult;
    int *validResult;
    int magicConstant;
    TaskType type;
} ThreadData;

#endif