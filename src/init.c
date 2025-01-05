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
    int fpvalue = fprintf(fp, "ref: refs/heads/master\n");
    if (fpvalue < 0) {
	perror("fprintf");
	return 1;
    }
    int fvalue = fclose(fp);
    if (fvalue == EOF) {
	perror("fclose");
	return 1;
    }

    return 0;
}
