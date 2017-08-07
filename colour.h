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
#define _1dot164        0x253f
#define _2dot017        0x408b
#define _n0dot391       0xfffff374
#define _n0dot813       0xffffe5fb
#define _1dot596        0x3312
// BGR to YCC
#define _0dot098        0x323
#define _0dot504        0x1021
#define _0dot257        0x839
#define _0dot439        0xe0C
#define _n0dot291       0xfffff6B0
#define _n0dot148       0xfffffb44
#define _n0dot071       0xfffffdba
#define _n0dot368       0xfffff439

#define yScale          (uint16_t)16
#define CbScale         (uint16_t)128
#define CrScale         (uint16_t)128

#define scale           (int)((16<<16)+(128<<8)+128)

int BGRtoYCC(int colour);

int YCCtoBGR(int colour);

long SubSample(int * pix);

uchar * SuperSample(uchar * YCC);

void printArray(uchar *array, int size);

#endif