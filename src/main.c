#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "dir.h"
#include "commit.h"
#include "init.h"
#include "add.h"
#include "rm.h"
int init(const char *base_path);
int command(char *command);
char* add(char *paths[], int count);
char* remove_files(char *paths[], int count);
int commit( char *message);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s init [base_path]\n", argv[0]);
        return 1;
    }
	int value = command(argv[1]);
    	const char *base_path = argc > 2 ? argv[2] : ".";
	switch(value){
		case 1:
			init(base_path);
			break;

		case 2:
			for(int i= 2; i < argc; i++){
				argv[i-2] = argv[i];
			
			}
			char* message = add(argv, argc-2);
			printf("%s\n", message);
			break;

		case 3:
			commit(argv[2]);
			break;
		case 4:
			for(int i= 2; i < argc; i++){
				argv[i-2] = argv[i];
			
			}
			char* message1 = remove_files(argv, argc-2);
			printf("%s\n", message1);
			break;
	}

    return 0;
}

int command(char *command){
	if(strcmp(command, "init") == 0){
		return 1;
	}
	else if(strcmp(command, "add") == 0){
		return 2;
	}
	else if (strcmp(command, "commit") == 0){
		return 3;
	}
	else if (strcmp(command,"rm") == 0){
		return 4;
	}
	else{
		return 0;
	}
}

