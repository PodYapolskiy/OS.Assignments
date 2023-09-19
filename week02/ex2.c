#include <stdio.h>
#include <string.h>

int main() {
    char c;
    char str[256];

    int i = 0;
    while ((c = getchar()) != '.') {
        if (c == '\n') return 0; // terminating character
        str[i++] = c;
    }

    printf("\"");
    for (int i = strlen(str) - 1; i >= 0; i--) {
        printf("%c", str[i]);
    }
    printf("\"");

    return 0;
}
