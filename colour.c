#include<stdio.h>
#include<stdlib.h>
#include "colour.h"

uchar scale[3] = {16, 128, 128};
// 3.13 Representation
short RGBtoYCCMatrix[] = {0x839, 0x1021, 0x323, 0xfb44, 0xf6B0, 0xe0C, 0xe0C, 0xf439, 0xfdba};
short BGRtoYCCMatrix[] = {0x323, 0x1021, 0x839, 0xe0C, 0xf6B0, 0xfb44, 0xfdba, 0xf439, 0xe0C};
//float RGBtoYCCMatrix[] = {0.257, 0.504, 0.098, -0.148, -0.291, 0.439, 0.439, -0.368, -0.071};
short YCCtoRGBMatrix[] = {0x253f, 0, 0x3312, 0x253f, 0xf37d, 0xe5fb, 0x253f, 0x408b, 0};
//float YCCtoRGBMatrix[] = {1.164, 0, 1.596, 1.164, -0.391, -0.813, 1.164, 2.017, 0};
short YCCtoBGRMatrix[] = {0x253f, 0x408b, 0, 0x253f, 0xf374, 0xe5fb, 0x253f, 0, 0x3312};
float YCCtoBGRMatrixFFF[] = {1.164, 2.017, 0, 1.164, -0.391, -0.813, 1.164, 0, 1.596};


int * matrixMult(short A[9], uchar B[3])
{
    int * product = calloc(3, sizeof(int));
    int x = 0;
    for(; x < 3; x++)
    {
        product[0] = product[0] + (A[x] * B[x]);
        product[1] = product[1] + (A[x+3] * B[x]);
        product[2] = product[2] + (A[x+6] * B[x]);
    }
    return product;
}

int * matrixMultNEG(short A[9], char B[3])
{
    
}

int * matrixMultNEGF(float A[9], char B[3])
{
    int * product = calloc(3, sizeof(int));

    int yVal = (uchar)B[0];
    product[0] = product[0] + (A[0] * yVal);
    product[1] = product[1] + (A[3] * yVal);
    product[2] = product[2] + (A[6] * yVal);
    product[0] = product[0] + (A[1] * B[1]);
    product[1] = product[1] + (A[4] * B[1]);
    product[2] = product[2] + (A[7] * B[1]);
    product[0] = product[0] + (A[2] * B[2]);
    product[1] = product[1] + (A[5] * B[2]);
    product[2] = product[2] + (A[8] * B[2]);
    return product;
}

uchar * SubSample(uchar * YCC)
{
    uchar * subSamp = calloc(6, sizeof(uchar));
    subSamp[0] = YCC[0];
    subSamp[1] = YCC[3];
    subSamp[2] = YCC[6];
    subSamp[3] = YCC[9];
    int Cb = (YCC[1] + YCC[4] + YCC[7] + YCC[10]) >> 2;
    int Cr = (YCC[2] + YCC[5] + YCC[8] + YCC[11]) >> 2;
    subSamp[4] = Cb;
    subSamp[5] = Cr;
    return subSamp;
}

uchar * SuperSample(uchar * YCC)
{
    uchar * superSamp = calloc(12, sizeof(uchar));
    superSamp[0] = YCC[0];
    superSamp[1] = YCC[4];
    superSamp[2] = YCC[5];
    superSamp[3] = YCC[1];
    superSamp[4] = YCC[4];
    superSamp[5] = YCC[5];
    superSamp[6] = YCC[2];
    superSamp[7] = YCC[4];
    superSamp[8] = YCC[5];
    superSamp[9] = YCC[3];
    superSamp[10] = YCC[4];
    superSamp[11] = YCC[5];
    return superSamp;
}

/*uchar * RGBtoYCC(uchar RGB[3])
{
    int * product = matrixMult(RGBtoYCCMatrix, RGB);
    static uchar ycc[3];
    // Loop unrolling for efficiency
    ycc[0] = (product[0]+4096>>13) + scale[0]; //4096 is our magic rounding number for 3.13
    ycc[1] = (product[1]+4096>>13) + scale[1];
    ycc[2] = (product[2]+4096>>13) + scale[2];
    return ycc;
}*/

