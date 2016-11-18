#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "usr_functions.h"

#define INIT_LINE_SIZE 256
#define BLOCK_SIZE 4096

inline int64_t min(int64_t r, int64_t l) { return r < l ? r : l; }

/*
   User-defined map function for the "Letter counter" task.
   This map function is called in a map worker process.
   @param split: The data split that the map function is going to work on.  Note
   that the file offset of the file descriptor split->fd should be set to the
   properly position when this map function is called.
   @param fd_out: The file descriptor of the intermediate data file output by
   the map function.
   @ret: 0 on success, -1 on error.
 */
int letter_counter_map(DATA_SPLIT* split, int fd_out) {
    int freq[26] = {0};  // Zero initialize
    FILE* fp = fdopen(split->fd, "r");
    FILE* wp = fdopen(fd_out, "w");
    if (!fp || !wp) return 1;
    char* str_buf = malloc(BLOCK_SIZE * sizeof(*str_buf));
    if (!str_buf) return 2;
    int64_t read_bytes = 0;
    size_t rd_sz;
    while (read_bytes < split->size &&
           (rd_sz = fread(str_buf, 1,
                          min(BLOCK_SIZE, (split->size - read_bytes)), fp)) >
               0) {
        read_bytes += rd_sz;
        for (size_t i = 0; i < rd_sz; i++) {
            char c = str_buf[i];
            if (isalpha(c)) {
                c = toupper(c);
                freq[c - 'A']++;
            }
        }
    }
    for (int i = 0; i < 26; i++) {
        fprintf(wp, "%c %d\n", 'A' + i, freq[i]);
    }
    fflush(wp);
    fclose(fp);
    free(str_buf);
    return 0;
}

/*
   User-defined reduce function for the "Letter counter" task.
   This reduce function is called in a reduce worker process.
   @param p_fd_in: The address of the buffer holding the intermediate data
   files' file descriptors.  The intermediate data files are output by the map
   worker processes, and they are the input for the reduce worker process.
   @param fd_in_num: The number of the intermediate files.
   @param fd_out: The file descriptor of the final result file.
   @ret: 0 on success, -1 on error.
   @example: if fd_in_num == 3, then there are 3 intermediate files, whose file
   descriptor is identified by p_fd_in[0], p_fd_in[1], and p_fd_in[2]
   respectively.

*/
int letter_counter_reduce(int* p_fd_in, int fd_in_num, int fd_out) {
    int freq[26] = {0};
    FILE* wp = fdopen(fd_out, "w");
    if (!wp) return 1;
    for (int i = 0; i < fd_in_num; i++) {
        FILE* fp = fdopen(p_fd_in[i], "r");
        if (!fp) return 1;
        rewind(fp);
        clearerr(fp);
        while (!feof(fp)) {
            char c;
            int num;
            int read = fscanf(fp, "%c %d\n", &c, &num);
            if (read < 2) _EXIT_ERROR(1, "Corrupted map file %d\n", p_fd_in[i]);
            freq[c - 'A'] += num;
        }
        fclose(fp);
    }

    for (int i = 0; i < 26; i++) {
        fprintf(wp, "%c %d\n", 'A' + i, freq[i]);
    }
    fclose(wp);
    return 0;
}

/*
   User-defined map function for the "Word finder" task.
   This map function is called in a map worker process.
   @param split: The data split that the map function is going to work on.  Note
   that the file offset of the file descriptor split->fd
   should be set to the properly position when this map function is called.
   @param fd_out: The file descriptor of the intermediate data file output by
   the map function.
   @ret: 0 on success, -1 on error.
 */
int word_finder_map(DATA_SPLIT* split, int fd_out) {
    FILE* fp = fdopen(split->fd, "r");
    FILE* wp = fdopen(fd_out, "w");
    if (!fp || !wp) return 1;
    char* line_buf = malloc(INIT_LINE_SIZE * sizeof(*line_buf));
    if (!line_buf) {
        _EXIT_ERROR(2, "Malloc of size %lu failed, halting.\n",
                    INIT_LINE_SIZE * sizeof(*line_buf));
    }
    ssize_t read_total = 0;
    ssize_t read;
    size_t n = INIT_LINE_SIZE;
    while (read_total < split->size &&
           (read = getline(&line_buf, &n, fp)) != -1) {
        read_total += read;
        if (strstr(line_buf, split->usr_data)) {
            fprintf(wp, "%s", line_buf);
        }
    }
    fflush(wp);
    fclose(fp);

    free(line_buf);
    return 0;
}

/*
   User-defined reduce function for the "Word finder" task.
   This reduce function is called in a reduce worker process.
   @param p_fd_in: The address of the buffer holding the intermediate data
   files' file descriptors.  The intermediate data files are output by the map
   worker processes, and they are the input for the reduce worker process.
   @param fd_in_num: The number of the intermediate files.
   @param fd_out: The file descriptor of the final result file.
   @ret: 0 on success, -1 on error.
   @example: if fd_in_num == 3, then there are 3 intermediate files, whose file
   descriptor is identified by p_fd_in[0], p_fd_in[1], and p_fd_in[2]
   respectively.
*/
int word_finder_reduce(int* p_fd_in, int fd_in_num, int fd_out) {
    FILE* wp = fdopen(fd_out, "w");
    if (!wp) return 1;
    char* line_buf = malloc(INIT_LINE_SIZE * sizeof(*line_buf));
    if (!line_buf) {
        _EXIT_ERROR(2, "Malloc of size %lu failed, halting.\n",
                    INIT_LINE_SIZE * sizeof(*line_buf));
    }
    ssize_t read;
    size_t n = INIT_LINE_SIZE;
    for (int i = 0; i < fd_in_num; i++) {
        FILE* fp = fdopen(p_fd_in[i], "r");
        if (!fp) return 1;
        rewind(fp);
        clearerr(fp);
        while ((read = getline(&line_buf, &n, fp)) != -1) {
            fprintf(wp, "%s", line_buf);
        }
        fclose(fp);
    }
    free(line_buf);
    fclose(wp);
    return 0;
}
