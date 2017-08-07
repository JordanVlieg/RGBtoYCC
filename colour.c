#include<stdio.h>
#include<stdlib.h>
#include <inttypes.h>
#include "colour.h"

/*uchar scale[3] = {16, 128, 128};
// 3.13 Representation
short RGBtoYCCMatrix[] = {0x839, 0x1021, 0x323, 0xfb44, 0xf6B0, 0xe0C, 0xe0C, 0xf439, 0xfdba};
short BGRtoYCCMatrix[] = {0x323, 0x1021, 0x839, 0xe0C, 0xf6B0, 0xfb44, 0xfdba, 0xf439, 0xe0C};
//float RGBtoYCCMatrix[] = {0.257, 0.504, 0.098, -0.148, -0.291, 0.439, 0.439, -0.368, -0.071};

short YCCtoRGBMatrix[] = {0x253f, 0, 0x3312, 0x253f, 0xf37d, 0xe5fb, 0x253f, 0x408b, 0};
//float YCCtoRGBMatrix[] = {1.164, 0, 1.596, 1.164, -0.391, -0.813, 1.164, 2.017, 0};
short YCCtoBGRMatrix[] = {0x253f, 0x408b, 0, 0x253f, 0xf374, 0xe5fb, 0x253f, 0, 0x3312};
float YCCtoBGRMatrixFFF[] = {1.164, 2.017, 0, 1.164, -0.391, -0.813, 1.164, 0, 1.596};
*/

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

void BGRtoYCC(uchar * colour)
{
    /* Clamping is not required in this function as for all possible RGB values,
     the YCbCr will never fall outside the allowed range (Tested)*/
    int Y, Cb, Cr;
    Y = _0dot098 * colour[0];
    Cb = _0dot439 * colour[0];
    Cr = _n0dot071 * colour[0];
    Y += _0dot504 * colour[1];
    Cb += _n0dot291 * colour[1];
    Cr += _n0dot368 * colour[1];
    Y += _0dot257 * colour[2];
    Cb += _n0dot148 * colour[2];
    Cr += _0dot439 * colour[2];
    colour[0] = ((Y+ROUNDING_CONST) >> MANTISSA_BITS) + (SCALE >> 16);
    colour[1] = ((Cb+ROUNDING_CONST) >> MANTISSA_BITS) + ((SCALE >> 8) & 0xFF);
    colour[2] = ((Cr+ROUNDING_CONST) >> MANTISSA_BITS) + (SCALE & 0xFF);
    return;
}

int YCCtoBGR(int colour)
{
    int yScaled = ((colour>>16) & 0xFF) - (SCALE >> 16);
    int CbScaled = ((colour>>8) & 0xFF) - ((SCALE >> 8) & 0xFF);
    int CrScaled = ((colour) & 0xFF) - (SCALE & 0xFF);

    int B, G, R;
    B = _1dot164 * yScaled;
    G = _1dot164 * yScaled;
    R = _1dot164 * yScaled;
    B += _2dot017 * CbScaled;
    G += _n0dot391 * CbScaled;
    G += _n0dot813 * CrScaled;
    R += _1dot596 * CrScaled;

    B = (B >= 2088960) ? 2088960: B;
    G = (G >= 2088960) ? 2088960: G;
    R = (R >= 2088960) ? 2088960: R;

    colour = 0;
    colour += ((B < 0) ? 0: ((B+ROUNDING_CONST)>>MANTISSA_BITS)) << 0;
    colour += ((G < 0) ? 0: ((G+ROUNDING_CONST)>>MANTISSA_BITS)) << 8;
    colour += ((R < 0) ? 0: ((R+ROUNDING_CONST)>>MANTISSA_BITS)) << 16;
    return colour;
}

/*uchar * RGBtoYCC(uchar RGB[3])
{
    int * product = matrixMult(RGBtoYCCMatrix, RGB);
    static uchar ycc[3];
    // Loop unrolling for efficiency
    ycc[0] = (product[0]+ROUNDING_CONST>>MANTISSA_BITS) + scale[0]; //ROUNDING_CONST is our magic rounding number for 3.13
    ycc[1] = (product[1]+ROUNDING_CONST>>MANTISSA_BITS) + scale[1];
    ycc[2] = (product[2]+ROUNDING_CONST>>MANTISSA_BITS) + scale[2];
    return ycc;
}

uchar * YCCtoRGB(int YCC[3])
{
    YCC[0] = YCC[0] - scale[0];
    YCC[1] = YCC[1] - scale[1];
    YCC[2] = YCC[2] - scale[2];
    int * product = matrixMult(YCCtoRGBMatrix, YCC);
    static uchar rgb[3];
    rgb[0] = (product[0]+ROUNDING_CONST>>MANTISSA_BITS);
    rgb[1] = (product[1]+ROUNDING_CONST>>MANTISSA_BITS);
    rgb[2] = (product[2]+ROUNDING_CONST>>MANTISSA_BITS);
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
    return;
}

/*
void main()
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
        //YCCtoBGR(colourc);
        printArray(colourc, 3);
    }    
    else{
        BGRtoYCC(colourc);
        printArray(colourc, 3);
        int blah = charArrToInt(colourc);
        int newBlah = YCCtoBGR(blah);
        printf("%d  %d  %d\n", newBlah>>24, (newBlah>>16)&0xFF, (newBlah>>8)&0xFF);
        //printArray(shit, 3);
        //free(shit);
    }  
}
*/