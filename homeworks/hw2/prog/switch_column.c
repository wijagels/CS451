#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 64

int sw_col(char*, int, int);
int readline(char*, size_t, int);

int main(int argc, char* argv[]) {
    if (argc != 4) return 1;
    return sw_col(argv[1], atoi(argv[2]), atoi(argv[3]));
}

int sw_col(char* fname, int first, int second) {
    if (first > second) return sw_col(fname, second, first);
    int read_fd = open(fname, O_RDONLY);
    int write_fd = open(fname, O_WRONLY | O_APPEND);
    if (read_fd == -1 || write_fd == -1) return 1;
    char l_buf[LINE_SIZE];
    char o_buf[LINE_SIZE];
    int r;
    write(write_fd, "===\n", 4);
    while ((r = readline(l_buf, LINE_SIZE, read_fd)) > 0) {
        printf("%s\n", l_buf);
        if (strncmp(l_buf, "===", 3) == 0) break;
        int cols = 1;
        strncpy(o_buf, l_buf, LINE_SIZE);
        strtok(o_buf, " \t");
        while (strtok(NULL, " \t")) cols++;
        if (cols < second) {
            write(write_fd, l_buf, r);
        } else {
            strncpy(o_buf, l_buf, LINE_SIZE);
            char* entry = strtok(o_buf, " \t");
            for (int i = 0; i < first - 1; i++) {
                fprintf(stderr, "%s\n", entry);
                write(write_fd, entry, strlen(entry));
                write(write_fd, " ", 1);
                entry = strtok(NULL, " \t");
            }
            strncpy(o_buf, l_buf, LINE_SIZE);
            entry = strtok(o_buf, " \t");
            // Write first swapped
            for (int i = 0; i < second - 1; i++) entry = strtok(NULL, " \t");
            write(write_fd, entry, strlen(entry));
            write(write_fd, " ", 1);

            strncpy(o_buf, l_buf, LINE_SIZE);
            strtok(o_buf, " \t");
            for (int i = 0; i < first - 1; i++) strtok(NULL, " \t");
            for (int i = first; i < second - 1; i++) {
                char* entry = strtok(NULL, " \t");
                write(write_fd, entry, strlen(entry));
                write(write_fd, " ", 1);
            }

            strncpy(o_buf, l_buf, LINE_SIZE);
            strtok(o_buf, " \t");
            // Write second swapped
            for (int i = 0; i < first - 1; i++) entry = strtok(NULL, " \t");
            write(write_fd, entry, strlen(entry));
            write(write_fd, " ", 1);

            strncpy(o_buf, l_buf, LINE_SIZE);
            strtok(o_buf, " \t");
            for (int i = 0; i < second - 1; i++) strtok(NULL, " \t");
            for (int i = second; i < cols; i++) {
                char* entry = strtok(NULL, " \t");
                write(write_fd, entry, strlen(entry));
                write(write_fd, " ", 1);
            }
        }
        write(write_fd, "\n", 1);
    }
    close(write_fd);
    close(read_fd);
    return 0;
}

/*
 * Basically just a crappy version of getline(3)
 */
int readline(char* buf, size_t count, int fd) {
    ssize_t bytes = read(fd, buf, count);
    if (bytes <= 0) return bytes;
    ssize_t i = 0;
    for (; i < bytes && buf[i] != '\n'; i++)
        ;
    // Move the offset back.
    lseek(fd, i - bytes + 1, SEEK_CUR);
    buf[i] = '\0';
    return i;
}
