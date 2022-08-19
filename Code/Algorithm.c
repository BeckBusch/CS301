// Includes
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>

// Unicode things
#include <Windows.h>

struct Queue {
    int start, end, size;
    unsigned capacity;
    int *array;
} Queue;

struct Vertex {
    // Colour enum for seeing if vertices have been visited.
    enum { WHITE, GREY, BLACK } colour;
    int distance;
} Vertex;

struct Queue* makeQueue(unsigned size) {

    // We need to make a pointer to the queue so that we can pass it by reference from the function we call push/pop from.
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = size;
    queue->start = 0; // Setting up the queue so that the start of the queue (next item) is always at zero.
    queue->size = 0; // Queue is empty to begin with.
    queue->end = queue->capacity - 1;
    queue->array = (int *) malloc(queue->capacity * sizeof(int)); // Using sizeof(int) means we allocate enough memory.
    // Initialise all values to minus one so that we can tell if the queue is empty or not (there is a vertex zero).
    memset(queue->array, -1, queue->capacity);

}

void push(struct Queue* queue, int vertex) {

    // Move the end marker along one, if we reach capacity, go back to the beginning.
    queue->end = (queue->end + 1) % queue->capacity;
    queue->array[queue->end] = vertex;
    queue->size = queue->size + 1;

}

int pop(struct Queue* queue) {

    // Get the vertex from the start of the queue.
    int vertex = queue->array[queue->start];
    // Move the start marker along one, if we reach capacity, go back to the beginning.
    queue->start = (queue->start + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return vertex;

}

// Implementation of the BFS algorithm.
void BFS(int source, int target, int adjlist[][4], int size) {

    // Set up an array of vertices to track progress.
    struct Vertex vertices[size];
    memset(&vertices->colour, WHITE, size);
    memset(&vertices->distance, INT_MAX, size);

    vertices[source].colour = GREY;
    vertices[source].distance = 0;

    // Set up the queue.
    struct Queue* queue = makeQueue(size);
    push(queue, source);

    // While the queue still has vertices to visit, visit them.
    while (queue->size != 0) {
        // Pop the front of the queue, this is the key in adjlist.
        int key = pop(queue);
        /* For each connected entry in adjlist, check if it has been visited.
           Then, mark it as grey if it has yet to be visited, and add it to the queue. 
           Make sure to update distance as well. */
        for (int i = 0; i < 4; i++) {
            int entry = adjlist[key][i];
            // If entry is -1 it means there is no valid entry here (vacancy).
            if (entry != -1) {
                if (vertices[entry].colour == WHITE) {
                    vertices[entry].colour = GREY;
                    vertices[entry].distance = vertices[key].distance + 1;
                    push(queue, entry);
                }
            }
        }
        // Mark the key vertex as visited.
        vertices[key].colour = BLACK;
    }

}

// Implementation of the A* algorithm.
void ASTAR() {

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
    int xydim, xdim, ydim = 0;

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
    xdim = xydim / ydim;

    // Start reading the file from the top again.
    rewind(fp);

    // Re-read the characters from the file, but this time, store them inside an array we just initialised.
    int array[ydim][xdim];
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

    // Each zero can only be adjacent to 4 zeroes maximum.
    int adjlist[xydim][4];
    memset(adjlist, -1, sizeof adjlist);
    int cnode;

    /* Construct the adjacency list.
       Numbered reading from left to right, top down. */
    for (int i = 0; i < ydim; i++) {
        for (int j = 0; j < xdim; j++) {
            // For loops go through rows, cols.
            if (array[i][j] == 0) {
                cnode = i * xdim + j; 
                if (array[i - 1][j] == 0) {
                    // Row above.
                    adjlist[cnode][0] = (i - 1) * xdim + j;
                } else if (array[i + 1][j] == 0) {
                    // Row below.
                    adjlist[cnode][1] = (i + 1) * xdim + j;
                } else if (array[i][j - 1] == 0) {
                    // Column left.
                    adjlist[cnode][2] = i * xdim + j - 1;
                } else if (array[i][j + 1] == 0) {
                    // Column right.
                    adjlist[cnode][3] = i * xdim + j + 1;
                }
            }
        }
    }

    // Print dimension for debugging purpose.
    printf("(Size of the array is %d)\n", xydim);

    // Close the stream.
    fclose(fp);

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

    int source = y1 * xdim + x1;
    int target = y2 * xdim + x2;

    BFS(source, target, adjlist, xydim);
    //ASTAR();

    //█ is for the completed path...

    // Keep the window open indefinitely.
    while (1) {

    }

    return 0;

}
