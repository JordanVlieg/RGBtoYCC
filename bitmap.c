#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "colour.h"


uchar headers[54];
ulong rowBytesSize;
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
    rowBytesSize = littleEndToInt(headers + 18) * 3;
    //rowBytesSize = ((littleEndToInt(headers + 18) * 24 +31) / 32) * 4;
    //printf("Row Size: %lu\n", rowBytesSize);
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
    return;
}

int main()
{
    FILE *bmpFile;
    FILE *newBmpFile;
    FILE *yccFile;
    bmpFile = fopen(INPUT_FILE, "r");
    if(bmpFile == NULL) 
    {
        perror("Error opening input file");
        return -1;
    }
    if(!parseHeaders(bmpFile))
    {
        perror("Invalid BMP file");
        return -1;
    }
    fclose(bmpFile);

    bmpFile = fopen(INPUT_FILE, "r");
    yccFile = fopen(COMPRESSED_FILE, "w+");
    fseek (bmpFile, offset, SEEK_SET);
    int padSize = 4 - (rowBytesSize % 4);
    rowBuffA = malloc(rowBytesSize + padSize);
    rowBuffB = malloc(rowBytesSize + padSize);
    
    
    int i = 0;
    for(; i<height-1;i+=2)
    {
        fread(rowBuffA, sizeof(uchar), rowBytesSize, bmpFile);
        fseek(bmpFile, padSize, SEEK_CUR);
        fread(rowBuffB, sizeof(uchar), rowBytesSize, bmpFile);
        fseek(bmpFile, padSize, SEEK_CUR);
        if(padSize % 2 == 1)
        {
            // JANKY SHIT BE HAPPNIN HERE
            rowBuffA[rowBytesSize] = rowBuffA[rowBytesSize - 3];
            rowBuffA[rowBytesSize + 1] = rowBuffA[rowBytesSize - 2];
            rowBuffA[rowBytesSize + 2] = rowBuffA[rowBytesSize - 1];
            rowBuffB[rowBytesSize] = rowBuffB[rowBytesSize - 3];
            rowBuffB[rowBytesSize + 1] = rowBuffB[rowBytesSize - 2];
            rowBuffB[rowBytesSize + 2] = rowBuffB[rowBytesSize - 1];
        }
        ulong j = 0;
        for(;j<rowBytesSize;j+=6)
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
    if(height % 2 == 1)
    {
        fread(rowBuffA, sizeof(uchar), rowBytesSize, bmpFile);
        fseek(bmpFile, padSize, SEEK_CUR);
        memcpy(rowBuffB, rowBuffA, rowBytesSize);
        if(padSize % 2 == 1)
        {
            // JANKY SHIT BE HAPPNIN HERE
            rowBuffA[rowBytesSize] = rowBuffA[rowBytesSize - 3];
            rowBuffA[rowBytesSize + 1] = rowBuffA[rowBytesSize - 2];
            rowBuffA[rowBytesSize + 2] = rowBuffA[rowBytesSize - 1];
            rowBuffB[rowBytesSize] = rowBuffB[rowBytesSize - 3];
            rowBuffB[rowBytesSize + 1] = rowBuffB[rowBytesSize - 2];
            rowBuffB[rowBytesSize + 2] = rowBuffB[rowBytesSize - 1];
        }
        ulong j = 0;
        for(;j<rowBytesSize;j+=6)
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

    yccFile = fopen(COMPRESSED_FILE, "r");
    newBmpFile = fopen(OUTPUT_FILE, "w+");
    fwrite(headers, sizeof(uchar), 54, newBmpFile);

    long subRowByteSize = rowBytesSize + (rowBytesSize % 6);

    uchar * subRow;
    subRow = malloc(subRowByteSize);
    rowBuffA = malloc(subRowByteSize);
    rowBuffB = malloc(subRowByteSize);

    uchar pad[3] = {0,0,0};

    int y = 0;
    for(; y<height-1; y+=2)
    {
        uchar * YCCBuff;
        fread(subRow, sizeof(uchar), subRowByteSize, yccFile);
        int x = 0;
        for(;x<rowBytesSize; x+=6)
        {
            YCCBuff = SuperSample(subRow + x);
            YCCtoBGR(YCCBuff);
            YCCtoBGR(YCCBuff + 3);
            YCCtoBGR(YCCBuff + 6);
            YCCtoBGR(YCCBuff + 9);
            memcpy((rowBuffA + x), YCCBuff, 6);
            memcpy((rowBuffB + x), (YCCBuff + 6), 6);
        }
        fwrite(rowBuffA, sizeof(uchar), rowBytesSize, newBmpFile);
        fwrite(pad, sizeof(uchar), padSize, newBmpFile);
        fwrite(rowBuffB, sizeof(uchar), rowBytesSize, newBmpFile);
        fwrite(pad, sizeof(uchar), padSize, newBmpFile);
    }
    if(height % 2 == 1)
    {
        uchar * YCCBuff;
        fread(subRow, sizeof(uchar), subRowByteSize, yccFile);
        int x = 0;
        for(;x<rowBytesSize; x+=6)
        {
            YCCBuff = SuperSample(subRow + x);
            YCCtoBGR(YCCBuff);
            YCCtoBGR(YCCBuff + 3);
            memcpy((rowBuffA + x), YCCBuff, 6);
        }
        fwrite(rowBuffA, sizeof(uchar), rowBytesSize, newBmpFile);
        fwrite(pad, sizeof(uchar), padSize, newBmpFile);
    }
    fclose(newBmpFile);

    return(0);
}