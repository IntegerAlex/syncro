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
static int snapshot();

int commit(char *message) {
	if(!snapshot()) {
		return -1;
	}

    printf("Committing changes with message: %s\n", message);
    return 0;
}


static int snapshot() {
	
	// Read the file line by line
	FILE *file = fopen("../.syncro/refs/trackables.txt", "r");
	char line[256];
	while (fgets(line, sizeof(line), file)) {
		snapem(line);
	}
	fclose(file);
    return 1;
}


static int snapem(char* path) {
    // Open the source file
    FILE *source = fopen(concat_path_with_cwd(path), "r");
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

char* commit_hash() {




}



// Function to compute the SHA-256 hash of a file's contents
void update_sha256_with_file(FILE* file, SHA256_CTX* sha256_ctx) {
    unsigned char buffer[1024];  // Buffer for reading the file
    size_t bytes_read;

    // Read the file in chunks and update the SHA-256 context
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) != 0) {
        SHA256_Update(sha256_ctx, buffer, bytes_read);
    }
}

// Function to compute a single SHA-256 hash for all files in a directory
int compute_single_sha256_for_directory(const char* dir_path) {
    // Initialize SHA-256 context
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);

    // Open the directory
    DIR* dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        return -1;
    }

    struct dirent* entry;
    char full_path[MAX_PATH_LENGTH];

    // Iterate over all the entries in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." (current and parent directory entries)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Build the full path of the file
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        // Open the file for reading
        FILE* file = fopen(full_path, "r");
        if (file == NULL) {
            perror("Error opening file");
            continue;  // Skip this file and continue with the next
        }

        // Update SHA-256 hash for the current file's contents
        update_sha256_with_file(file, &sha256_ctx);

        // Close the file after reading
        fclose(file);
    }

    // Close the directory
    closedir(dir);

    // Allocate memory for the final hash result
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256_ctx);

    // Print the SHA-256 hash in hexadecimal format
    printf("SHA-256 hash of directory contents: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}

