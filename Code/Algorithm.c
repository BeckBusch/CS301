// Includes.
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
#define UNINITIALISED 4

#define L 0
#define R 1
#define NULLDIR -1

struct PriorityVertex {
    int16_t value;
    uint16_t priority;
} PriorityVertex;

struct Queue {
    int16_t start, end, size;
    unsigned capacity;
    struct PriorityVertex *array;
} Queue;

struct Vertex {
    int16_t pred;
    uint16_t distance;
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

void init(struct Queue* queue, uint16_t size) {

    // Initialise all values to minus one so that we can tell if the queue is empty or not (there is a vertex zero).
    for (uint16_t i = 0; i < size; i++) {
        struct PriorityVertex defaultValue = { .value = -1, .priority = UINT16_MAX, };
        queue->array[i] = defaultValue;
    }

}

void push(struct Queue* queue, int16_t vertex, uint16_t priority) {

    // Move the end marker along one, if we reach capacity, go back to the beginning.
    queue->end = (queue->end + 1) % queue->capacity;
    struct PriorityVertex newVertex = { .value = vertex, .priority = priority };
    queue->array[queue->end] = newVertex;
    queue->size = queue->size + 1;

}

int16_t pop(struct Queue* queue) {

    // Get the vertex from the start of the queue.
    struct PriorityVertex vertex = queue->array[queue->start];
    // Reset to empty value of -1.
    struct PriorityVertex defaultValue = { .value = -1, .priority = UINT16_MAX };
    queue->array[queue->start] = defaultValue;
    // Move the start marker along one, if we reach capacity, go back to the beginning.
    queue->start = (queue->start + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return vertex.value;

}

void delete(struct Queue* queue, uint16_t pos) {

    // Replace each entry in the array with the next entry until the end.
    for (uint16_t i = 0; i < queue->end - pos; i++) {
        queue->array[pos + i] = queue->array[pos + i + 1];
    }
    // Move the end pointer over.
    queue->end--;
    queue->size--;

}

bool queueContains(struct Queue* queue, int16_t vertex) {

    // If the queue contains the vertex return true, else return false.
    for (uint16_t i = 0; i < queue->size; i++) {
        if (queue->array[queue->start + i].value == vertex) {
            return true;
        }
    }
    return false;
}

uint16_t heuristic(int16_t entry, uint16_t target, uint16_t xdim) {

    // Calculate square of horizontal using modulo.
    uint16_t horiz = (target % xdim - entry % xdim) * (target % xdim - entry % xdim);

    // Calculate square of vertical using division (truncation).
    uint16_t vert = (target / xdim - entry / xdim) * (target / xdim - entry / xdim);

    return (horiz + vert) / 10;

}

struct PriorityVertex removeVertex(struct Queue* queue) {

    // This function operates like pop but differs in that the priority queue is out of order.
    uint16_t maxPriority = UINT16_MAX;
    uint16_t pos;
    for (uint16_t i = 0; i < queue->size; i++) {
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

// Implementation of the A* algorithm.
uint16_t *ASTAR(uint16_t source, uint16_t target, int16_t adjlist[][4], uint16_t xdim, uint16_t ydim, uint16_t array[15][19]) {

    // Dimension.
    uint16_t size = xdim * ydim;

    // Set up the queue.
    struct Queue* queue = makeQueue(size);
    init(queue, size);
    push(queue, source, 0);

    // Set up an array to track vertices and their distances (INT_MAX means a vertex has yet to be visited).
    struct Vertex vertices[size];
    for (uint16_t i = 0; i < size; i++) {

        // We don't care about vertex colour for this implementation.
        vertices[i].pred = -1;
        vertices[i].distance = UINT16_MAX;
        vertices[i].visited = false;

    }

    // In our implementation we should count the source block as having a distance of 1.
    vertices[source].distance = 1;

    // While we have yet to reach the destination, keep looping.
    while (queue->size != 0) {

        // Get the vertex with the highest priority.
        struct PriorityVertex key = removeVertex(queue);

        // Set the visited status to true.
        vertices[key.value].visited = true;

        // If we have reached our target, then we can stop searching.
        if (key.value == target) {

            // printf("The target is %d blocks from the source.\n", vertices[target].distance );

            // Initialise an array to store the shortest path (with blanks).
            int16_t shortestPath[size];
            for (uint16_t i = 0; i < size; i++) {
                shortestPath[i] = -1;
            }

            // Index and order are both required for successful formatting.
            uint16_t index = target;
            uint16_t order = vertices[target].distance;

            while (vertices[index].pred != source) { 
                // Order is very important here, we need it to make sure that the order we visit vertices in is preserved.
                shortestPath[index] = order;
                index = vertices[index].pred;
                order--;
            }
            
            // Add the last 2 vertices.
            shortestPath[index] = order;
            order--;
            shortestPath[source] = order;

            // Array to return. Need to allocate space for an additional int so we can store the size as well.
            uint16_t *finalPath = malloc((vertices[target].distance + 1) * sizeof(uint16_t));
            for (uint16_t i = 0; i < size; i++) {
                if (shortestPath[i] != -1) {
                    order = shortestPath[i];
                    finalPath[order] = i;
                }
            }

            // Set the size at position 0.
            finalPath[0] = vertices[target].distance;

            // Return the formatted shortest path.
            return finalPath;

        }

        // Search through all possible neighbours.
        for (uint8_t i = 0; i < 4; i++) {
            int16_t entry = adjlist[key.value][i];

            // If entry is -1 it means there is no valid entry here (vacancy).
            if (entry != -1) {
                // If we have already visited this neighbour just continue.
                if (vertices[entry].visited == true) { continue; }

                // Update the distances.
                uint16_t newDistance = vertices[key.value].distance + 1;
                if (newDistance < vertices[entry].distance) {

                    // Priority is the sum of the new distance plus the heuristic function result.
                    uint16_t priority = newDistance + heuristic(entry, target, xdim);
                    vertices[entry].distance = newDistance;

                    // Set the predecessor (we want to take the newly discovered shortest path to entry vertex).
                    vertices[entry].pred = key.value;

                    if (!queueContains(queue, entry)) {
                        // If the neighbour is not in the priority queue already, add it.
                        push(queue, entry, priority);
                    }

                }
            }
        }

    }

}

// Decode an array of vertices representing the shortest path into a list of directions.
int8_t *decode(uint16_t *finalPath, uint16_t xdim, uint16_t target) {

    // Get the length of the final path and store it in size.
    uint16_t size = finalPath[0];
    
    // Allocate space for our return array.
    int8_t *instructionSet = malloc(size * sizeof(int8_t));
    for (uint16_t i = 0; i < size; i++) {
        instructionSet[i] = NULLDIR;
    }

    // Track the current and previous directions, and array indices.
    uint16_t i = 1, j = 0;
    uint8_t traversalDirection = UNINITIALISED;
    uint8_t prevDirection = UNINITIALISED;

    // While we have yet to reach the target vertex, walk through the shortest path, keeping track of the direction we are moving in.
    while (finalPath[i] != target) {
        if (finalPath[i] - finalPath[i + 1] == xdim) {
            traversalDirection = NORTH;
        } else if (finalPath[i + 1] - finalPath[i] == xdim) {
            traversalDirection = SOUTH;
        } else if (finalPath[i + 1] - finalPath[i] == 1) {
            traversalDirection = EAST;
        } else if (finalPath[i] - finalPath[i + 1] == 1) {
            traversalDirection = WEST;
        }

        /* If we have just started traversing, then we cannot have made a turn yet, so prevDirection needs to be initialised.
           If the previous direction is different from our current one, then we have to had made a turn. */
        if ((traversalDirection != prevDirection) && (prevDirection != UNINITIALISED)) {

            // We can determine whether we turned left or right based on our previous and current directions.
            switch (traversalDirection) 
            {
                case NORTH:
                    if (prevDirection == EAST) {
                        instructionSet[j] = L;
                    } else if (prevDirection == WEST) {
                        instructionSet[j] = R;
                    }
                    break;

                case EAST:
                    if (prevDirection == NORTH) {
                        instructionSet[j] = R;
                    } else if (prevDirection == SOUTH) {
                        instructionSet[j] = L;
                    }
                    break;

                case SOUTH:
                    if (prevDirection == EAST) {
                        instructionSet[j] = R;
                    } else if (prevDirection == WEST) {
                        instructionSet[j] = L;
                    }
                    break;

                case WEST:
                    if (prevDirection == NORTH) {
                        instructionSet[j] = L;
                    } else if (prevDirection == SOUTH) {
                        instructionSet[j] = R;
                    }
                    break;

                default:
                    break;
            }
            // Increment the index so that we can store the next instruction (if applicable).
            j++;

        }
        // Set the previous direction to our current one, and increment our position in the shortest path.
        prevDirection = traversalDirection;
        i++;

    }
    // Return the set of instructions produced.
    return instructionSet;

}

int main() {

    // Store the dimensions of the map so we can't go outside of it later on. Hard coding this.
    uint16_t xdim = 19, ydim = 15;
    uint16_t xydim = xdim * ydim;

    // Copy paste the map here - make sure to append backslashes after each line.
    uint16_t array[ydim][xdim];
    char arrayString[] =   "1111111111111111101\
                            1000001000000010001\
                            1110101010111010101\
                            1010000010100000101\
                            1010101110101110101\
                            1000100000100000101\
                            1011111110001110101\
                            1000100000100010101\
                            1110001111101010101\
                            1000100000101000001\
                            1011111110001111101\
                            1000001000101000101\
                            1111101011101010101\
                            0000000010000010001\
                            1111111111111111111";

    uint16_t i = 0;
    uint8_t j, k = 0;

    // Convert the string to an array so we can use it.
    while (arrayString[i] != '\0') {
        if (arrayString[i] == 48) {
            array[j][k] = 0;
            if (k == xdim - 1) {
                k = 0;
                j++;
            } else {
                k++;
            }
        } else if (arrayString[i] == 49) {
            array[j][k] = 1;
            if (k == xdim - 1) {
                k = 0;
                j++;
            } else {
                k++;
            }
        }
        i++;
    }

    // Each zero can only be adjacent to 4 zeroes maximum.
    int16_t adjlist[xydim][4];
    for (uint16_t i = 0; i < xydim; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            adjlist[i][j] = -1;
        }
    }
    uint16_t cnode;

    /* Construct the adjacency list.
       Numbered reading from left to right, top down. */
    for (uint16_t i = 0; i < ydim; i++) {
        for (uint16_t j = 0; j < xdim; j++) {
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

    // Source x and y co-ordinates.
    uint16_t sxcord = 1;
    uint16_t sycord = 1;

    // Target x and y co-ordinates.
    uint16_t txcord = 16;
    uint16_t tycord = 13;

    // The offset value - if we are indexing starting at 0, this should be 0, if we are indexing starting at 1, this should be 1 etc.
    uint8_t offset = 0;

    // Calculation for the source and target co-ordinates.
    uint16_t source = ((sycord - offset) * xdim + sxcord - offset);
    uint16_t target = ((tycord - offset) * xdim + txcord - offset);

    uint16_t *finalPath = ASTAR(source, target, adjlist, xdim, ydim, array);
    int8_t *instructionSet = decode(finalPath, xdim, target);

    // Print statements for debugging - this is the output we use for turning.
    for (uint16_t i = 0; i < finalPath[0] + 1; i++) {
        if (instructionSet[i] == L) {
            printf("L");
        }
        if (instructionSet[i] == R) {
            printf("R");
        }
    }
    printf("\n");

    // Keep the window open indefinitely.
    while (1) {

    }

    return 0;

}
