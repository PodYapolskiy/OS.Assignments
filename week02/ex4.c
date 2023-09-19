#include <stdio.h>
#include <ctype.h>
#include <string.h>

int count(char str[], char c) {
    int count = 0;

    for (int i = 0; i < strlen(str); i++) {
        if (tolower(str[i]) == c) {
            count++;
        }
    }
    
    return count;
}

void countAll(char str[]) {
    for (int i = 0; i < strlen(str); i++) {
        char c = tolower(str[i]);
        printf("%c:%d", c, count(str, c));
        if (i != strlen(str) - 1) {
            printf(", ");
        }
    }
    printf("\n");
}

int main() {
    char str[257];

    scanf("%s", str);
    countAll(str);

    return 0;
}