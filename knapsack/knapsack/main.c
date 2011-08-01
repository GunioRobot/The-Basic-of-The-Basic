//
//  main.c
//  knapsack
//
//  Created by Guanshan Liu on 01/08/2011.
//  Copyright 2011 Guanshan Liu. All rights reserved.
//
//  Based on osa9's n4.c
// 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREADS  2

#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))

typedef struct {
    int tid; // thread id
    
    int *values;
    int *weights;
    int objectNum;
    int capacity;
} threadargs;

typedef threadargs *threadargs_t;

int **matrix;

int sync_progress = 0;
pthread_mutex_t sync_mut  =  PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  sync_cond =  PTHREAD_COND_INITIALIZER;

int thread_create(void *(*start_func)(void *), void *arg);
void barrier_sync(int num_process, int pid);
void knapsack_thread(int tid, int *values, int *weights, int num, int capcity);
void *exec_thread(void *param);
int knapsack(int *values, int *weights, int num, int capcity);

int thread_create(void *(*start_func)(void *), void *arg){
    int status = 0;
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    status = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    if(status == 0) {
        status = pthread_create(&tid, &attr, start_func, arg);
    }
    if(status != 0) {
        status = pthread_create(&tid, 0, start_func, arg);
    }
    return status;
}

void barrier_sync (int num_process, int pid) {
    pthread_mutex_lock(&sync_mut);
    
    if (sync_progress + 1 < num_process) {
        ++sync_progress;
        pthread_cond_wait(&sync_cond, &sync_mut);
    }
    else{
        pthread_cond_broadcast(&sync_cond);
    }
    
    pthread_mutex_unlock(&sync_mut);
}

void knapsack_thread(int tid, int *values, int *weights, int num, int capcity) {
    weights--;
    values--;
    
    for(int i = 1; i <= num; i++) {
        int t = capcity / NUM_OF_THREADS + 1;
        for (int j = t * tid + 1 ; j <=  MIN(capcity, t * (tid + 1)); j++) {
            int v = (j-weights[i]>=0) ? values[i] + matrix[i-1][j-weights[i]] : 0;
            matrix[i][j] = MAX(matrix[i-1][j], v);
        }

        barrier_sync(NUM_OF_THREADS, tid);
    }
}

void *exec_thread(void *param) {
    threadargs_t p = (threadargs_t)param;
    knapsack_thread(p->tid, p->values, p->weights, p->objectNum, p->capacity);
    return  NULL ;
}

int knapsack(int *values, int *weights, int num, int capcity) {
    
    for(int i = 0; i <= capcity; ++i) {
        matrix[0][i] = 0;
    }
    
    for(int i = 1; i <= num; ++i) {
        matrix[i][0] = 0;
    }

    threadargs_t p = (threadargs_t)malloc(sizeof(threadargs) * NUM_OF_THREADS);
    
    sync_progress = 0;
    
    for (int i = 0; i < NUM_OF_THREADS; ++i) {
        p[i].tid = i;
        p[i].values = values;
        p[i].weights = weights;
        p[i].objectNum = num;
        p[i].capacity = capcity;
    }
    
    for (int i = 0; i < NUM_OF_THREADS - 1; ++i)
        thread_create(exec_thread, &p[i]);
    
    exec_thread((void *)&p[NUM_OF_THREADS - 1]);
    
    return matrix[num][capcity];
}

int main(){
    
    int *values;
    int *weights;
    
    int capacity;
    int objectNum;
    
    printf("Please enter the capacity of the knapsack: ");
    scanf("%d", &capacity);
    printf("Please enter the number of objects: ");
    scanf("%d",&objectNum);
        
    values = (int *)malloc(sizeof(int) * objectNum);
    weights = (int *)malloc(sizeof(int) * objectNum);
    
    matrix = (int **)malloc(sizeof(int *) * (objectNum + 1));
        
    for(int i = 0;i < objectNum; ++i){
        matrix[i] = (int *)malloc(sizeof(int) * (capacity + 1));
        printf("weight and value pair: ");
        scanf ("%d %d" , &weights[i], &values[i]);
    }
    matrix[objectNum] = (int *)malloc(sizeof(int) * (capacity + 1));
    
    printf("Result: %d\n", knapsack(values, weights, objectNum, capacity));
    
    free(values);
    free(weights);
    for (int i = 0; i <= objectNum; i++) {
        free(matrix[i]);
    }
    free(matrix);
    
    return 0;
}
