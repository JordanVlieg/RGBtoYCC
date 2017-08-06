#ifndef CHECKER
#define CHECKER

#define uchar               unsigned char
#define uShort              unsigned short
#define uInt                unsigned int
#define ulong               unsigned long
#define WINDOWS_NT_DIBSIZE  40
#define false               0
#define true                1
#define INPUT_FILE           "./Fox.bmp"
#define OUTPUT_FILE           "./FoxSUB.bmp"
#define COMPRESSED_FILE           "./ycc.jzv"
// YCC to BGR
#define _1dot164        (int16_t)0x253f
#define _2dot017        (int16_t)0x408b
#define _n0dot391       (int16_t)0xf374
#define _n0dot813       (int16_t)0xe5fb
#define _1dot596        (int16_t)0x3312
// BGR to YCC
#define _0dot098        (int16_t)0x323
#define _0dot504        (int16_t)0x1021
#define _0dot257        (int16_t)0x839
#define _0dot439        (int16_t)0xe0C
#define _n0dot291       (int16_t)0xf6B0
#define _n0dot148       (int16_t)0xfb44
#define _n0dot071       (int16_t)0xfdba
#define _n0dot368       (int16_t)0xf439

#define yScale          (uint16_t)16
#define CbScale         (uint16_t)128
#define CrScale         (uint16_t)128

void BGRtoYCC(uchar * colour);

void YCCtoBGR(uchar * colour);

uchar * SubSample(uchar * YCC);

uchar * SuperSample(uchar * YCC);

void printArray(uchar *array, int size);

#endif