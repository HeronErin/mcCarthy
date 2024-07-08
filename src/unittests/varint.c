#include "testing.h"
#include "../decoding/datatypes.h"
#include "../common.h"


const uint8_t test1[] = {0}; // Decimal value: 0
const uint8_t test2[] = {1}; // Decimal value: 1
const uint8_t test3[] = {2}; // Decimal value: 2
const uint8_t test4[] = {0x7f}; // Decimal value: 127
const uint8_t test5[] = {0x80, 1}; // Decimal value: 128
const uint8_t test6[] = {0xFF, 1}; // Decimal value: 255
const uint8_t test7[] = {0xdd, 0xc7, 1}; // Decimal value: 25565
int main(){
    BUFF* test1b = MK_BUFF(test1);
    int test1c = decodeVarInt(test1b);
    ASSERT_SUCCESS();
    ASSERT_EQ(0, test1c);
    ASSERT_EQ(test1b->index, 1);

    BUFF* test2b = MK_BUFF(test2);
    int test2c = decodeVarInt(test2b);
    ASSERT_SUCCESS();
    ASSERT_EQ(1, test2c);
    ASSERT_EQ(test2b->index, 1);

    BUFF* test3b = MK_BUFF(test3);
    int test3c = decodeVarInt(test3b);
    ASSERT_SUCCESS();
    ASSERT_EQ(2, test3c);
    ASSERT_EQ(test3b->index, 1);

    BUFF* test4b = MK_BUFF(test4);
    int test4c = decodeVarInt(test4b);
    ASSERT_SUCCESS();
    ASSERT_EQ(127, test4c);
    ASSERT_EQ(test4b->index, 1);


    BUFF* test5b = MK_BUFF(test5);
    int test5c = decodeVarInt(test5b);
    ASSERT_SUCCESS();
    ASSERT_EQ(128, test5c);
    ASSERT_EQ(test5b->index, 2);

    BUFF* test6b = MK_BUFF(test6);
    int test6c = decodeVarInt(test6b);
    ASSERT_SUCCESS();
    ASSERT_EQ(255, test6c);
    ASSERT_EQ(test6b->index, 2);
    
    BUFF* test7b = MK_BUFF(test7);
    int test7c = decodeVarInt(test7b);
    ASSERT_SUCCESS();
    ASSERT_EQ(25565, test7c);
    ASSERT_EQ(test7b->index, 3);

    

    return 0;

}



