#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <unistd.h>
#include <sys/stat.h>
#define MAX_PATH_LENGTH 1024
#define CHUNK 1024
#include <errno.h>


// Function to create a directory and all its parent directories
// parameters:
// 	dir: the directory to creat
// returns:
// 	0 on success, -1 on error
// 		-1 is returned if the directory cannot be created
// 			-1 is also returned if the directory already exists
// 				-1 is also returned if the directory cannot be created due to a permission error
// 					-1 is also returned if the directory cannot be created due to a file system error
//
int mkdir_p(const char *dir) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    // Copy directory string
    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }

    // Iterate through the path
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, 0777) != 0) {
                if (errno != EEXIST) {
                    fprintf(stderr, "Error: Failed to create directory %s: %s\n", tmp, strerror(errno));
                    return -1;
                }
            }
            *p = '/';
        }
    }

    if (mkdir(tmp, 0777) != 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "Error: Failed to create directory %s: %s\n", tmp, strerror(errno));
            return -1;
        }
    }

    return 0;
}
// Function to concatenate path with the current working directory
// Returns a pointer to the full path, or NULL on error 
// The caller is responsible for freeing the returned pointer
// This function is not thread-safe
// The returned pointer is only valid until the next call to this function 
// parameters:
// 	path: the path to concatenate with the current working directory
// 	      if path is NULL, the current working directory is returned
// 	      if path is an absolute path, it is returned as is
// returns:
// 	a pointer to the full path, or NULL on error
char* concat_path_with_cwd(const char* path) {
    char* cwd = getcwd(NULL, 0);  // Get current working directory
    if (!cwd) {
        return NULL;
    }

    size_t full_path_size = strlen(cwd) + strlen(path) + 2;
    char* full_path = malloc(full_path_size);
    if (!full_path) {
        free(cwd);
        return NULL;
    }

    snprintf(full_path, full_path_size, "%s/%s", cwd, path);
    free(cwd);
    return full_path;
}

// Function to create a BLOB object from a file
// parameters:
// 	path: the path to the file to create the BLOB object from
// 	returns:
// 		0 on success, -1 on error
// 			-1 is returned if the file cannot be opened, or if the BLOB object cannot be created
// 				-1 is also returned if the file is empty
// 					-1 is also returned if the BLOB object cannot be written to the .syncro/objects directory
//
static int snapem(char* path) {
    // Validate path
    if (strstr(path, "..") != NULL) {
        fprintf(stderr, "Error: Path contains '..' which is not allowed: %s\n", path);
        return -1;
    }

    // Resolve full source path
    char source_path[PATH_MAX];
    snprintf(source_path, sizeof(source_path), "./%s", path);

    // Open the source file
    FILE *source = fopen(source_path, "rb");
    if (!source) {
        fprintf(stderr, "Error: Failed to open source file %s: %s\n", source_path, strerror(errno));
        return -1;
    }

    // Construct the destination file path
    char final_dest_path[PATH_MAX];
    snprintf(final_dest_path, sizeof(final_dest_path), ".syncro/objects/stage/%s", path);

    // Ensure the parent directory exists:
    char dest_dir[PATH_MAX];
    snprintf(dest_dir, sizeof(dest_dir), "%s", final_dest_path);
    char *last_slash = strrchr(dest_dir, '/');
    if (last_slash != NULL) {
        *last_slash = '\0'; // Terminate string at the last slash to get the directory path
        if (mkdir_p(dest_dir) != 0) {
            fprintf(stderr, "Error: Failed to create directory %s\n", dest_dir);
            fclose(source);
            return -1;
        }
    }

    // Open the destination file
    FILE *dest = fopen(final_dest_path, "wb");
    if (!dest) {
        fprintf(stderr, "Error: Failed to open destination file %s: %s\n", final_dest_path, strerror(errno));
        fclose(source);
        return -1;
    }

    // Initialize zlib stream
    z_stream defstream = {0};
    if (deflateInit(&defstream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        fprintf(stderr, "Error: Failed to initialize zlib compression\n");
        fclose(source);
        fclose(dest);
        return -1;
    }

    unsigned char in[CHUNK], out[CHUNK];
    int flush, bytes_read;

    // Compression loop
    do {
        bytes_read = fread(in, 1, CHUNK, source);
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;

        defstream.avail_in = bytes_read;
        defstream.next_in = in;

        do {
            defstream.avail_out = CHUNK;
            defstream.next_out = out;
            if (deflate(&defstream, flush) == Z_STREAM_ERROR) {
                fprintf(stderr, "Error: Compression failed\n");
                deflateEnd(&defstream);
                fclose(source);
                fclose(dest);
                return -1;
            }
            size_t write_size = CHUNK - defstream.avail_out;
            if (fwrite(out, 1, write_size, dest) != write_size) {
                fprintf(stderr, "Error: Failed to write compressed data to %s: %s\n", final_dest_path, strerror(errno));
                deflateEnd(&defstream);
                fclose(source);
                fclose(dest);
                return -1;
            }
        } while (defstream.avail_out == 0);
    } while (flush != Z_FINISH);

    // Cleanup
    deflateEnd(&defstream);
    fclose(source);
    fclose(dest);

    return 0;
}
// Function to commit the staged files
// parameters:
// 	message: the commit message
// returns:
// 	0 on success, -1 on error
// 		-1 is returned if the tracker file cannot be opened
// 			-1 is also returned if any of the staged files cannot be snapem'd
// 				-1 is also returned if the commit message cannot be written to the .syncro/objects directory
// 					-1 is also returned if the commit message cannot be written to the .syncro/refs/commits directory
// 						-1 is also returned if the commit message cannot be written to the .syncro/refs/trackable.txt file
// 							-1 is also returned if the commit message cannot be written to the .syncro/refs/HEAD file
int commit(char* message) {
    FILE *tracker = fopen(".syncro/refs/trackable.txt", "r");
    if (!tracker) {
        fprintf(stderr, "Error: Failed to open tracker file .syncro/refs/trackable.txt\n");
        return -1;
    }
	
    if (mkdir(".syncro/objects/stage", 0777) != 0) {
	fprintf(stderr, "Error: Failed to create .syncro/objects/stage directory\n");
	return -1;
    }
    char path[MAX_PATH_LENGTH];
    while (fgets(path, sizeof(path), tracker)) {
        // Trim newline
        size_t len = strlen(path);
        if (len > 0 && path[len - 1] == '\n') {
            path[len - 1] = '\0';
        }

        if (snapem(path) != 0) {
            fprintf(stderr, "Error: Failed to snapem %s\n", path);
            fclose(tracker);
            return -1;
        }
    }

    fclose(tracker);
    printf("Commit successful\n%s\n", message);
    return 0;
}

