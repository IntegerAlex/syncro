#include "dir.h"
int check_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        return 0; // Directory doesn't exist
    }
    return S_ISDIR(st.st_mode); // Check if it's a directory
}
