---

## Overview
This document provides a comprehensive explanation of the `syncro` file snapshot and commit system, highlighting dynamic and static memory allocation techniques used in the code.

## Functionality Breakdown

### 1. `mkdir_p` (Create Directory Recursively)
#### **Purpose:**
Creates directories recursively, ensuring that parent directories exist before creating child directories.

#### **Implementation:**
- Uses `snprintf` to copy the directory path.
- Iterates through each directory level and calls `mkdir`.
- Handles errors like existing directories (`EEXIST`) or permission issues.
- Returns `0` on success and `-1` on failure.

#### **Memory Allocation:**
- Uses a local array `tmp[PATH_MAX]`, which is **statically allocated**.
- No dynamic memory allocation is required.

### 2. `concat_path_with_cwd` (Concatenate Path with CWD)
#### **Purpose:**
Concatenates the provided path with the current working directory to generate an absolute path.

#### **Implementation:**
- Uses `getcwd(NULL, 0)`, which dynamically allocates memory.
- Computes the required memory size for the concatenated path.
- Allocates memory using `malloc()`.
- Uses `snprintf()` to construct the full path.
- Frees memory used for `getcwd` after concatenation.

#### **Memory Allocation:**
- **Dynamic Allocation:** `getcwd(NULL, 0)` and `malloc()`.
- **Deallocation:** `free(cwd)` to prevent memory leaks.

### 3. `snapem` (Create a Compressed BLOB of a File)
#### **Purpose:**
Compresses a file using `zlib` and stores it in `.syncro/objects/stage/`.

#### **Implementation:**
- Validates the file path (rejects `..` for security).
- Opens the source file (`fopen()` in binary mode).
- Constructs the destination file path and ensures the directory exists (`mkdir_p`).
- Uses `zlib`'s `deflateInit()` to initialize compression.
- Reads input file in chunks (`fread()`), compresses using `deflate()`, and writes output (`fwrite()`).
- Handles error cases and ensures cleanup with `deflateEnd()`.

#### **Memory Allocation:**
- **Static Allocation:**
  - `unsigned char in[CHUNK], out[CHUNK]` (buffers for file compression).
- **Dynamic Allocation:**
  - `z_stream defstream = {0};` (zlib structure for compression, but does not require manual `malloc/free`).

### 4. `commit` (Commit Staged Files)
#### **Purpose:**
Reads a list of staged files from `.syncro/refs/trackable.txt`, compresses them using `snapem`, and commits changes.

#### **Implementation:**
- Opens the tracking file to get a list of staged files.
- Iterates through each file and calls `snapem()`.
- Calls `makeSHA()` to generate a commit hash.
- Prints a success message.

#### **Memory Allocation:**
- **Static Allocation:**
  - `char path[MAX_PATH_LENGTH];` for reading file paths.
  - `char final_dest_path[PATH_MAX];` for constructing the destination path.
- **Dynamic Allocation:**
  - None, since all operations use pre-allocated buffers.

## Industry Standards and Best Practices
1. **Error Handling:**
   - Uses `strerror(errno)` for meaningful error messages.
   - Ensures file and directory operations handle permission issues gracefully.

2. **Security Considerations:**
   - Prevents directory traversal attacks by rejecting `..` in `snapem()`.
   - Uses `0777` permissions cautiously, but might need to be restricted further.

3. **Memory Management:**
   - Properly frees allocated memory in `concat_path_with_cwd`.
   - Uses stack-based allocation where possible to avoid heap fragmentation.

4. **Code Readability and Maintainability:**
   - Uses clear function names and comments for understanding.
   - Follows modular design by separating concerns (`snapem` for compression, `commit` for processing, etc.).

## Additional Notes
- The system does not use `calloc()` or `realloc()` as memory needs are predictable.
- `zlib` internally handles its own dynamic allocations, reducing complexity.
- Thread safety is not explicitly addressed, so concurrent writes might need further synchronization mechanisms if multi-threading is introduced in the future.
