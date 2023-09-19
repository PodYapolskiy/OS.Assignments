// https://stackoverflow.com/questions/2053843/min-and-max-value-of-data-type-in-c
#include <stdio.h>
#include <limits.h>
#include <float.h>

int main() {
	int a = INT_MAX;
	unsigned short int b = USHRT_MAX;
	signed long int c = LONG_MAX;
	float d = FLT_MAX;
	double e = DBL_MAX;

	printf("%ld %d\n", sizeof(a), a);
	printf("%ld %d\n", sizeof(b), b);
	printf("%ld %ld\n", sizeof(c), c);
	printf("%ld %f\n", sizeof(d), d);
	printf("%ld %f\n", sizeof(e), e);

	return 0;
}
