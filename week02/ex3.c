#include <stdio.h>
#include <math.h>

long long int convert(long long int num, int src, int trg) {
    long long int res = 0; // result
    long long int dec = 0; // decimal
    int i = 0;
    
    while (num > 0) {
        dec += (num % 10) * pow(src, i++);
        num /= 10;
    }
    
    i = 0;
    while (dec > 0) {
        res += (dec % trg) * pow(10, i++);
        dec /= trg;
    }
    return res;
}

int main() {
    long long int number;
    int source, target;

    scanf("%lld", &number);
    scanf("%d", &source);
    scanf("%d", &target);

    if (!(2 < source < 10) || !(2 < target < 10)) {
        printf("cannot convert!\n");
        return 0;
    }

    printf("%lld\n", convert(number, source, target));
    return 0;
}