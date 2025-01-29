#include <string.h>
#include <stdio.h> 

// Function to add files to trackable.txt
// path[]: (Character) array of paths of files to be added
// n: (Integer) number of files to be added
// Returns: (pointer to character) message indicating success or failure
//
// Note: This function is called by the add command in the main function
char* add(char* path[], int n) {    
    if (path == NULL) {
        return "No files to add";
    }

    // Open the file in read mode to check for existing content
    FILE *file = fopen("./.syncro/refs/trackable.txt", "a+");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // To store the file content for checking duplicates
    char existing_paths[1024][1024];  // Max 1024 lines, max 1024 chars per line
    int line_count = 0;
    
    // Read the existing content into the array
    while (fgets(existing_paths[line_count], sizeof(existing_paths[line_count]), file)) {
        existing_paths[line_count][strcspn(existing_paths[line_count], "\n")] = '\0';  // Remove newline
        line_count++;
    }

    // Append new paths only if they don't already exist in the file
    for (int i = 0; i < n; i++) {
        int already_exists = 0;

        // Check if the path already exists in the file
        for (int j = 0; j < line_count; j++) {
            if (strcmp(existing_paths[j], path[i]) == 0) {
                already_exists = 1;
                break;
            }
        }

        // If the path doesn't already exist, append it
        if (!already_exists) {
            int value = fprintf(file, "%s\n", path[i]);
            if (value < 0) {
                fclose(file);
                return "Error adding files";
            }
        }
    }

    fclose(file);

    char *message = "Files added successfully";
    return message;
}

