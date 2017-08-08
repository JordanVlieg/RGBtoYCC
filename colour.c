#include<stdio.h>
#include<stdlib.h>
#include <inttypes.h>
#include "colour.h"

int * SubSample(int * pix)
{
    int * subSamp = calloc(2, sizeof(int));
    int tmp = 0;
    tmp += ((pix[0] >> 16) & 0xFF);
    tmp += ((pix[1] >> 16) & 0xFF) << 8;
    tmp += ((pix[2] >> 16) & 0xFF) << 16;
    tmp += ((pix[3] >> 16) & 0xFF) << 24;
    int Cb = (((pix[0] >> 8) & 0xFF) + ((pix[1] >> 8) & 0xFF) + ((pix[2] >> 8) & 0xFF) + ((pix[3] >> 8) & 0xFF)) >> 2;
    Cb += (((pix[0] & 0xFF) + (pix[1] & 0xFF) + (pix[2] & 0xFF) + (pix[3] & 0xFF)) >> 2) << 8;
    subSamp[0] = tmp;
    subSamp[1] = Cb;
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

int BGRtoYCC(int colour)
{
    /* Clamping is not required in this function as for all possible RGB values,
     the YCbCr will never fall outside the allowed range (Tested)*/
    int Y, Cb, Cr;
    Y = _0dot098 * ((colour>>16) & 0xFF);
    Cb = _0dot439 * ((colour>>16) & 0xFF);
    Cr = _n0dot071 * ((colour>>16) & 0xFF);
    Y += _0dot504 * ((colour>>8) & 0xFF);
    Cb += _n0dot291 * ((colour>>8) & 0xFF);
    Cr += _n0dot368 * ((colour>>8) & 0xFF);
    Y += _0dot257 * ((colour) & 0xFF);
    Cb += _n0dot148 * ((colour) & 0xFF);
    Cr += _0dot439 * ((colour) & 0xFF);
    colour ^= colour;
    colour += (((Y+ROUNDING_CONST) >> MANTISSA_BITS) + yScale) << 16;
    colour += (((Cb+ROUNDING_CONST) >> MANTISSA_BITS) + CbScale) << 8;
    colour += ((Cr+ROUNDING_CONST) >> MANTISSA_BITS) + CrScale;
    return colour;
}

int YCCtoBGR(int colour)
{
    int yScaled = ((colour>>16) & 0xFF) - yScale;
    int CbScaled = ((colour>>8) & 0xFF) - CbScale;
    int CrScaled = ((colour) & 0xFF) - CrScale;

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
        BGRtoYCC(8287309);
        //YCCtoBGR(colourc);
        //printArray(colourc, 3);
    }    
    else{
        BGRtoYCC(8287309);
        //printArray(colourc, 3);
        //int blah = charArrToInt(colourc);
        //int newBlah = YCCtoBGR(blah);
        //printf("%d  %d  %d\n", newBlah>>24, (newBlah>>16)&0xFF, (newBlah>>8)&0xFF);
        //printArray(shit, 3);
        //free(shit);
    }  
}
*/