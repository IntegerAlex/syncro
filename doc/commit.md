# Detailed Byte-Level Explanation of the Commit Process in Syncro

## Overview
The `commit` function in `syncro` is responsible for tracking and compressing files listed in `.syncro/refs/trackable.txt`. The process involves reading the file paths from the tracker file, opening them, compressing their contents using zlib, and storing them in `.syncro/objects/`.

## Step-by-Step Execution

### 1. Opening the Tracker File
The function starts by opening `.syncro/refs/trackable.txt` in read mode:
```c
char* tracker_path = ".syncro/refs/trackable.txt";
FILE *tracker = fopen(tracker_path, "r");
```
This file contains a list of file paths that need to be committed.

- **File Descriptor Allocation:** The OS assigns a file descriptor (integer) to the tracker file, which is used for subsequent read operations.
- **Error Handling:** If the file cannot be opened, an error is printed, and the function exits.

### 2. Iterating Through Tracked Files
The function reads file paths from `trackable.txt` line by line:
```c
while (!feof(tracker)) {
    char path[MAX_PATH_LENGTH];
    if (fgets(path, sizeof(path), tracker)) {
```
- **fgets Behavior:** Reads up to `MAX_PATH_LENGTH` bytes or until a newline is encountered.
- **Newline Trimming:** The last character is checked and removed if it is `\n`.

### 3. Calling `snapem(path)`
For each file path read, the `snapem` function is invoked to handle compression and storage.

## Breakdown of `snapem`

### 1. Constructing the Source File Path
```c
char source_path[MAX_PATH_LENGTH];
snprintf(source_path, sizeof(source_path), "./%s", path);
```
- **`snprintf` Ensures Buffer Safety:** Prevents buffer overflows by limiting the write size.
- **File Path Resolution:** The source file path is constructed relative to the current working directory.

### 2. Opening the Source File
```c
FILE *source = fopen(source_path, "rb");
```
- **`rb` Mode:** Reads the file in binary mode, preserving exact byte data.
- **Error Handling:** If `fopen` fails, an error is logged.

### 3. Ensuring Destination Directory Exists
```c
mkdir(".syncro/objects", 0777);
```
- **Syscall Effect:** Creates `.syncro/objects/` if it does not exist.
- **Mode `0777`:** Grants read, write, and execute permissions to all users.

### 4. Creating the Destination File Path
```c
char final_dest_path[MAX_PATH_LENGTH];
snprintf(final_dest_path, sizeof(final_dest_path), ".syncro/objects/%s", path);
```
- **Path Construction:** Creates a path for the compressed file within `.syncro/objects/`.

### 5. Opening the Destination File
```c
FILE *dest = fopen(final_dest_path, "wb");
```
- **`wb` Mode:** Opens the file for binary writing, overwriting existing content.
- **Error Handling:** If `fopen` fails, the source file is closed, and the function returns.

### 6. Initializing zlib Compression
```c
z_stream defstream = {0};
if (deflateInit(&defstream, Z_DEFAULT_COMPRESSION) != Z_OK) {
```
- **zlib Structure:** `z_stream` is initialized with zeros.
- **Compression Level:** `Z_DEFAULT_COMPRESSION` balances speed and compression ratio.

### 7. File Compression
The function reads `CHUNK` bytes at a time, compresses them, and writes the compressed data to the destination file.

#### Read Loop:
```c
do {
    bytes_read = fread(in, 1, CHUNK, source);
    flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
```
- **Buffered Read:** Reads up to `CHUNK` bytes.
- **End of File Handling:** If EOF is reached, `flush` is set to `Z_FINISH`.

#### Compression Loop:
```c
do {
    defstream.avail_out = CHUNK;
    defstream.next_out = out;
    if (deflate(&defstream, flush) == Z_STREAM_ERROR) {
```
- **Compression Process:** Calls `deflate` to compress `in` buffer into `out` buffer.
- **Error Handling:** If compression fails, `deflateEnd` is called to free resources.

#### Writing Compressed Data:
```c
fwrite(out, 1, CHUNK - defstream.avail_out, dest);
```
- **Data Transfer:** The compressed bytes are written to the destination file.

### 8. Cleanup and Finalization
```c
deflateEnd(&defstream);
fclose(source);
fclose(dest);
```
- **Resource Management:** Ensures all files are closed, and compression structures are cleaned up.

## Finalizing the Commit
```c
printf("Commit successful\n %s", message);
```
- **Success Message:** Prints confirmation and commit message.

## Byte-Level Impact
- **Uncompressed Data:** Stored in `source_path`, read in `CHUNK`-sized blocks.
- **Compressed Data:** Written in `CHUNK - defstream.avail_out` bytes.
- **File System Impact:** Creates a new compressed file in `.syncro/objects/` for each tracked file.

## Summary
The `commit` process reads tracked files, compresses their contents using zlib, and writes them to a structured `.syncro/objects/` directory while ensuring error handling and efficient memory usage.


