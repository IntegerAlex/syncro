# **Function Documentation: `add`**

## **1. Overview**
The `add` function is designed to append file paths to a tracking file (`trackable.txt`) while preventing duplicate entries. This function is invoked when the `add` command is executed in the main function.

## **2. Function Prototype**
```c
char* add(char* path[], int n);
```

## **3. Parameters**
- `path[]`: A character array (array of strings) containing file paths to be added.
- `n`: An integer representing the number of file paths to add.

## **4. Return Value**
- Returns a message (`char*` pointer) indicating success or failure:
  - **"Files added successfully"** if the operation is successful.
  - **"No files to add"** if the input `path` is `NULL`.
  - **`NULL`** if an error occurs while opening the file.
  - **"Error adding files"** if `fprintf` fails while writing to the file.

## **5. Functional Description**
The function follows these steps:

1. **Check for NULL Input:**
   - If `path` is `NULL`, return "No files to add".

2. **Open the File:**
   - The function attempts to open `./.syncro/refs/trackable.txt` in append (`a+`) mode.
   - If the file fails to open, an error message is printed using `perror()`, and `NULL` is returned.

3. **Read Existing Paths:**
   - The function reads existing file paths from `trackable.txt` into a 2D character array (`existing_paths`), which is **statically allocated**.
   - Each line is trimmed to remove the trailing newline character.

4. **Check for Duplicates:**
   - The function loops through the `path` array to verify if the path is already present in `existing_paths`.
   - If a path exists, it is ignored.

5. **Write Unique Paths:**
   - If a path is not found in `existing_paths`, it is written to `trackable.txt` using `fprintf()`.
   - If `fprintf` fails, the file is closed and an error message is returned.

6. **Close the File & Return Message:**
   - The file is closed using `fclose(file);`
   - A success message is returned.

## **6. Memory Allocation**
### **Static Memory Allocation**
- `existing_paths[1024][1024]`: A statically allocated 2D array for storing up to 1024 file paths, each up to 1024 characters long.
- `char *message`: A pointer to a string literal used for returning messages.

### **Dynamic Memory Allocation**
- No dynamic memory allocation (`malloc` or `calloc`) is used in this function.
- The function could be improved to use dynamic memory allocation to handle an arbitrary number of file paths instead of the fixed 1024x1024 array.

## **7. Industry Best Practices Followed**
✔ **Error Handling:** Proper checks for `NULL`, file open failures, and `fprintf` failures.
✔ **File Handling:** Ensures the file is closed after operations to prevent resource leaks.
✔ **Performance Optimization:** Checks for duplicates before writing to reduce unnecessary writes.
✔ **Security Considerations:** No direct buffer overflows, but static allocation limits scalability.

## **8. Potential Improvements**
1. **Dynamic Memory Allocation:** Instead of `existing_paths[1024][1024]`, use `malloc()` to allocate memory dynamically.
2. **Use `fseek()` Instead of Reading Everything Into Memory:** This avoids unnecessary memory consumption when checking for duplicates.
3. **Improve Error Messages:** Return dynamically allocated messages instead of string literals for more descriptive error handling.

## **9. Example Usage**
```c
char *files[] = {"file1.txt", "file2.txt"};
char *result = add(files, 2);
printf("%s\n", result);
```



