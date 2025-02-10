#include<sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include "init.h"
#ifdef _WIN32
    #include <direct.h>  // Windows-specific header for mkdir
    #define MKDIR(path) _mkdir(path)  // Windows `_mkdir()` takes only one argument
#else
    #include <unistd.h>  // Required for POSIX mkdir
    #define MKDIR(path) mkdir(path, 0700)  // Linux `mkdir()` takes two arguments
#endif
// This function initializes a Syncro repository in the given directory. 
// Parameters:
// 	*base_path: a string that contains the path to the directory where the repository will be initialized.
// Example: "/home/user/Project"
// Returns: 0 if the repository is successfully initialized, 1 otherwise.
int init(const char *base_path) {
    char path[256];

    snprintf(path, sizeof(path), "%s/.syncro", base_path);
    if (MKDIR(path) == -1) {
        perror("mkdir");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/.syncro/objects", base_path);
    if (MKDIR(path) == -1) {
        perror("mkdir");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/.syncro/refs", base_path);
    if (MKDIR(path) == -1) {
        perror("mkdir");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/.syncro/refs/heads", base_path);
    if (MKDIR(path) == -1) {
        perror("mkdir");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/.syncro/refs/tags", base_path);
    if (MKDIR(path) == -1) {
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

    // Print license and Author information with the init message
    printf("\nSyncro - Just works better\n");
    printf("Version 0.1\n");
    printf("Copyright 2024-2025 Akshat Kotpalliwar (alias IntegerAlex on github)\n");
    printf("License: GOFL-V1\n");
    printf("Warranty and Liability: None\n");
    printf("\nInitialized empty Syncro repository in %s/.syncro/\n", base_path);
    
    return 0;
}
