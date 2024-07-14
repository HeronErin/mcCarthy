#include "lib/nbt.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(){
    FILE* fp = fopen("bigtest.nbt", "r");
    fseek(fp, 0, SEEK_END);
    size_t l = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t* buff = malloc(l);
    fread(buff, l, 1, fp);
    fclose(fp);
    

    NbtTag* t = parseGzipBinary(buff, l);
    if (t == NULL) exit(69);

    printNbt(t);


    return 0;
}