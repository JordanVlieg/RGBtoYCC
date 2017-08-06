#ifndef CHECKER
#define CHECKER

#define uchar               unsigned char
#define uShort              unsigned short
#define uInt                unsigned int
#define ulong               unsigned long
#define WINDOWS_NT_DIBSIZE  40
#define false               0
#define true                1
#define INPUT_FILE           "./huge.bmp"
#define OUTPUT_FILE           "./hugeSUB.bmp"
#define COMPRESSED_FILE           "./ycc.jzv"
/*
#define 1.164   (int16_t)0x253f
#define 2.017   (int16_t)0x253f
#define 0       (int16_t)0x0
#define -0.391   (int16_t)0xf374
#define -0.813   (int16_t)0xe5fb
#define 1.596  (int16_t)0x3312
*/

//short YCCtoBGRMatrix[] = {0x253f, 0x408b, 0, 0x253f, 0xf374, 0xe5fb, 0x253f, 0, 0x3312};
//float YCCtoBGRMatrixFFF[] = {1.164, 2.017, 0, 1.164, -0.391, -0.813, 1.164, 0, 1.596};


void BGRtoYCC(uchar * colour);

void YCCtoBGR(uchar * colour);

uchar * SubSample(uchar * YCC);

uchar * SuperSample(uchar * YCC);

void printArray(uchar *array, int size);

#endif