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

int charArrToInt(uchar * chars)
{
    return (chars[0]<<16)+(chars[1]<<8)+(chars[2]);
}

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
    for(; i<height;i+=2)
    {
        fread(rowBuffA, sizeof(uchar), rowBytesSize, bmpFile);
        fseek(bmpFile, padSize, SEEK_CUR);
        if((height % 2 == 1) && (i == height-1))
        {
            memcpy(rowBuffB, rowBuffA, rowBytesSize);
        }
        else
        {
            fread(rowBuffB, sizeof(uchar), rowBytesSize, bmpFile);
            fseek(bmpFile, padSize, SEEK_CUR);
        }
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
            int shit[4];
            shit[0] = BGRtoYCC(charArrToInt(rowBuffA + j));
            shit[1] = BGRtoYCC(charArrToInt(rowBuffA + j + 3));
            shit[2] = BGRtoYCC(charArrToInt(rowBuffB + j));
            shit[3] = BGRtoYCC(charArrToInt(rowBuffB + j + 3));
            uchar * subsamp;
            subsamp = SubSample(shit);
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
            int shit[4];
            shit[0] = YCCtoBGR(charArrToInt(YCCBuff));
            shit[1] = YCCtoBGR(charArrToInt(YCCBuff + 3));
            shit[2] = YCCtoBGR(charArrToInt(YCCBuff + 6));
            shit[3] = YCCtoBGR(charArrToInt(YCCBuff + 9));
            memcpy((rowBuffA + x), shit, 3);
            memcpy((rowBuffA + x + 3), shit + 1, 3);
            memcpy((rowBuffB + x), shit + 2, 3);
            memcpy((rowBuffB + x + 3), shit + 3, 3);
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
            int shit[4];
            shit[0] = charArrToInt(YCCBuff);
            shit[1] = charArrToInt(YCCBuff + 3);
            shit[0] = YCCtoBGR(shit[0]);
            shit[1] = YCCtoBGR(shit[1]);
            memcpy((rowBuffA + x), shit, 3);
            memcpy((rowBuffA + x + 3), shit+1, 3);
        }
        fwrite(rowBuffA, sizeof(uchar), rowBytesSize, newBmpFile);
        fwrite(pad, sizeof(uchar), padSize, newBmpFile);
    }
    fclose(newBmpFile);

    return(0);
}