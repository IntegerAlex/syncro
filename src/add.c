#include <string.h>
#include <stdio.h> 

// Function to add files to trackable.txt
// path[]: (Character) array of paths of files to be added
// n: (Integer) number of files to be added
// Returns: (pointer to character) message indicating success or failure
//
// Note: This function is called by the add command in the main function
char* add(char* path[], int n){	
	if(path == NULL){
		return "No files to add";
	}
	FILE *file = fopen("./.syncro/refs/trackable.txt", "w+");
	if (file == NULL) {
    		perror("Error opening file");
    	return NULL;
	}

	for (int i = 0; i < n; i++) {
    		int value = fprintf(file, "%s\n", path[i]);
		if(value < 0){
			return "Error adding files";
		}
	}

	fclose(file);

	char *message = "Files added successfully";
	return message;

}



