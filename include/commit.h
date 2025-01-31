#ifndef COMMIT_H
#define COMMIT_H

#define CHUNK 1024 // Size of each chunk to read and compress

int commit(char *message);   // Commit function declaration
int snapem(char* path);      // Snapem function declaration

#endif

