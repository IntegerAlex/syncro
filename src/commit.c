#include <stdio.h>	
#include <zlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <openssl/sha.h>
#include "commit.h"

#define CHUNK 1024 // Size of each chunk to read and compress

#define MAX_PATH_LENGTH 1024


static char* concat_path_with_cwd(const char*);
static int snapem(char*);
static int snapshot(void);
char* commit_hash(void);

int commit(char *message) {
	int error = snapshot();
	if (error == -1) {
		return -1;
	}
	const char* hash = commit_hash();
	printf("Commit hash: %s\n", hash);
	
    printf("Committing changes with message: %s\n", message);
    return 0;
}


static int snapshot(void) {
	
	// Read the file line by line
	
	FILE *file = fopen(concat_path_with_cwd(".syncro/refs/trackable.txt"), "r");
	printf("%s\n", concat_path_with_cwd(".syncro/refs/trackable.txt"));
	if (!file) {
		perror("Error opening trackables file");
		return -1;
	}
	char line[256];
	while (fgets(line, sizeof(line), file)) {
		snapem(line);
		if (line[strlen(line) - 1] == '\n') {
			line[strlen(line) - 1] = '\0';  // Remove newline character
		}
		printf("Snapshotting: %s\n", line);
	}
	
	fclose(file);
    return 1;
}


static int snapem(char* path) {
    // Open the source file
    FILE *source = fopen(concat_path_with_cwd(path), "r");
    printf("%s\n", concat_path_with_cwd(path));
    if (!source) {
        perror("Error opening source file");
        return -1;  // If fopen fails, return an error
    }
 // Create the destination path by concatenating CWD + .syncro/objects/ + path
    char* dest_path = concat_path_with_cwd(".syncro/objects/stage");
    if (!dest_path) {
        perror("Error concatenating CWD for destination path");
        fclose(source);
        return -1;
    }

    // Append the relative path to the destination path
    char final_dest_path[1024];
    snprintf(final_dest_path, sizeof(final_dest_path), "%s%s", dest_path, path);

    // Open the destination file to write the compressed data
    FILE *dest = fopen(final_dest_path, "a");
    if (!dest) {
        perror("Error opening destination file");
        fclose(source);  // Don't forget to close the source file if fopen fails
        return -1;
    }

    // Initialize zlib deflate stream
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;

    if (deflateInit(&defstream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        perror("Error initializing deflate stream");
        fclose(source);
        fclose(dest);
        return -1;
    }

    unsigned char out[CHUNK];
    unsigned char in[CHUNK];
    int flush;
    int bytes_read, bytes_written;

    // Read and compress in chunks
    do {
        bytes_read = fread(in, 1, CHUNK, source);
        if (bytes_read == 0) {
            flush = Z_FINISH;  // No more data to read, so flush
        } else {
            flush = Z_NO_FLUSH;
        }

        if (bytes_read < 0) {
            perror("Error reading from source file");
            deflateEnd(&defstream);
            fclose(source);
            fclose(dest);
            return -1;  // Handle fread failure
        }

        defstream.avail_in = bytes_read;
        defstream.next_in = in;

        do {
            defstream.avail_out = CHUNK;
            defstream.next_out = out;

            int ret = deflate(&defstream, flush);
            if (ret == Z_STREAM_ERROR) {
                perror("Error during compression");
                deflateEnd(&defstream);
                fclose(source);
                fclose(dest);
                return -1;
            }

            bytes_written = CHUNK - defstream.avail_out;
            if (fwrite(out, 1, bytes_written, dest) != bytes_written || ferror(dest)) {
                perror("Error writing to destination file");
                deflateEnd(&defstream);
                fclose(source);
                fclose(dest);
                return -1;
            }
        } while (defstream.avail_out == 0);

    } while (flush != Z_FINISH);

    // Clean up
    deflateEnd(&defstream);
    fclose(source);
    fclose(dest);
		
    return 0;
}
static char* concat_path_with_cwd(const char* relative_path) {
    // Get the current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return NULL;  // Error getting current working directory
    }

    // Allocate enough memory for the combined path
    size_t path_len = strlen(cwd) + strlen(relative_path) + 2; // +2 for '/' and null-terminator
    char* absolute_path = (char*)malloc(path_len);
    if (!absolute_path) {
        perror("Memory allocation error");
        return NULL;  // Memory allocation failed
    }

    // Concatenate the current directory with the relative path
    snprintf(absolute_path, path_len, "%s/%s", cwd, relative_path);

    // Return the resulting absolute path
    return absolute_path;
}

char* commit_hash(void) {


	return "this is a hash";

}



