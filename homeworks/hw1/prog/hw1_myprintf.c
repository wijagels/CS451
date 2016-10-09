#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int myprintf(const char* format, ...) {
    size_t f_len = strlen(format);
    for (int k = 0; k < 2; k++) {
        if (k == 1) printf("\nArgument List:\n");
        va_list args;
        va_start(args, format);
        for (size_t i = 0; i <= f_len; i++) {
            if (format[i] == '%') {
                char fmt = format[i + 1];
                i++;  // Don't print the format specifier
                switch (fmt) {
                    case 'c': {
                        char chr = va_arg(args, int);
                        if (k == 0)
                            printf("%c", chr);
                        else
                            printf("Char-->%c\n", chr);
                        break;
                    }
                    case 'd': {
                        int num = va_arg(args, int);
                        if (k == 0)
                            printf("%d", num);
                        else
                            printf("Integer-->%d\n", num);
                        break;
                    }
                    case 's': {
                        const char* str = va_arg(args, const char*);
                        if (k == 0)
                            printf("%s", str);
                        else
                            printf("String-->%s\n", str);
                        break;
                    }
                    default:
                        return -1;
                }
            } else {
                if (k == 0) printf("%c", format[i]);
            }
        }
    }
    return 0;
}
