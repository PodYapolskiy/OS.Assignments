#include <stdio.h>
#include <math.h>

typedef struct {
    double x;
    double y;
} Point;

double distance(Point* p1, Point* p2) {
    return sqrt(pow(p2->y - p1->y, 2) + pow(p2->x - p1->x, 2));
}

double area(Point* p1, Point* p2, Point* p3) {
    return fabs(p1->x * p2->y - p2->x * p1->y + p2->x * p3->y - p3->x * p2->y + p3->x * p1->y - p1->x * p3->y) / 2;
}

int main() {
    Point A = {.x = 2.5, .y = 6.0};
    Point B = {.x = 1.0, .y = 2.2};
    Point C = {.x = 10.0, .y = 6.0};

    printf("%f\n", distance(&A, &B));
    printf("%f\n", area(&A, &B, &C));

    return 0;
}