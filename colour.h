#ifndef CHECKER
#define CHECKER

#define uchar               unsigned char
#define uShort              unsigned short
#define uInt                unsigned int
#define ulong               unsigned long
#define WINDOWS_NT_DIBSIZE  40
#define false               0
#define true                1

void * BGRtoYCC(uchar * colour);

void YCCtoBGR(uchar * colour);

uchar * SubSample(uchar * YCC);

uchar * SuperSample(uchar * YCC);

void printArray(uchar *array, int size);

#endif