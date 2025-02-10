#include <stdio.h>
#include <string.h>

// Function to remove specified file paths from trackable.txt
// Parameters:
//  path[]: (Character array) Paths of files to be removed
//  n: (Integer) Number of files to be removed
// Returns: (pointer to character) Success or error message
char* remove_files(char* path[], int n) {
    FILE *file = fopen("./.syncro/refs/trackable.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return "Error opening file";
    }

    // Temporary storage for file lines
    char existing_paths[1024][1024]; // Max 1024 lines, 1024 chars per line
    int line_count = 0;

    // Read file contents into array
    while (fgets(existing_paths[line_count], sizeof(existing_paths[line_count]), file)) {
        existing_paths[line_count][strcspn(existing_paths[line_count], "\n")] = '\0'; // Remove newline
        line_count++;
    }
    fclose(file);

    // Open file in write mode to update contents
    file = fopen("./.syncro/refs/trackable.txt", "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return "Error writing file";
    }

    // Write back only the lines that are NOT in the paths to remove
    for (int i = 0; i < line_count; i++) {
        int to_remove = 0;
        for (int j = 0; j < n; j++) {
            if (strcmp(existing_paths[i], path[j]) == 0) {
                to_remove = 1;
                break;
            }
        }
        if (!to_remove) {
            fprintf(file, "%s\n", existing_paths[i]);
        }
    }

    fclose(file);
    return "Files removed successfully";
}

