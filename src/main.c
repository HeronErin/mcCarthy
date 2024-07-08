#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

int main() {
    const char *input = "This is the text to be compressed.";
    uLong sourceLen = strlen(input) + 1;  // Include the null terminator

    // Allocate memory for the destination buffer
    uLong destLen = compressBound(sourceLen); // Determine the maximum size of the compressed data
    Bytef *dest = (Bytef *)malloc(destLen);

    // Compress the data
    int res = compress(dest, &destLen, (const Bytef *)input, sourceLen);
    if (res != Z_OK) {
        printf("Compression failed with error code %d\n", res);
        free(dest);
        return 1;
    }

    // Print the size of the compressed data
    printf("Original size: %lu\n", sourceLen);
    printf("Compressed size: %lu\n", destLen);

    // Free the allocated memory
    free(dest);

    return 0;
}
