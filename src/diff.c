#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "inflate.h"

int zlib_inflate(FILE* source, FILE* dest);
#define FILE_BUFF ".syncro/refs/buff.out"

int diff(char* path, char* commit_id) {
    char HEAD[256];  

    // Get the latest commit ID if not provided
    if (commit_id == NULL) {
        FILE *commit_file = fopen(".syncro/COMMITS", "r");
        if (!commit_file) {
            perror("Error opening .syncro/COMMITS");
            return -1;
        }
        if (!fgets(HEAD, sizeof(HEAD), commit_file)) {
            perror("Error reading commit ID");
            fclose(commit_file);
            return -1;
        }
        fclose(commit_file);
        HEAD[strcspn(HEAD, "\n")] = 0; // Remove newline if present
	strcat(HEAD, "/");
        commit_id = HEAD;
    }

    // Construct the deflated file path
    char deflated_file[512];  
    snprintf(deflated_file, sizeof(deflated_file), ".syncro/objects/%s%s", commit_id ,path);
	printf("deflated_file: %s\n", deflated_file);
    // Open the deflated file for reading
    FILE *source_file = fopen(deflated_file, "rb");
    if (!source_file) {
        perror("Error opening deflated file");
        return -1;
    }

    // Open the buffer file for writing
    FILE *dest_file = fopen(FILE_BUFF, "wb");
    if (!dest_file) {
        perror("Error opening destination file");
        fclose(source_file);
        return -1;
    }

    // Inflate the file
    int result = zlib_inflate(source_file, dest_file);

    // Close files
    fclose(source_file);
    fclose(dest_file);

    return result;
}

