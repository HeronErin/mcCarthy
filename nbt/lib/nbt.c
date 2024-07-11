// McCarthy's                            ,----, 
//                ,--.                 ,/   .`| 
//              ,--.'|    ,---,.     ,`   .'  : 
//          ,--,:  : |  ,'  .'  \  ;    ;     / 
//       ,`--.'`|  ' :,---.' .' |.'___,/    ,'  
//       |   :  :  | ||   |  |: ||    :     |   
//       :   |   \ | ::   :  :  /;    |.';  ;   
//       |   : '  '; |:   |    ; `----'  |  |   
//       '   ' ;.    ;|   :     \    '   :  ;   
//       |   | | \   ||   |   . |    |   |  '   
//       '   : |  ; .''   :  '; |    '   :  |   
//       |   | '`--'  |   |  | ;     ;   |.'    
//       '   : |      |   :   /      '---'      
//       ;   |.'      |   | ,'                  
//       '---'        `----'      Library.
// Not the best NBT implementation, but it works.

// Original Mojang spec: https://web.archive.org/web/20110723210920/http://www.minecraft.net/docs/NBT.txt
// Community documentation: https://wiki.vg/NBT


#include "nbt.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <endian.h>
#include <string.h>
#include "zlib.h"

// Heavily modified fromhttps://stackoverflow.com/questions/49622938/gzip-compression-using-zlib-into-buffer
int gzip_compress(const Bytef *source, uLong sourceLen, Bytef **dest, uLong *destLen) {
    // Allocate memory for the destination buffer
    uLong bound = compressBound(sourceLen);
    *dest = (Bytef *)malloc(bound);

    // Initialize the z_stream structure
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = (Bytef *)source;
    stream.avail_in = sourceLen;
    stream.next_out = *dest;
    stream.avail_out = bound;

    // Initialize deflate with gzip header
    int res = deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
    if (res != Z_OK) {
        free(*dest);
        return res;
    }

    // Compress the data
    res = deflate(&stream, Z_FINISH);
    if (res != Z_STREAM_END) {
        deflateEnd(&stream);
        free(*dest);
        return res == Z_OK ? Z_BUF_ERROR : res;
    }

    // Finalize the compression
    res = deflateEnd(&stream);
    if (res != Z_OK) {
        free(*dest);
        return res;
    }

    *destLen = stream.total_out;
    return Z_OK;
}

int gzip_decompress(const Bytef *source, uLong sourceLen, Bytef **dest, uLong *destLen) {
    // Estimate the size of the decompressed data (adjust if necessary)
    uLong decompressedLen = *destLen;
    *dest = (Bytef *)malloc(decompressedLen);

    // Initialize the z_stream structure
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = (Bytef *)source;
    stream.avail_in = sourceLen;
    stream.next_out = *dest;
    stream.avail_out = decompressedLen;

    // Initialize inflate with gzip header detection
    int res = inflateInit2(&stream, 16 + MAX_WBITS);
    if (res != Z_OK) {
        free(*dest);
        return res;
    }

    // Decompress the data
    res = inflate(&stream, Z_NO_FLUSH);
    if (res != Z_STREAM_END) {
        inflateEnd(&stream);
        free(*dest);
        return res == Z_OK ? Z_BUF_ERROR : res;
    }

    // Finalize the decompression
    res = inflateEnd(&stream);
    if (res != Z_OK) {
        free(*dest);
        return res;
    }

    *destLen = stream.total_out;
    return Z_OK;
}



