#ifndef COMMIT_H
#define COMMIT_H

#define CHUNK 1024 // Size of each chunk to read and compress

int commit(char *message);
static int snapshot();
static int snapem(char* path);
#endif

