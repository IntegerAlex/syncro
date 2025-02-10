**Document: Understanding the Code Implementation**

### Overview
This document explains the implementation of the given C program, focusing on its functionality, dynamic and static memory allocation, and adherence to industry standards.

---

### Functionality
The program performs the following tasks:
1. Generates a SHA-256 hash of the current timestamp.
2. Uses this hash to rename a predefined file (`.syncro/objects/stage`) to a new name inside `.syncro/objects/`.

The program consists of two primary functions:
- `hash()`: Computes a SHA-256 hash of the current time.
- `makeSHA()`: Calls `hash()`, constructs a new file path, and renames the stage file accordingly.

---

### Code Breakdown
#### 1. **Hashing the Current Time** (`hash` function)
- Uses `time()` to get the current timestamp.
- Converts it into a string.
- Uses OpenSSL’s EVP API to compute a SHA-256 hash of this timestamp.
- Formats the hash as a hexadecimal string for use as a filename.

#### 2. **Renaming the Stage File** (`makeSHA` function)
- Calls `hash()` to obtain a unique hash-based name.
- Constructs the new file path dynamically.
- Uses `rename()` to rename the file.
- Frees allocated memory before exiting.

---

### Memory Allocation
#### **Static Memory Allocation**
Static memory is allocated for:
- `time_str[TIME_STR_SIZE]` (fixed-size array for the timestamp string)
- `hash[EVP_MAX_MD_SIZE]` (array for the SHA-256 hash output)

#### **Dynamic Memory Allocation**
Dynamic memory is allocated for:
- `EVP_MD_CTX_new()`: Allocates a context for hashing.
- `hash_str = calloc(hash_len * 2 + 1, sizeof(char))`: Stores the final hash as a string.
- `new_path = malloc(new_size)`: Stores the full new filename.
- All dynamically allocated memory is freed before function exit.

---

### Industry Standards and Best Practices
#### **1. Memory Management**
- Uses `calloc` and `malloc` to allocate memory dynamically.
- Ensures all allocated memory is freed before function exit.

#### **2. Error Handling**
- Checks for null pointers after memory allocation.
- Prints meaningful error messages in case of failure.
- Returns `-1` on failure and `0` on success.

#### **3. Secure String Handling**
- Uses `snprintf()` to prevent buffer overflows.
- Validates output length during `snprintf()` calls.

#### **4. OpenSSL Best Practices**
- Uses `EVP_MD_CTX_new()` and `EVP_MD_CTX_free()` instead of deprecated functions.
- Ensures proper error checking for `EVP` operations.

---


