#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct dirstat_s {
    int regular;
    int dirs;
    int links;
} dirstat;

void count_dir(dirstat*, DIR*, char path[4096]);

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;
    DIR* dirp;
    dirp = opendir(argv[1]);
    if (!dirp) return 1;
    dirstat ds;
    ds.regular = 0;
    ds.dirs = 0;
    ds.links = 0;
    char path[4096] = "./";
    count_dir(&ds, dirp, path);
    printf("Regular files: %d\n", ds.regular);
    printf("Directories: %d\n", ds.dirs);
    printf("Sym-links: %d\n", ds.links);
}

void count_dir(dirstat* ds, DIR* dirp, char path[4096]) {
    struct dirent* entry;
    if (!dirp) return;
    while ((entry = readdir(dirp))) {
        switch (entry->d_type) {
            case DT_REG:
                ds->regular++;
                break;
            case DT_LNK:
                ds->links++;
                break;
            case DT_DIR:
                if (strncmp(entry->d_name, ".", 256) == 0 ||
                    strncmp(entry->d_name, "..", 256) == 0)
                    break;
                ds->dirs++;
                DIR* subdir;
                char newp[4096];
                strncpy(newp, path, 4096);
                strncat(newp, "/", 4096);
                strncat(newp, entry->d_name, 4096);
                subdir = opendir(newp);
                count_dir(ds, subdir, newp);
                break;
        }
    }
}
