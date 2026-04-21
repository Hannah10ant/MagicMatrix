// Testingsleep.c - Testing file to ensure that POSIX libary unistd.h works on code for sleep fucntion
// Last Edited 21/04/26 - Hannah Tennant
// Compile using gcc Testingsleep.c -o Testingsleep

// Notes taken from GeeksforGeeks.org

#include <stdio.h>
#include <unistd.h> // required for sleep()

int main() {
    printf("Start\n");

    sleep(1); // 1 second

    printf("End (after 1 second delay)\n");

    return 0;
}