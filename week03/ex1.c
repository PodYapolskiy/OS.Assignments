#include <stdio.h>
#include <stdlib.h>

int const_tri(int* p, int n) {
    if (n <= 2) {
        return *(p + n);
    }
    return const_tri(p, n - 1) + const_tri(p, n - 2) + const_tri(p, n - 3);
}

int main() {
    const int x = 1;
    const int* q = &x;

    int* p = (int*) malloc(3 * sizeof(int)); 
    // try to use q pointer somewhere (it's useless) otherwise)
    *p = *q; 
    *(p + 1) = *q;
    *(p + 2) = *q * 2;
    
    long int pace = sizeof(int);
    printf("%ld\n", pace); // addresses pace
    for (int i = 0; i < 3; i++) {
        printf("%p\n", p + i);
    }
    for (int i = 0; i < 2; i++) {
        if ((p + 1) - (p) == pace) {
            return 1; // error
        }
    }

    printf("%d\n", const_tri(p, 4)); // 7

    free(p);
    return 0;
}