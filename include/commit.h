#ifndef COMMIT_H
#define COMMIT_H

#define CHUNK 1024 // Size of each chunk to read and compress

int commit(char *message);   // Commit function declaration
static int snapshot(void);          // Snapshot function declaration
static int snapem(const char* path);      // Snapem function declaration

#endif

