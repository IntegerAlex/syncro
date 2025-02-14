#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "inflate.h"
#define CHUNK 16384

int zlib_inflate(FILE *source, FILE *dest) {
    int ret;
    z_stream strm = {0};
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    // Initialize zlib inflate stream
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
        fprintf(stderr, "zlib initialization error: %s\n",
                ret == Z_MEM_ERROR ? "Memory error" :
                ret == Z_VERSION_ERROR ? "Version mismatch" :
                "Invalid initialization");
        return -1;
    }

    // Decompression loop
    do {
        strm.avail_in = (uInt)fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            fprintf(stderr, "Error reading compressed data: %s\n", strerror(errno));
            inflateEnd(&strm);
            return -1;
        }
        if (strm.avail_in == 0)
            break;

        strm.next_in = in;

        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;

            ret = inflate(&strm, Z_NO_FLUSH);
            if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                fprintf(stderr, "Decompression error: %s\n", 
                        ret == Z_NEED_DICT ? "Need dictionary" :
                        ret == Z_DATA_ERROR ? "Corrupted data" :
                        "Memory error");
                inflateEnd(&strm);
                return -1;
            }

            size_t have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have) {
                fprintf(stderr, "Error writing decompressed data: %s\n", strerror(errno));
                inflateEnd(&strm);
                return -1;
            }
        } while (strm.avail_out == 0);
    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);
    
    if (ret != Z_STREAM_END) {
        fprintf(stderr, "Decompression did not complete properly\n");
        return -1;
    }

    return 0;
}

