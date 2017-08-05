#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "colour.h"


uchar headers[54];
ulong rowSize;
uInt height;
uInt offset;
uchar * rowBuffA;
uchar * rowBuffB;

uInt littleEndToInt(uchar *bytes)
{
    return bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24);
}

uShort littleEndToShort(uchar *bytes)
{
    return bytes[0] + (bytes[1] << 8);
}

ulong getFileSize(uchar *header)
{
    return littleEndToInt(header + 2);
}

int parseHeaders(FILE *bmpFile)
{
    fread(headers, sizeof(uchar), 54, bmpFile);
    if(littleEndToShort(headers) != 0x4d42)
    {
        // Wrong BMP type, must be windows.
        return false;
    }
    if(littleEndToInt(headers + 14) != WINDOWS_NT_DIBSIZE)
    {
        // Wrong DIB header size, not windows NT.
        return false;
    }
    offset = littleEndToInt(headers + 10);
    rowSize = ((littleEndToInt(headers + 18) * 24 +31) / 32) * 4;
    //printf("Row Size: %lu\n", rowSize);
    height = littleEndToInt(headers + 22);
    if(littleEndToShort(headers  + 28) != 24)
    {
        // Incorrect bits per pixel
        return false;
    }
    if(littleEndToInt(headers + 30) != 0)
    {
        // Already compressed, not a valid BMP
        return false;
    }
    return true;

}

void printByteArrayHex(uchar *bytes, int size)
{
    int i = 0;
    for(;i<size; i++)
    {
        printf("%x ", bytes[i]);
    }
    printf("\n\n\n");
}

int main()
{
    FILE *bmpFile;
    FILE *newBmpFile;
    FILE *yccFile;
    bmpFile = fopen("./Fox.bmp", "r");
    if(bmpFile == NULL) 
    {
        perror("Error opening file");
        return -1;
    }
    if(!parseHeaders(bmpFile))
    {
        perror("Invalid BMP file");
        return -1;
    }
    fclose(bmpFile);

    bmpFile = fopen("./Fox.bmp", "r");
    yccFile = fopen("./ycc.hex", "w+");
    fseek (bmpFile, offset, SEEK_SET);
    /*
    printf("File size: %lu\n", getFileSize(headers));
    printf("Offset: %u\n", offset);
    printf("widthBytes: %lu\n", rowSize);
    printf("Height: %u\n", height);
    */

    rowBuffA = malloc(rowSize);
    rowBuffB = malloc(rowSize);
    
    int i = 0;
    for(; i<height;i+=2)
    {
        fread(rowBuffA, sizeof(uchar), rowSize, bmpFile);
        fread(rowBuffB, sizeof(uchar), rowSize, bmpFile);
        ulong j = 0;
        for(;j<rowSize;j+=6)
        {
            uchar * YCCBuff = malloc(12);
            BGRtoYCC(rowBuffA + j);
            BGRtoYCC(rowBuffA + j + 3);
            BGRtoYCC(rowBuffB + j);
            BGRtoYCC(rowBuffB + j + 3);
            YCCBuff[0] = rowBuffA[j];
            YCCBuff[1] = rowBuffA[j + 1];
            YCCBuff[2] = rowBuffA[j + 2];
            YCCBuff[3] = rowBuffA[j + 3];
            YCCBuff[4] = rowBuffA[j + 4];
            YCCBuff[5] = rowBuffA[j + 5];
            YCCBuff[6] = rowBuffB[j];
            YCCBuff[7] = rowBuffB[j + 1];
            YCCBuff[8] = rowBuffB[j + 2];
            YCCBuff[9] = rowBuffB[j + 3];
            YCCBuff[10] = rowBuffB[j + 4];
            YCCBuff[11] = rowBuffB[j + 5];

            uchar * subsamp;
            subsamp = SubSample(YCCBuff);
            free(YCCBuff);
            fwrite(subsamp, sizeof(uchar), 6, yccFile);
            free(subsamp);
        }
    }
    free(rowBuffA);
    free(rowBuffB);

    fclose(bmpFile);
    fclose(yccFile);

    yccFile = fopen("./ycc.hex", "r");
    newBmpFile = fopen("./FoxSub.bmp", "w+");
    fwrite(headers, sizeof(uchar), 54, newBmpFile);

    uchar * subRow;
    subRow = malloc(rowSize);
    rowBuffA = malloc(rowSize);
    rowBuffB = malloc(rowSize);

    int y = 0;
    for(; y<height; y+=2)
    {
        uchar * YCCBuff;
        uchar * BGRBuff[12];
        uchar *pixBuff[3];
        fread(subRow, sizeof(uchar), rowSize, yccFile);
        int x = 0;
        for(;x<rowSize; x+=6)
        {
            YCCBuff = SuperSample(subRow + x);
            YCCtoBGR(YCCBuff);
            YCCtoBGR(YCCBuff + 3);
            YCCtoBGR(YCCBuff + 6);
            YCCtoBGR(YCCBuff + 9);
            memcpy((rowBuffA + x), YCCBuff, 6);
            memcpy((rowBuffB + x), (YCCBuff + 6), 6);
        }
        fwrite(rowBuffA, sizeof(uchar), rowSize, newBmpFile);
        fwrite(rowBuffB, sizeof(uchar), rowSize, newBmpFile);
    }
    fclose(newBmpFile);

    return(0);
}