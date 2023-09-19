#include <stdio.h>

int tribonacci(int n) {
    int x1 = 0, x2 = 1, x3 = 1, i = 3;
    while (i++ <= n) {
        int temp = x1 + x2 + x3;
        x1 = x2;
        x2 = x3;
        x3 = temp;
    }
    return x3;
}

int main() {
    printf("tribonacci(4) = %d\n", tribonacci(4));
    printf("tribonacci(36) = %d\n", tribonacci(36));
    return 0;
}
