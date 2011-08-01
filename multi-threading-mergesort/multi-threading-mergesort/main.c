//
//  main.c
//  multi-threading-mergesort
//
//  Created by Guanshan Liu on 01/08/2011.
//  Copyright 2011 Guanshan Liu. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_COUNT           100
#define NUM_UPPER_BOUNDARY  1000

typedef struct {
    unsigned int* array;
    int left;
    int right;
} threadargs;

typedef threadargs *threadargs_t;

void fill_random_array(unsigned int* numbers, int count);
void print_slice(unsigned int* numbers, int start, int end);
void print_numbers(unsigned int* numbers, int count);
void swap_numbers(unsigned int *a, unsigned int *b);
void *mergesort_fn(void *args);
void merge_sort(unsigned int *numbers, int left, int right);

void fill_random_array(unsigned int* numbers, int count) {
    for (int i = 0; i < count; i++) {
        numbers[i] = arc4random() % NUM_UPPER_BOUNDARY;
    }
}

void print_slice(unsigned int* numbers, int start, int end) {
    for (int i = start; i <= end; i++) {
        printf("%8d\n", numbers[i]);
    }
}

void print_numbers(unsigned int* numbers, int count) {
    print_slice(numbers, 0, count - 1);
}

void swap_numbers(unsigned int *a, unsigned int *b) {
    unsigned int t = *a;
    *a = *b;
    *b = t;
}

void *mergesort_fn(void *args) {
    threadargs_t threadargs = (threadargs_t)args;
    merge_sort(threadargs->array, threadargs->left, threadargs->right);
    return NULL;
}

void merge_sort(unsigned int *numbers, int left, int right) {
    if (left < right) {  
        int middle = (left + right) / 2;
        pthread_t left_thread, right_thread;
        threadargs_t left_args, right_args;
        
        left_args = (threadargs_t)malloc(sizeof(threadargs));
        right_args = (threadargs_t)malloc(sizeof(threadargs));
        
        left_args->array = numbers;
        left_args->left = left;
        left_args->right = middle;
        
        right_args->array = numbers;
        right_args->left = middle + 1;
        right_args->right = right;
        
        pthread_create(&left_thread, NULL, mergesort_fn, (void *)left_args);
        pthread_create(&right_thread, NULL, mergesort_fn, (void *)right_args);
        
        pthread_join(left_thread, NULL);
        pthread_join(right_thread, NULL);
        
        int i = left;
        int j = middle + 1;
        int k = right - left + 1;
        unsigned int *temp = (unsigned int*)malloc(sizeof(unsigned int) * k);
        for (k = 0; i <= middle && j <= right; k++) {
            if (numbers[i] <= numbers[j]) {
                temp[k] = numbers[i];
                i++;
            }
            else {
                temp[k] = numbers[j];
                j++;
            }
        }
        while (i <= middle) {
            temp[k] = numbers[i];
            i++;
            k++;
        }
        while (j <= right) {
            temp[k] = numbers[j];
            j++;
            k++;
        }
        for (k = 0; k <= right - left; k++) {
            numbers[left + k] = temp[k];
        }
        free(temp);
        free(left_args);
        free(right_args);
    }
}

int main(int argc, char *argv[]) {
    srand((unsigned int) time(0));
    
    unsigned int numbers[MAX_COUNT];
    fill_random_array(numbers, MAX_COUNT);
    printf("Fill random numbers...\n");
    print_numbers(numbers, MAX_COUNT);
    printf("\n\nStart sorting...\n");
    
    merge_sort(numbers, 0, MAX_COUNT - 1);
    
    printf("\n\nFinished.\n\nNow print the result...\n");
    print_numbers(numbers, MAX_COUNT);
    
    return 0;
}