void * BGRtoYCC(uchar * colour)
{
    int Y, Cb, Cr;
    Y = BGRtoYCCMatrix[0] * colour[0];
    Cb = BGRtoYCCMatrix[3] * colour[0];
    Cr = BGRtoYCCMatrix[6] * colour[0];
    Y += BGRtoYCCMatrix[1] * colour[1];
    Cb += BGRtoYCCMatrix[4] * colour[1];
    Cr += BGRtoYCCMatrix[7] * colour[1];
    Y += BGRtoYCCMatrix[2] * colour[2];
    Cb += BGRtoYCCMatrix[5] * colour[2];
    Cr += BGRtoYCCMatrix[8] * colour[2];
    colour[0] = (Y+4096>>13) + scale[0]; //4096 is our magic rounding number for 3.13
    colour[1] = (Cb+4096>>13) + scale[1];
    colour[2] = (Cr+4096>>13) + scale[2];
}

void YCCtoBGR(uchar * colour)
{
    uchar yScaled = colour[0] - scale[0];
    char CbScaled = colour[1] - scale[1];
    char CrScaled = colour[2] - scale[2];

    int B, G, R;
    B = YCCtoBGRMatrix[0] * yScaled;
    G = YCCtoBGRMatrix[3] * yScaled;
    R = YCCtoBGRMatrix[6] * yScaled;
    B += YCCtoBGRMatrix[1] * CbScaled;
    G += YCCtoBGRMatrix[4] * CbScaled;
    R += YCCtoBGRMatrix[7] * CbScaled;
    B += YCCtoBGRMatrix[2] * CrScaled;
    G += YCCtoBGRMatrix[5] * CrScaled;
    R += YCCtoBGRMatrix[8] * CrScaled;

    B = (B >= 2088960) ? 2088960: B;
    G = (G >= 2088960) ? 2088960: G;
    R = (R >= 2088960) ? 2088960: R;

    colour[0] = (B < 0) ? 0: (B+4096>>13);
    colour[1] = (G < 0) ? 0: (G+4096>>13);
    colour[2] = (R < 0) ? 0: (R+4096>>13);
    return;
}

/*uchar * YCCtoRGB(int YCC[3])
{
    YCC[0] = YCC[0] - scale[0];
    YCC[1] = YCC[1] - scale[1];
    YCC[2] = YCC[2] - scale[2];
    int * product = matrixMult(YCCtoRGBMatrix, YCC);
    static uchar rgb[3];
    rgb[0] = (product[0]+4096>>13);
    rgb[1] = (product[1]+4096>>13);
    rgb[2] = (product[2]+4096>>13);
    return rgb;
}*/

void printArray(uchar *array, int size)
{
    int x = 0;
    for(; x < size; x++)
    {
        printf("%d ", array[x]);
    }
    printf("\n");
}

/*void main()
{
    printf("Enter Colour Values in format X, X, X\n");
    fflush(stdout);
    int colour[3];
    scanf("%d, %d, %d", colour, colour+1, colour+2);
    fflush(stdout);
    printf("To convert RGB to YCbCr enter '1', for the reverse enter '2': ");
    int choice[2];
    scanf("%d", choice);
    fflush(stdout);
    printf("%d, %d, %d, %d\n\n", colour[0], colour[1], colour[2], choice[0]);
    uchar colourc[3];
    colourc[0] = colour[0];
    colourc[1] = colour[1];
    colourc[2] = colour[2];
    if(choice[0] > 1)
    {
        YCCtoBGR(colourc);
        printArray(colourc, 3);
    }    
    else{
        uchar * shit;
        shit = BGRtoYCC(colourc);
        printArray(shit, 3);
        YCCtoBGR(shit);
        printArray(shit, 3);
        free(shit);
    }  
}/*/