#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "hash.h"
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#define MAX_PATH_LENGTH 1024
#define CHUNK 1024

char* makeSHA(void); // Impoted function to hash the current time and rename the stage file from hash.c

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

    // Calculate file size
    fseek(source, 0, SEEK_END);
    long source_size = ftell(source);
    fseek(source, 0, SEEK_SET);

    if (source_size == 0) {
        fprintf(stderr, "Warning: Source file %s is empty\n", source_path);
        fclose(source);
        return 0; // Not an error, just nothing to compress
    }

    // Read entire file into memory
    unsigned char *source_buffer = malloc(source_size);
    if (!source_buffer) {
        fprintf(stderr, "Error: Failed to allocate memory for source file\n");
        fclose(source);
        return -1;
    }

    if (fread(source_buffer, 1, source_size, source) != source_size) {
        fprintf(stderr, "Error: Failed to read source file\n");
        free(source_buffer);
        fclose(source);
        return -1;
    }
    fclose(source);

    // Calculate maximum compressed size
    uLongf dest_size = compressBound(source_size);
    unsigned char *dest_buffer = malloc(dest_size);
    if (!dest_buffer) {
        fprintf(stderr, "Error: Failed to allocate memory for compressed data\n");
        free(source_buffer);
        return -1;
    }

    // Compress data in a single operation
    int z_result = compress2(dest_buffer, &dest_size, source_buffer, source_size, Z_BEST_COMPRESSION);
    if (z_result != Z_OK) {
        fprintf(stderr, "Error: Compression failed with code %d\n", z_result);
        free(source_buffer);
        free(dest_buffer);
        return -1;
    }

    free(source_buffer); // Don't need source buffer anymore

    // Construct the destination file path
    char final_dest_path[PATH_MAX];
    snprintf(final_dest_path, sizeof(final_dest_path), ".syncro/objects/stage/%s", path);

    // Ensure the parent directory exists
    char dest_dir[PATH_MAX];
    snprintf(dest_dir, sizeof(dest_dir), "%s", final_dest_path);
    char *last_slash = strrchr(dest_dir, '/');
    if (last_slash != NULL) {
        *last_slash = '\0'; // Terminate string at the last slash to get the directory path
        if (mkdir_p(dest_dir) != 0) {
            fprintf(stderr, "Error: Failed to create directory %s\n", dest_dir);
            free(dest_buffer);
            return -1;
        }
    }

    // Open the destination file
    FILE *dest = fopen(final_dest_path, "wb");
    if (!dest) {
        fprintf(stderr, "Error: Failed to open destination file %s: %s\n", final_dest_path, strerror(errno));
        free(dest_buffer);
        return -1;
    }

    // Write original file size first (for decompression)
    uint32_t size_header = source_size;
    if (fwrite(&size_header, sizeof(size_header), 1, dest) != 1) {
        fprintf(stderr, "Error: Failed to write size header\n");
        free(dest_buffer);
        fclose(dest);
        return -1;
    }

    // Write compressed data
    if (fwrite(dest_buffer, 1, dest_size, dest) != dest_size) {
        fprintf(stderr, "Error: Failed to write compressed data\n");
        free(dest_buffer);
        fclose(dest);
        return -1;
    }

    printf("Compressed %s: original size %ld bytes, compressed size %lu bytes\n", 
           path, source_size, dest_size);

    free(dest_buffer);
    fclose(dest);
    return 0;
}

int decompress_file(const char *compressed_path, const char *output_path) {
    FILE *compressed_file = fopen(compressed_path, "rb");
    if (!compressed_file) {
        fprintf(stderr, "Error: Failed to open compressed file %s: %s\n", 
                compressed_path, strerror(errno));
        return -1;
    }

    // Read the original file size
    uint32_t original_size;
    if (fread(&original_size, sizeof(original_size), 1, compressed_file) != 1) {
        fprintf(stderr, "Error: Failed to read original file size from %s\n", compressed_path);
        fclose(compressed_file);
        return -1;
    }

    // Calculate the compressed data size
    fseek(compressed_file, 0, SEEK_END);
    long compressed_file_size = ftell(compressed_file);
    fseek(compressed_file, sizeof(original_size), SEEK_SET);
    
    long compressed_data_size = compressed_file_size - sizeof(original_size);

    // Allocate memory for compressed and decompressed data
    unsigned char *compressed_data = malloc(compressed_data_size);
    unsigned char *decompressed_data = malloc(original_size);
    
    if (!compressed_data || !decompressed_data) {
        fprintf(stderr, "Error: Failed to allocate memory for decompression\n");
        fclose(compressed_file);
        free(compressed_data);
        free(decompressed_data);
        return -1;
    }

    // Read the compressed data
    if (fread(compressed_data, 1, compressed_data_size, compressed_file) != compressed_data_size) {
        fprintf(stderr, "Error: Failed to read compressed data from %s\n", compressed_path);
        fclose(compressed_file);
        free(compressed_data);
        free(decompressed_data);
        return -1;
    }
    
    fclose(compressed_file);

    // Decompress the data
    uLongf dest_len = original_size;
    int z_result = uncompress(decompressed_data, &dest_len, 
                             compressed_data, compressed_data_size);
    
    free(compressed_data); // Don't need compressed data anymore
    
    if (z_result != Z_OK) {
        fprintf(stderr, "Error: Decompression failed with code %d\n", z_result);
        free(decompressed_data);
        return -1;
    }

    // Write the decompressed data to the output file
    FILE *output_file = fopen(output_path, "wb");
    if (!output_file) {
        fprintf(stderr, "Error: Failed to open output file %s: %s\n", 
                output_path, strerror(errno));
        free(decompressed_data);
        return -1;
    }

    if (fwrite(decompressed_data, 1, dest_len, output_file) != dest_len) {
        fprintf(stderr, "Error: Failed to write decompressed data to %s\n", output_path);
        fclose(output_file);
        free(decompressed_data);
        return -1;
    }

    fclose(output_file);
    free(decompressed_data);
    
    printf("Decompressed %s: compressed size %ld bytes, original size %u bytes\n", 
           compressed_path, compressed_data_size, original_size);
           
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
    char* hashValue = makeSHA();
    // append the commit hash to COMMITS file
    FILE *commit_file = fopen(".syncro/COMMITS", "a");
    if (!commit_file) {
	fprintf(stderr, "Error: Failed to open .syncro/COMMITS\n");
	return -1;
    }
    fprintf(commit_file, "%s\n",hashValue );
    free(hashValue);
    printf("Commit successful\n%s\n", message);
    return 0;
}
