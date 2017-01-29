#include <assert.h>
#include <string.h>

#include "../nilsimsa.h"

int main(int argc, char** argv)
{
    const char data1[] = "abcdefgh";
    const char data2[] = "This is a much more ridiculous test because of 21347597.";
    const char data3[] = "The rain in Spain falls mostly in the plains.";
    const char data4[] = "The rain in Spain falls mainly in the plains.";

    int data1_sz = sizeof(data1) / sizeof(data1[0]) - 1;
    int data2_sz = sizeof(data2) / sizeof(data2[0]) - 1;
    int data3_sz = sizeof(data3) / sizeof(data3[0]) - 1;
    int data4_sz = sizeof(data4) / sizeof(data4[0]) - 1;

    char hash1[65] = { 0 };
    char hash2[65] = { 0 };
    char hash3[65] = { 0 };
    char hash4[65] = { 0 };

    nilsimsa_compute(data1, data1_sz, hash1);
    nilsimsa_compute(data2, data2_sz, hash2);
    nilsimsa_compute(data3, data3_sz, hash3);
    nilsimsa_compute(data4, data4_sz, hash4);

    assert(nilsimsa_compare(hash1, hash2) == 4);
    assert(nilsimsa_compare(hash3, hash4) == 92);

    return 0;
}