int populateFromPayload(NbtTag* tagToPopulate, uint8_t type, uint8_t** binRef, uint8_t* extent){
    uint8_t* bin = *binRef;
    tagToPopulate->type = type;
    fprintf(stderr, "Type: %hu\n", type);
    uint16_t datalen;
    switch (type){
        case T_End:
            break;
        
        case T_Byte_Array:
            if (bin+4 > extent){errno = ERANGE; return -1;}
            datalen = be32toh(*(uint32_t*)bin);
            bin+=4;
            goto AFTER_STRING_BIN_INC;
        case T_String:
            if (bin+2 > extent){errno = ERANGE; return -1;}
            datalen = be16toh(*(uint16_t*)bin);
            bin+=2;
            AFTER_STRING_BIN_INC:
            
            if (bin+datalen > extent){errno = ERANGE; return -1;}
            
            char* name = malloc(datalen+1);
            memcpy(name, bin, datalen);
            name[datalen] = 0;
            
            bin += datalen;
            tagToPopulate->stringSize = datalen;
            tagToPopulate->stringBuffer = name;
            break;
        case T_Byte:
            if (bin >= extent){errno = ERANGE; return -1;}
            tagToPopulate->byteValue = *(bin++);
            break;
        case T_Short:
            if (bin + 2 > extent){errno = ERANGE; return -1;}
            tagToPopulate->shortValue = be16toh(*(int16_t*)bin);
            bin+=2;
            break;
        case T_Int:
            if (bin + 4 > extent){errno = ERANGE; return -1;}
            tagToPopulate->intValue = be32toh(*(int32_t*)bin);
            bin+=4;
            break;
        case T_Long:
            if (bin + 8 > extent){errno = ERANGE; return -1;}
            tagToPopulate->longValue = be64toh(*(int64_t*)bin);
            bin+=8;
            break;
        case T_float:
            if (bin + sizeof(float) > extent){errno = ERANGE; return -1;}
            int floatRaw = be32toh(*(int*)bin);  // We need to convert endianness as an int, otherwise things get messed up. 
            tagToPopulate->floatValue =  *(float*)(&floatRaw);
            bin+=sizeof(float);
            break;
        case T_Double:
            if (bin + sizeof(double) > extent){errno = ERANGE; return -1;}
            long doubleRaw = be64toh(*(long*)bin); // Ditto
            tagToPopulate->doubleValue = *(double*)&doubleRaw;
            bin+=sizeof(double);
            break;
        case T_List:
            if (bin + 1 /*byte*/ + 4 /*int*/ > extent){errno = ERANGE; return -1;}
            uint8_t type = *(bin++);
            tagToPopulate->listContentType = type;
            
            tagToPopulate->listSize = be32toh(*(int32_t*)bin);
            bin += 4;
            int size = tagToPopulate->listSize;
            
            NbtTag* tags = malloc(sizeof(NbtTag) * size);
            tagToPopulate->nbtListTags = tags;

            for (int i = 0; i < size; i++){
                if (0 != populateFromPayload(&tags[i], type, &bin, extent))
                    return -1;
            }

            break;
        case T_Compound:
            size_t reserveElementSize = 128;
            CompoundElement** elements = malloc(reserveElementSize * sizeof(void*));
            size_t elementIndex = 0;
            while(1){
                if (bin >= extent) break; // Assume that if we reached the end of the buffer, the compound is over. 

                uint8_t type =  *(bin++);
                if (type == T_End) break;
                if (bin+2 > extent){errno = ERANGE; return -1;}
                CompoundElement* element = malloc(sizeof(CompoundElement));
                if (elementIndex >= reserveElementSize-1){
                    reserveElementSize*=2;
                    elements = realloc(elements, reserveElementSize * sizeof(void*));
                }
                elements[elementIndex++] = element;

                uint16_t strlen = be16toh(*(uint16_t*)bin);
                bin+=2;
                fprintf(stderr, "Ex: %d %hu\n", type, strlen);
                if (bin+strlen > extent){errno = ERANGE; return -1;}
                char* name = malloc(strlen+1);
                memcpy(name, bin, strlen);
                name[strlen] = 0;
                
                element->name = name;
                bin+=strlen;

                if (0 != populateFromPayload(&element->tag, type, &bin, extent))
                    return -1;
            }
            elements[elementIndex] = 0;
            tagToPopulate->compoundLength = elementIndex;
            tagToPopulate->compoundElements = elements;
    }
    *binRef = bin;
    return 0;
}


void printNbt_(NbtTag* element, char* knownName, int knownIndex, int tabCount){
    if (element == NULL) return;
    int size, i;
    int tabCountTemp = tabCount;
    while(tabCountTemp--) putchar('\t');

    if (knownName) printf("%s(\"%s\"", TAG_TYPE_NAMES[element->type], knownName);
    else if (knownIndex >= 0) printf("%s(%d", TAG_TYPE_NAMES[element->type], knownIndex);
    else printf("%s(", TAG_TYPE_NAMES[element->type]);
    switch (element->type) {
        case T_End:
            printf(")\n");
            break;
        case T_Byte_Array:
            printf(") [Containing %d bytes]\n", element->byteArraySize);
            break;
        case T_String:
            printf(") [Containing %d bytes]: %s\n", element->stringSize, element->stringBuffer);
            break;
        case T_Byte:
            printf("): %d\n", element->byteValue);
            break;
        case T_Short:
            printf("): %d\n", element->shortValue);
            break;
        case T_Int:
            printf("): %d\n", element->intValue);
            break;
        case T_Long:
            printf("): %ld\n", element->longValue);
            break;
        case T_float:
            printf("): %f\n", element->floatValue);
            break;
        case T_Double:
            printf("): %f\n", element->doubleValue);
            break;
        case T_List:
            size = element->listSize;
            NbtTag* listElement = element->nbtListTags;
            printf("): %d entries of type %s\n", size, TAG_TYPE_NAMES[element->listContentType]);
            for (i = 0; i < size; i++){
                printNbt_(&listElement[i], NULL, i, tabCount + 1);
            }
            break;
        case T_Compound:
            size = element->compoundLength;
            CompoundElement** elements = element->compoundElements;
            printf("): %d entries\n", size);
            for (i = 0; i < size; i++){
                CompoundElement* element = elements[i];
                if (element != NULL)
                printNbt_(&element->tag, element->name, -1, tabCount + 1);
            }
            break;
    }
}

void printNbt(NbtTag* root){
    printNbt_(root, NULL, -1, 0);
}


NbtTag* parseBinary(uint8_t* bin, size_t len){
    NbtTag* root = malloc(sizeof(NbtTag));
    if (0 != populateFromPayload(root, T_Compound, &bin, bin + len))
        return NULL;
    return root;
}

NbtTag* parseZlibBinary(uint8_t* bin, size_t len){
    size_t compressLen = compressBound(len);
    uint8_t* compressed = (uint8_t*)malloc(compressLen);

    int r = uncompress(compressed, &compressLen, bin, len);
    printf("Dlen %lu: %d\n", compressLen, r);
    NbtTag* nbt = parseBinary(compressed, compressLen);
    free(compressed);
    return nbt;
}
NbtTag* parseGzipBinary(uint8_t* bin, size_t len){
    size_t compressLen = 1024*1024*5;
    uint8_t* compressed = NULL;

    int r = gzip_decompress(bin, len, &compressed, &compressLen);
    printf("Dlen %lu: %d\n", compressLen, r);
    NbtTag* nbt = parseBinary(compressed, compressLen);
    // free(compressed);
    return nbt;
}




