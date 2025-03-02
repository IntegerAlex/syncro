#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include "inflate.h"
#define CHUNK 16384

int zlib_inflate(FILE *source, FILE *dest) {
    // Read the original file size header first
    uint32_t original_size;
    if (fread(&original_size, sizeof(original_size), 1, source) != 1) {
        fprintf(stderr, "Error: Failed to read original file size header\n");
        return -1;
    }
    
    // Calculate the compressed data size
    long current_pos = ftell(source);
    fseek(source, 0, SEEK_END);
    long file_size = ftell(source);
    fseek(source, current_pos, SEEK_SET);
    
    long compressed_size = file_size - current_pos;
    
    // Allocate memory for compressed and decompressed data
    unsigned char *compressed_data = malloc(compressed_size);
    unsigned char *decompressed_data = malloc(original_size);
    
    if (!compressed_data || !decompressed_data) {
        fprintf(stderr, "Error: Failed to allocate memory for decompression\n");
        free(compressed_data);
        free(decompressed_data);
        return -1;
    }
    
    // Read the compressed data
    if (fread(compressed_data, 1, compressed_size, source) != compressed_size) {
        fprintf(stderr, "Error: Failed to read compressed data\n");
        free(compressed_data);
        free(decompressed_data);
        return -1;
    }
    
    // Decompress the data in one operation
    uLongf dest_len = original_size;
    int z_result = uncompress(decompressed_data, &dest_len, compressed_data, compressed_size);
    
    // Free the compressed data as we don't need it anymore
    free(compressed_data);
    
    if (z_result != Z_OK) {
        fprintf(stderr, "Error: Decompression failed with code %d\n", z_result);
        free(decompressed_data);
        return -1;
    }
    
    // Write the decompressed data to the destination
    if (fwrite(decompressed_data, 1, dest_len, dest) != dest_len) {
        fprintf(stderr, "Error: Failed to write decompressed data\n");
        free(decompressed_data);
        return -1;
    }
    
    // Also write to buff.out if it's different from dest
    FILE *buff_out = fopen(".syncro/refs/buff.out", "wb");
    if (buff_out && buff_out != dest) {
        if (fwrite(decompressed_data, 1, dest_len, buff_out) != dest_len) {
            fprintf(stderr, "Warning: Failed to write to buff.out\n");
            // Don't return -1 here as this is a supplementary operation
        }
        fclose(buff_out);
    }
    
    // Clean up
    free(decompressed_data);
    
    printf("Decompressed: original size %u bytes\n", original_size);
    return 0;
}
