#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define MAX_PATH_LENGTH 1024
#define CHUNK 1024

// Function to concatenate path with the current working directory
char* concat_path_with_cwd(const char* path) {
    char* cwd = getcwd(NULL, 0);  // Get current working directory
    if (!cwd) {
        return NULL;
    }

    size_t cwd_len = strlen(cwd);
    size_t path_len = strlen(path);
    
    // Allocate memory for the concatenated path
    char* full_path = malloc(cwd_len + path_len + 2);
    if (!full_path) {
        free(cwd);
        return NULL;
    }

    // Construct the full path
    snprintf(full_path, cwd_len + path_len + 2, "%s/%s", cwd, path);
    free(cwd);
    return full_path;
}

static int snapem(const char* path) {
    // Resolve the full path of the source file
    char* source_path = concat_path_with_cwd(path);
    if (!source_path) {
        fprintf(stderr, "Error: Failed to resolve source path for %s\n", path);
        return -1;
    }

    // Open the source file in read mode (binary)
    FILE *source = fopen(source_path, "rb");
    if (!source) {
        fprintf(stderr, "Error: Failed to open source file %s\n", source_path);
        free(source_path);
        return -1;
    }
    free(source_path);  // Free the source path memory after file is opened

    // Define the destination directory and resolve the path
    char* dest_path = "/home/akshatrhel/projects/syncro/.syncro/objects/"; // Destination directory 
    if (!dest_path) {
        fclose(source);
        return -1;
    }
    
    // Create the final destination path by combining the destination directory and original file path
    char final_dest_path[MAX_PATH_LENGTH];
    snprintf(final_dest_path, sizeof(final_dest_path), "%s/%s", dest_path, path);

    // Open the destination file in write mode (binary)
    FILE *dest = fopen(final_dest_path, "wb");
    if (!dest) {
        fprintf(stderr, "Error: Failed to open destination file %s\n", final_dest_path);
        fclose(source);
        return -1;
    }

    // Initialize the zlib stream for compression
    z_stream defstream = {0};
    if (deflateInit(&defstream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        fprintf(stderr, "Error: Failed to initialize zlib compression\n");
        fclose(source);
        fclose(dest);
        return -1;
    }

    // Buffers for reading and writing compressed data
    unsigned char in[CHUNK], out[CHUNK];
    int flush, bytes_read;

    // Compression loop
    do {
        bytes_read = fread(in, 1, CHUNK, source);
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;  // Set flush flag at EOF
        
        defstream.avail_in = bytes_read;
        defstream.next_in = in;
        
        // Compress the data and write to the destination
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
            fwrite(out, 1, CHUNK - defstream.avail_out, dest);
        } while (defstream.avail_out == 0);
    } while (flush != Z_FINISH);  // Continue until compression is finished

    // Clean up and close files
    deflateEnd(&defstream);
    fclose(source);
    fclose(dest);
    
    return 0;  // Success
}
int main(void){
snapem("LICENSE.md");  // Call the snapem function with the source file path
}
