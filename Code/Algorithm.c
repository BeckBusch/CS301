// Includes
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

// Unicode things
#include <Windows.h>

struct PriorityVertex {
    int value;
    int priority;
} PriorityVertex;

struct Queue {
    int start, end, size;
    unsigned capacity;
    struct PriorityVertex *array;
} Queue;

struct Vertex {
    // Colour enum for seeing if vertices have been visited.
    enum { WHITE, GREY, BLACK } colour;
    int distance;
    bool visited;
} Vertex;

struct Queue* makeQueue(unsigned size) {

    // We need to make a pointer to the queue so that we can pass it by reference from the function we call push/pop from.
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = size;
    queue->start = 0; // Setting up the queue so that the start of the queue (next item) is always at zero.
    queue->size = 0; // Queue is empty to begin with.
    queue->end = queue->capacity - 1;
    queue->array = (struct PriorityVertex *) malloc(queue->capacity * sizeof(struct PriorityVertex)); // Using sizeof(int) means we allocate enough memory.

}

void init(struct Queue* queue, int size) {

    // Initialise all values to minus one so that we can tell if the queue is empty or not (there is a vertex zero).
    for (int i = 0; i < size; i++) {
        struct PriorityVertex defaultValue = { .value = -1, .priority = INT_MAX, };
        queue->array[i] = defaultValue;
    }

}

void push(struct Queue* queue, int vertex, int priority) {

    // Move the end marker along one, if we reach capacity, go back to the beginning.
    queue->end = (queue->end + 1) % queue->capacity;
    struct PriorityVertex newVertex = { .value = vertex, .priority = priority };
    queue->array[queue->end] = newVertex;
    queue->size = queue->size + 1;

}

int pop(struct Queue* queue) {

    // Get the vertex from the start of the queue.
    struct PriorityVertex vertex = queue->array[queue->start];
    // Reset to empty value of -1.
    struct PriorityVertex defaultValue = { .value = -1, .priority = INT_MAX };
    queue->array[queue->start] = defaultValue;
    // Move the start marker along one, if we reach capacity, go back to the beginning.
    queue->start = (queue->start + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return vertex.value;

}

bool contains(struct Vertex vertices[], int vertex) {

    // If the vertex has been visited return true, else return false.
    if (vertices[vertex].visited == true) {
        return true;
    }
    return false;

}

// bool queueContains(struct Queue* queue, int vertex) {

//     // If the queue contains the vertex return true, else return false.
//     for (int i = 0; i < queue->capacity; i++) {
//         struct PriorityVertex key = queue->array[queue->start + i];
//         if (key.value == vertex) {
//             return true;
//         }
//     }
//     return false;

// }

void delete(struct Queue* queue, int pos) {

    // Replace each entry in the array with the next entry until the end.
    for (int i = 0; i < queue->end - pos; i++) {
        queue->array[pos + i] = queue->array[pos + i + 1];
    }
    // Move the end pointer over.
    queue->end--;

}

struct PriorityVertex removeVertex(struct Queue* queue) {

    // This function operates like pop but differs in that the priority queue is out of order.
    int maxPriority = INT_MAX;
    int pos;
    for (int i = 0; i < queue->capacity; i++) {
        // Although maxPriority is an indicator of what to prioritise, lower values should be visited first despite the name.
        if (queue->array[queue->start + i].priority < maxPriority) {
            maxPriority = queue->array[queue->start + i].priority;
            pos = queue->start + i;
        }
    }

    struct PriorityVertex returnVertex = { .value = queue->array[pos].value, .priority = queue->array[pos].priority };
    // Call to delete subroutine.
    delete(queue, pos);
    // Return the vertex.
    return returnVertex;

}

// Implementation of the BFS algorithm.
void BFS(int source, int target, int adjlist[][4], int size) {

    // Set up an array of vertices to track progress.
    struct Vertex vertices[size];
    for (int i = 0; i < size; i++) {
        vertices[i].colour = WHITE;
        vertices[i].distance = INT_MAX;
    }

    vertices[source].colour = GREY;
    vertices[source].distance = 0;

    // Set up the queue.
    struct Queue* queue = makeQueue(size);
    init(queue, size);
    push(queue, source, 0);

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
                    push(queue, entry, 0);
                }
            }
        }
        // Mark the key vertex as visited.
        vertices[key].colour = BLACK;
    }
    printf("The target is %d blocks from the source.\n", vertices[target].distance);

}

