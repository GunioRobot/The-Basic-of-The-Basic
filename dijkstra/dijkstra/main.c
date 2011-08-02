//
//  main.c
//  dijkstra
//
//  Created by Guanshan Liu on 02/08/2011.
//  Copyright 2011 Guanshan Liu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#define INFINITE    9999
#define MAXVERTEX   20

unsigned int graphMatrix[MAXVERTEX][MAXVERTEX];
unsigned int pathMatrix[MAXVERTEX][MAXVERTEX];
unsigned int shortPath[MAXVERTEX];

void dijkstra(int start, int count);

void dijkstra(int start, int count) {
    int *final = (int *)malloc(sizeof(int) * count);
    for (int i = 0; i < count; i++) {
        final[i] = 0;
        shortPath[i] = graphMatrix[start][i];
        for (int j = 0; j < count; j++) {
            pathMatrix[i][j] = 0;
        }
        if (shortPath[i] < INFINITE) {
            pathMatrix[i][start] = 1;
            pathMatrix[i][i] = 1;
        }
    }
    shortPath[start] = 0;
    final[start] = 1;
    for (int i = 1; i < count; i++) { // the remaining vertices: count - 1
        int min = INFINITE;
        int closest;
        for (int j = 0; j < count; j++) {
            if (final[j] == 0) {
                if (shortPath[j] < min) {
                    closest = j;
                    min = shortPath[j];
                }
            }
        }
        final[closest] = 1;
        for (int j = 0; j < count; j++) {
            if (final[j] == 0 && (min + graphMatrix[closest][j]) < shortPath[j]) {
                shortPath[j] = (min + graphMatrix[closest][j]);
                for (int k = 0; k < count; k++) {
                    pathMatrix[j][k] = pathMatrix[closest][k];
                }
                pathMatrix[j][j] = 1;
            }
        }
    }
}

int main() {
    // Test/implementation code goes here.
    return 0;
}