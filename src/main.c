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
int diff(char* path, char* commit_id,int verbose);

int main(int argc, char *argv[]) {
    if (argc < 2) {
	    printf("syncro - Just works better\n");
	    printf("version 0.1\n");
	    printf("copywrite 2024-2025 Akshat Kotpalliwar (alias IntegerAlex on github\n");
	    printf("Usage: %s <command> [<args>]\n", argv[0]);
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
		case 5:
			diff(argv[2], NULL , 0);
			break;
		case 6:
			diff(argv[2], NULL ,1);
			break;
		default:
			printf("Invalid command\n");
			printf("Usage: <command> [<args>]\n");
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
	else if (strcmp(command, "diff") == 0){
		return 5;
	}
	else if (strcmp(command, "vdiff") == 0){
		return 6;
	}
	else{
		printf("Invalid command\n");
		printf("Usage: <command> [<args>]\n");
		return 0;
	}
}

