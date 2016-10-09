// Copyright William Jagels 2016

#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

int str_manip(char *str, char *substr) {
    size_t str_len = strlen(str);
    size_t substr_len = strlen(substr);
    printf("str %s\n", str);
    char low[str_len + 1];
    strncpy(low, str, str_len + 1);
    for (int i = 0; i < str_len; i++) {
        low[i] = tolower(low[i]);
    }
    char rev[str_len * 2 + 1];
    rev[0] = '\0';
    strcat(rev, low);
    for (int i = 1; i <= str_len; i++) {
        rev[str_len - 1 + i] = low[str_len - i];
    }
    rev[str_len * 2] = '\0';
    printf("newstr %s\n", rev);
    printf("substr %s\n", substr);
    char substr_low[substr_len + 1];
    strncpy(substr_low, substr, substr_len + 1);
    for (int i = 0; i < substr_len; i++) {
        substr_low[i] = tolower(substr_low[i]);
    }
    int ocr = 0;
    for (char *s = rev; (s = strstr(s, substr_low)); ocr++, s++)
        ;
    printf("occurences %d\n", ocr);
    return 0;
}
