#pragma once
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>


#define ASSERT_SUCCESS() if (errno != 0){                                  \
    fprintf(stderr,                                                        \
        "\nFailed unittest %s on line %d with error: %s(%d)\n",            \
        __FILE__, __LINE__, strerror(errno), errno                         \
    );                                                                     \
    exit(-1);                                                              \
}

#define ASSERT_FAILURE(XXXX)  if (errno != (XXXX)){\
    fprintf(stderr,                                \
        "\nFailed unittest %s on line %d.\nExpected error state: %s(%d)\nBut instead error is: %s(%d).\n",\
        __FILE__, __LINE__,                        \
        strerror(XXXX), (XXXX),                    \
        strerror(errno), errno                     \
    );                                             \
    exit(-1);                                      \
} errno = 0

#define ASSERT(XXXX) if (!(XXXX)){                             \
    fprintf(stderr,                                            \
        "\nFailed unittest %s on line %d. Assertion failed\n", \
        __FILE__, __LINE__);                                   \
    exit(-1);                                                  \
}
#define ASSERT_EQ(XXXX, YYYY) if ((XXXX) != (YYYY)){                              \
    fprintf(stderr,                                                               \
        "\nFailed unittest %s on line %d. Left value(%lx) != Right value(%lx)\n", \
        __FILE__, __LINE__, (XXXX), (YYYY));                                      \
    exit(-1);                                                                     \
}

