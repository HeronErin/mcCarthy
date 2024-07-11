#include "lib/nbt.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(){
    FILE* fp = fopen("hello_world.nbt", "r");
    fseek(fp, 0, SEEK_END);
    size_t l = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t* buff = malloc(l);
    fread(buff, l, 1, fp);
    fclose(fp);
    

    NbtTag* t = parseGzipBinary(buff, l);
    uint8_t* bin;
    size_t size;
    printf("Ret: %d\n", writeBinary(t, &bin, &size));
    // perror(strerror(errno));
    // printNbt(t);

    return 0;
}