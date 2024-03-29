#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <math.h>
#include <float.h>

#define SIZE 5

void* aggregate(
    void* base,
    size_t size,
    int n,
    void* initial_value,
    void* (*opr)(const void*, const void*)
) {
    void* answer = initial_value;
    if (sizeof(int) == size) {
        for (int i = 0; i < n; i++) {
            answer = opr(&((int*) base)[i], (int*) answer);
            free(initial_value);
            initial_value = answer;
        }
    } else if (sizeof(double) == size) {
        for (int i = 0; i < n; i++) {
            answer = opr(&((double*) base)[i], (double*) answer);
            free(initial_value);
            initial_value = answer;
        }
    }
    return initial_value;
}

void* add_int(const void* a, const void* b) {
    int* result = (int*) malloc(sizeof(int));
    *result = *((int*) a) + *((int*) b);
    return result;
}

void* add_double(const void* a, const void* b) {
    double* result = (double*) malloc(sizeof(double));
    *result = *((double*) a) + *((double*) b);
    return result;
}

void* multiply_int(const void* a, const void* b) {
    int* result = (int*) malloc(sizeof(int));
    *result = *((int*) a) * *((int*) b);
    return result;
}

void* multiply_double(const void* a, const void* b) {
    double* result = (double*) malloc(sizeof(double));
    *result = *((double*) a) * *((double*) b);
    return result;
}

void* max_int(const void* a, const void* b) {
    int* result = (int*) malloc(sizeof(int));
    *result = *(int*) a > *(int*) b ? *(int*) a : *(int*) b;
    return result;
}

void* max_double(const void* a, const void* b) {
    double* result = (double*) malloc(sizeof(double));
    *result = *(double*) a > *(double*) b ? *(double*) a : *(double*) b;
    return result;
}

int main() {
    int arr1[SIZE] = {5, 4, 3, 2, 1};
    double arr2[SIZE] = {1.1, 2.8, 3.1, 4.5, 5.9};

    int* int_sum_p = (int*) malloc(sizeof(int));
    int_sum_p[0] = 0;

    int* int_mul_p = (int*) malloc(sizeof(int));
    int_mul_p[0] = 1;

    int* int_max_p = (int*) malloc(sizeof(int));
    int_max_p[0] = INT_MIN;

    double* double_sum_p = (double*) malloc(sizeof(double));
    double_sum_p[0] = 0.0;

    double* double_mul_p = (double*) malloc(sizeof(double));
    double_mul_p[0] = 1.0;

    double* double_max_p = (double*) malloc(sizeof(double));
    double_max_p[0] = -DBL_MAX; // real crutch (✨love C proramming✨)

    int* int_sum = aggregate(arr1, sizeof(int), SIZE, int_sum_p, add_int);
    int* int_mul = aggregate(arr1, sizeof(int), SIZE, int_mul_p, multiply_int);
    int* int_max = aggregate(arr1, sizeof(int), SIZE, int_max_p, max_int);

    double* double_sum = aggregate(arr2, sizeof(double), SIZE, double_sum_p, add_double);
    double* double_mul = aggregate(arr2, sizeof(double), SIZE, double_mul_p, multiply_double);
    double* double_max = aggregate(arr2, sizeof(double), SIZE, double_max_p, max_double);

    printf("%d %d %d\n", *int_sum, *int_mul, *int_max);
    printf("%f %f %f\n", *double_sum, *double_mul, *double_max);

    free(int_sum);
    free(int_mul);
    free(int_max);
    free(double_sum);
    free(double_mul);
    free(double_max);

    return 0;
}