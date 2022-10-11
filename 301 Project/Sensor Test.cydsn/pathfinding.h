/* ========================================
 * motors.h
 * header file for motors.c
 * ========================================
*/

#ifndef PATHFINDING_H_
#define PATHFINDING_H_
    
#include "project.h"
    
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
#define J 2
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

struct Queue* makeQueue(unsigned size);
void init(struct Queue* queue, uint16_t size);
void push(struct Queue* queue, int16_t vertex, uint16_t priority);
int16_t pop(struct Queue* queue);
void delete(struct Queue* queue, uint16_t pos);
bool queueContains(struct Queue* queue, int16_t vertex);
uint16_t heuristic(int16_t entry, uint16_t target, uint16_t xdim);
struct PriorityVertex removeVertex(struct Queue* queue);
uint16_t *ASTAR(uint16_t source, uint16_t target, int16_t adjlist[][4], uint16_t xdim, uint16_t ydim, uint16_t array[15][19]);
int8_t *decode(uint16_t *finalPath, int16_t adjlist[][4], uint16_t xdim, uint16_t target);

#endif /* PATHFINDING_H_ */
/* [] END OF FILE */
