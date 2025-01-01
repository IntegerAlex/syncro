#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "dir.h"
#include "init.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s init [base_path]\n", argv[0]);
        return 1;
    }

    const char *base_path = argc > 2 ? argv[2] : ".";

    if (strcmp(argv[1], "init") == 0) {
        char syncro_path[256];
        snprintf(syncro_path, sizeof(syncro_path), "%s/.syncro", base_path);

        if (check_dir(syncro_path)) {
            printf("Syncro already initialized\n");
        } else {
            if (init(base_path) == 0) {
                printf("Syncro initialized at %s/.syncro\n", base_path);
            }
        }
    } else {
        printf("Unknown command: %s\n", argv[1]);
        printf("Usage: %s init [base_path]\n", argv[0]);
    }

    return 0;
}

