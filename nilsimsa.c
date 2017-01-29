/*!
 * The MIT License (MIT)
 * =====================
 *
 * Copyright 2017 Sepehr Laal
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "nilsimsa.h"

#ifdef __cplusplus
extern "C" {
#endif

static int __score(int num);
static int __tran3(int a, int b, int c, int n);
static int __hexchar_to_int(char digit);
static const char* __int_to_hexchar(int num);

#define ACCUM_LENGTH 256
#define DIGEST_LENGTH 32
#define WINDOW_LENGTH 4 

void nilsimsa_compute(const char* data, int n, char* out)
{
    int accum[ACCUM_LENGTH] = { 0 };
    int digest[DIGEST_LENGTH] = { 0 };
    int window[WINDOW_LENGTH] = { -1, -1, -1, -1 };

    for (auto i = 0; i < n; ++i)
    {
        int ch = data[i] & 0xFF;

        if (window[1] > -1)
        {
            accum[__tran3(ch, window[0], window[1], 0)] += 1;
        }

        if (window[2] > -1)
        {
            accum[__tran3(ch, window[0], window[2], 1)] += 1;
            accum[__tran3(ch, window[1], window[2], 2)] += 1;
        }

        if (window[3] > -1)
        {
            accum[__tran3(ch, window[0], window[3], 3)] += 1;
            accum[__tran3(ch, window[1], window[3], 4)] += 1;
            accum[__tran3(ch, window[2], window[3], 5)] += 1;
            accum[__tran3(window[3], window[0], ch, 6)] += 1;
            accum[__tran3(window[3], window[2], ch, 7)] += 1;
        }

        window[3] = window[2];
        window[2] = window[1];
        window[1] = window[0];
        window[0] = ch;
    }

    int total = 0;

    if (n == 3)
        total = 1;
    else if (n == 4)
        total = 4;
    else if (n > 4)
        total = 8 * n - 28;

    int threshold = total / ACCUM_LENGTH;

    for (int i = 0; i < ACCUM_LENGTH; i++)
        if (accum[i] > threshold)
            digest[i >> 3] += 1 << (i & 7);

    int rev_index = 0;

    for (int i = 0; i < DIGEST_LENGTH; ++i)
    {
        rev_index = DIGEST_LENGTH - i - 1;
        out[2*i  ] = __int_to_hexchar(digest[rev_index])[0];
        out[2*i+1] = __int_to_hexchar(digest[rev_index])[1];
    }

    out[2 * DIGEST_LENGTH] = '\0';
}

int nilsimsa_compare(const char* hash1, const char* hash2)
{
    int digest1[DIGEST_LENGTH] = { 0 };
    int digest2[DIGEST_LENGTH] = { 0 };

    for (int i = 0; i < DIGEST_LENGTH; ++i)
    {
        digest1[i] = (__hexchar_to_int(hash1[2 * i]) << 4) + __hexchar_to_int(hash1[2 * i + 1]);
        digest2[i] = (__hexchar_to_int(hash2[2 * i]) << 4) + __hexchar_to_int(hash2[2 * i + 1]);
    }

    int bits = 0;
    for (int i = 0; i < DIGEST_LENGTH; i++)
        bits += __score(255 & (digest1[i] ^ digest2[i]));
    return 128 - bits;
}

//////////////////////////////////////////////////////////////////////////
// implementation detail

// This is a precomputed constant for the standard Nilsimsa "53"-based transition table.
static const int __TRAN53[] =
{
    0x02, 0xD6, 0x9E, 0x6F, 0xF9, 0x1D, 0x04, 0xAB, 0xD0, 0x22, 0x16, 0x1F, 0xD8, 0x73, 0xA1, 0xAC,
    0x3B, 0x70, 0x62, 0x96, 0x1E, 0x6E, 0x8F, 0x39, 0x9D, 0x05, 0x14, 0x4A, 0xA6, 0xBE, 0xAE, 0x0E,
    0xCF, 0xB9, 0x9C, 0x9A, 0xC7, 0x68, 0x13, 0xE1, 0x2D, 0xA4, 0xEB, 0x51, 0x8D, 0x64, 0x6B, 0x50,
    0x23, 0x80, 0x03, 0x41, 0xEC, 0xBB, 0x71, 0xCC, 0x7A, 0x86, 0x7F, 0x98, 0xF2, 0x36, 0x5E, 0xEE,
    0x8E, 0xCE, 0x4F, 0xB8, 0x32, 0xB6, 0x5F, 0x59, 0xDC, 0x1B, 0x31, 0x4C, 0x7B, 0xF0, 0x63, 0x01,
    0x6C, 0xBA, 0x07, 0xE8, 0x12, 0x77, 0x49, 0x3C, 0xDA, 0x46, 0xFE, 0x2F, 0x79, 0x1C, 0x9B, 0x30,
    0xE3, 0x00, 0x06, 0x7E, 0x2E, 0x0F, 0x38, 0x33, 0x21, 0xAD, 0xA5, 0x54, 0xCA, 0xA7, 0x29, 0xFC,
    0x5A, 0x47, 0x69, 0x7D, 0xC5, 0x95, 0xB5, 0xF4, 0x0B, 0x90, 0xA3, 0x81, 0x6D, 0x25, 0x55, 0x35,
    0xF5, 0x75, 0x74, 0x0A, 0x26, 0xBF, 0x19, 0x5C, 0x1A, 0xC6, 0xFF, 0x99, 0x5D, 0x84, 0xAA, 0x66,
    0x3E, 0xAF, 0x78, 0xB3, 0x20, 0x43, 0xC1, 0xED, 0x24, 0xEA, 0xE6, 0x3F, 0x18, 0xF3, 0xA0, 0x42,
    0x57, 0x08, 0x53, 0x60, 0xC3, 0xC0, 0x83, 0x40, 0x82, 0xD7, 0x09, 0xBD, 0x44, 0x2A, 0x67, 0xA8,
    0x93, 0xE0, 0xC2, 0x56, 0x9F, 0xD9, 0xDD, 0x85, 0x15, 0xB4, 0x8A, 0x27, 0x28, 0x92, 0x76, 0xDE,
    0xEF, 0xF8, 0xB2, 0xB7, 0xC9, 0x3D, 0x45, 0x94, 0x4B, 0x11, 0x0D, 0x65, 0xD5, 0x34, 0x8B, 0x91,
    0x0C, 0xFA, 0x87, 0xE9, 0x7C, 0x5B, 0xB1, 0x4D, 0xE5, 0xD4, 0xCB, 0x10, 0xA2, 0x17, 0x89, 0xBC,
    0xDB, 0xB0, 0xE2, 0x97, 0x88, 0x52, 0xF7, 0x48, 0xD3, 0x61, 0x2C, 0x3A, 0x2B, 0xD1, 0x8C, 0xFB,
    0xF1, 0xCD, 0xE4, 0x6A, 0xE7, 0xA9, 0xFD, 0xC4, 0x37, 0xC8, 0xD2, 0xF6, 0xDF, 0x58, 0x72, 0x4E
};

/*
* This is an optimization table for doing bitwise vector comparisons. The
* population count of x, POPC[x], is the number of 1's in the binary
* representation of x. The bitwise XOR(a, b) applied within this table,
* POPC[a ^ b], is the Hamming distance between a and b. For more
* information, see http://en.wikipedia.org/wiki/Hamming_weight.
*/
static const int __POPC[] =
{
    0x00, 0x01, 0x01, 0x02, 0x01, 0x02, 0x02, 0x03, 0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04,
    0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04, 0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05,
    0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04, 0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05,
    0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06,
    0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04, 0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05,
    0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06,
    0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06,
    0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07,
    0x01, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x04, 0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05,
    0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06,
    0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06,
    0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07,
    0x02, 0x03, 0x03, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06,
    0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07,
    0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x05, 0x06, 0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07,
    0x04, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06, 0x07, 0x05, 0x06, 0x06, 0x07, 0x06, 0x07, 0x07, 0x08
};

/*
* Used to convert integers to 1-byte hex strings
* Integers in between the range of 0 and 256.
*/
static const char* __HEX_BYTE_DIGITS[] =
{
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
    "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
    "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
    "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
    "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
    "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
};

static int __tran3(int a, int b, int c, int n)
{
    return (((__TRAN53[(a + n) & 255] ^ __TRAN53[b] * (n + n + 1)) + __TRAN53[c ^ __TRAN53[n]]) & 255);
}

static int __hexchar_to_int(char digit)
{
    return ((digit | 432) * 239'217'992 & 0xffff'ffff) >> 28;
}

static const char* __int_to_hexchar(int num)
{
    return __HEX_BYTE_DIGITS[num];
}

static int __score(int num)
{
    return __POPC[num];
}

#ifdef __cplusplus
} //!extern "C"
#endif
