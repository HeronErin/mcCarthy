#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "connection/server/serve.c"
#include "connection/server/defaultConfig.c"

#include "descripterMap.c"

int main() {
    bindServer(TESTING_CONFIG);


    return 0;
}
