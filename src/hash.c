#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>

#define TIME_STR_SIZE 32 // Size of the time string
#define STAGE ".syncro/objects/stage"  // Path to the stage folder 


// Function to hash the current time
// parameters:
// 	   void
// returns:
// 	   char* - the hash value of the current time
// 	   		   or NULL if an error occurred
// reference:
//    https://wiki.openssl.org/index.php/EVP_Message_Digests
static char *hash(void) {
    time_t now = time(NULL);
    char time_str[TIME_STR_SIZE];
    snprintf(time_str, sizeof(time_str), "%ld", now);

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        fprintf(stderr, "Failed to create EVP_MD_CTX\n");
        return NULL;
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1 ||
        EVP_DigestUpdate(mdctx, time_str, strlen(time_str)) != 1 ||
        EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
        fprintf(stderr, "Failed to compute hash\n");
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }
    EVP_MD_CTX_free(mdctx);

    char *hash_str = calloc(hash_len * 2 + 1, sizeof(char));
    if (!hash_str) {
        fprintf(stderr, "Failed to allocate memory for hash string\n");
        return NULL;
    }

    for (unsigned int i = 0; i < hash_len; i++) {
        if (snprintf(&hash_str[i * 2], 3, "%02x", hash[i]) != 2) {
            fprintf(stderr, "Failed to format hash string\n");
            free(hash_str);
            return NULL;
        }
    }

    return hash_str;
}

// Export function to hash the current time and rename the stage file
// parameters:
// 	   void
// returns:
// 	   int - 0 if successful, -1 if an error occurred
// 	   		 and an error message is printed to stderr
// 	   		 if an error occurred
int makeSHA(void) {
    // Get the hash value
    char* hashValue = hash();  
    if (!hashValue) {
        return -1;  // Return error if hash() failed
    }

    // Calculate the new size for the concatenated string
    size_t new_size = strlen(".syncro/objects/") + strlen(hashValue) + 1;  // +1 for the null terminator

    // Allocate memory for the new path
    char *new_path = malloc(new_size);
    if (!new_path) {
        free(hashValue);  // Clean up the allocated hashValue
        return -1;  // Memory allocation failed
    }

    // Concatenate the paths safely using snprintf
    snprintf(new_path, new_size, ".syncro/objects/%s", hashValue);

    // Perform the rename operation
    if (rename(STAGE, new_path) != 0) {
        fprintf(stderr, "Error: Failed to rename %s to %s\n", STAGE, new_path);
        free(hashValue);  // Clean up the allocated hashValue
        free(new_path);   // Clean up the allocated new_path
        return -1;  // Rename failed
    }

    // Clean up the allocated memory
    free(hashValue);
    free(new_path);

    return 0;  // Success
}
