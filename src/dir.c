#include "dir.h"
#include <sys/stat.h>

// This function checks if a file is a directory.
// Parameters:
// 	*path: a string that contains the path to the file.
// 		Example: "/home/user/Project"
// Returns: 1 if the file is a directory, 0 otherwise.
int check_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        return 0; // Directory doesn't exist
    }
    return S_ISDIR(st.st_mode);
}


//stat st is a struct that contains information about a file.
// (stat path, &st) is a system call that fills the struct with information about the file at path.
// S_ISDIR(st.st_mode) is a macro that checks if the file is a directory.
// If the file is a directory, the function returns 1, otherwise it returns 0.
