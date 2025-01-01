#include<sys/stat.h>
#include <sys/types.h>
#include "init.h"
int init(const char *base_path) {
    char path[256];

    snprintf(path, sizeof(path), "%s/.syncro", base_path);
    if (mkdir(path, 0700) == -1) {
        perror("mkdir");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/.syncro/objects", base_path);
    if (mkdir(path, 0700) == -1) {
        perror("mkdir");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/.syncro/refs", base_path);
    if (mkdir(path, 0700) == -1) {
        perror("mkdir");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/.syncro/refs/heads", base_path);
    if (mkdir(path, 0700) == -1) {
        perror("mkdir");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/.syncro/refs/tags", base_path);
    if (mkdir(path, 0700) == -1) {
        perror("mkdir");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/.syncro/HEAD", base_path);
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }
    fprintf(fp, "ref: refs/heads/master\n");
    fclose(fp);

    return 0;
}
