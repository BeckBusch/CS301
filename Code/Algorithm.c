// Includes
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>

// Unicode things
#include <Windows.h>

typedef struct Queue {
    int start, end;
    unsigned capacity;
    int *array;
} Queue;

void push() {

}

void pop() {

}

void BFS() {

}

int main() {

    // Get current working directory.
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);

    // Set the specific file to be read from.
    char filename[] = "map_1.txt";

    char buf[PATH_MAX];
    sprintf(buf, "%s\\%s", cwd, filename);

    // Open the file to be read and copy the values into an array.
    FILE *fp;
    fp = fopen(buf, "r");

    char seq;
    SetConsoleOutputCP(CP_UTF8);

    // Store the dimensions of the map so we can't go outside of it later on.
    int xydim, ydim = 0;

    // Read characters from the file and print them until we reach EOF.
    // This part is only for demonstration purposes, we don't need this in the final code.
    while (1) {
        if (feof(fp) != 0) {
            break;
        }
        seq = fgetc((FILE*)fp);
        // Use ASCII code for 1, not 1 itself.
        if (seq == 49) {
            printf("%s", "██");
            xydim++;
        } else if (seq == 48) {
            printf("▒▒");
            xydim++;
        } else if (seq == 10) {
            printf("\n");
            ydim++;
        }
    }

    // Start reading the file from the top again.
    rewind(fp);

    // Re-read the characters from the file, but this time, store them inside an array we just initialised.
    int array[ydim][xydim / ydim];
    int countx, county = 0;
    while (1) {
        if (feof(fp) != 0) {
            break;
        }
        seq = fgetc((FILE*)fp);
        // Use ASCII code for 1, not 1 itself.
        if (seq == 49) {
            array[county][countx] = 1;
            countx++;
        } else if (seq == 48) {
            array[county][countx] = 0;
            countx++;
        } else if (seq == 10) {
            county++;
        }
        countx++;
    }
    // Now the array is stored in memory.

    // Print dimension for debugging purpose.
    printf("(Size of the array is %d)\n", xydim);

    // Close the stream.
    fclose(fp);

    // Set up the queue.
    Queue queue;

    queue.capacity = xydim;
    queue.start = 0;
    queue.end = queue.capacity - 1;
    queue.array = (int *) malloc(queue.capacity);

    int x1; int y1; int x2; int y2;

    // Print statements to prompt the user for the start and end co-ordinates.
    printf("\nFrom what point should the traversal start?\n");
    printf("Please enter the x co-ordinate:\n");
    scanf("%d", &x1);
    printf("Please enter the y co-ordinate:\n");
    scanf("%d", &y1);

    printf("\nFrom what point should the traversal end?\n");
    printf("Please enter the x co-ordinate:\n");
    scanf("%d", &x2);
    printf("Please enter the y co-ordinate:\n");
    scanf("%d", &y2);

    //BFS(x1, y1, x2, y2);

    //█ is for the completed path...

    // Keep the window open indefinitely.
    while (1) {

    }

    return 0;

}