// Implementation of the A* algorithm.
void ASTAR(int source, int target, int adjlist[][4], int size, int array[15][19], int xdim) {

    // Set up the queue.
    struct Queue* queue = makeQueue(size);
    init(queue, size);
    // Source distance is zero...
    push(queue, source, 0);

    // Set up an array to track vertices and their distances (INT_MAX means a vertex has yet to be visited).
    struct Vertex vertices[size];
    //int parents[size];
    for (int i = 0; i < size; i++) {

        // We don't care about vertex colour for this implementation.
        vertices[i].distance = INT_MAX;
        vertices[i].visited = false;
        //parents[i] = -1;

    }

    vertices[source].distance = 0;
    //int lastDistance;
    // While we have yet to reach the destination, keep looping.
    while (1) {

        struct PriorityVertex key = removeVertex(queue);
        vertices[key.value].visited = true;
        //lastDistance = vertices[key.value].distance;

        if (key.value == target) {
            // for (int i = 0; i < size; i++) {
            //     printf("%d ", vertices[i].distance);
            // }
            printf("The target is %d blocks from the source.\n", vertices[key.value].distance );//
            return;
        }

        int ydim = size / xdim;
            // for (int i = 0; i < size; i++) {
            //     printf("%d\n", queue->array[i].value);
            // }
            // printf("\n");
            // for (int county = 0; county < ydim; county++) {
            //     for (int countx = 0; countx < xdim; countx++) {
            //         if (array[county][countx] == 0) {
            //             if (vertices[county * xdim + countx].visited == true) {
            //                 printf("%s", "  ");
            //             } else {
            //                 printf("▒▒");
            //             }
            //         } else {
            //             printf("%s", "██");
            //         }
            //     }
            //     printf("\n");
            // }
            // printf("\n\n\n\n\n\n\n\n\n\n\n\n");

            // // NO-OP to delay print statements.
            // for (int i = 0; i < 500000000; i++) {
            //     (void)0;
            // }

        for (int i = 0; i < 4; i++) {
            int entry = adjlist[key.value][i];

            // If entry is -1 it means there is no valid entry here (vacancy).
            if (entry != -1) {
                // If we have already visited this neighbour just continue.
                if (vertices[entry].visited == true) { continue; }

                // printf("(%d, %d)", key, end);
                int newDistance = vertices[key.value].distance + 1;//
                if (newDistance < vertices[entry].distance || vertices[entry].distance == INT_MAX) {

                    vertices[entry].distance = newDistance + 0;//heuristic();
                    // for (int i = 0; i < 4; i++) {
                    //     parents[adjlist[entry][i]] = entry;
                    // }

                    // If the neighbour is not in the priority queue already, add it.
                    
                    push(queue, entry, vertices[entry].distance);
                    // printf("(");
                    // for (int i = 0; i < queue->capacity; i++) {
                    
                    //     if (queue->array[i].value != -1) {
                    //         printf("%d, ", queue->array[i].value);
                    //     }
                    // }
                    // printf(")");
                    // printf("\n");

                }

            }
        }
        printf("\n");

    }

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
    // We may need to get xdim and ydim from here however.
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
    for (int county = 0; county < ydim; county++) {
        for (int countx = 0; countx < xdim; countx++) {
            seq = fgetc((FILE*)fp);
            if (seq == 49) {
                array[county][countx] = 1;
            } else if (seq == 48) {
                array[county][countx] = 0;
            } else if (seq == 10) {
                countx--;
            }
        }
    }

    // Verify the values have been copied over correctly.
    for (int county = 0; county < ydim; county++) {
        for (int countx = 0; countx < xdim; countx++) {
            printf("%d", array[county][countx]);
        }
        printf("\n");
    }
    // Now the array is stored in memory.

    // Each zero can only be adjacent to 4 zeroes maximum.
    int adjlist[xydim][4];
    for (int i = 0; i < xydim; i++) {
        for (int j = 0; j < 4; j++) {
            adjlist[i][j] = -1;
        }
    }
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
                } if (array[i + 1][j] == 0) {
                    // Row below.
                    adjlist[cnode][1] = (i + 1) * xdim + j;
                } if (array[i][j - 1] == 0) {
                    // Column left.
                    adjlist[cnode][2] = i * xdim + j - 1;
                } if (array[i][j + 1] == 0) {
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
    // printf("Please enter the x co-ordinate:\n");
    // scanf("%d", &x1);
    // printf("Please enter the y co-ordinate:\n");
    // scanf("%d", &y1);

    // printf("\nFrom what point should the traversal end?\n");
    // printf("Please enter the x co-ordinate:\n");
    // scanf("%d", &x2);
    // printf("Please enter the y co-ordinate:\n");
    // scanf("%d", &y2);

    //int source = y1 * xdim + x1;
    //int target = y2 * xdim + x2;
    // int source = 39;
    // int target = 68;
    int source = 20;
    int target = 68;

    BFS(source, target, adjlist, xydim);
    //ASTAR(source, target, adjlist, xydim, array, xdim);
    printf("Pathfinding complete. \n");

    //█ is for the completed path...

    // Keep the window open indefinitely.
    while (1) {

    }

    return 0;

}
