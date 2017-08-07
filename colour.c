#include<stdio.h>
#include<stdlib.h>
#include <inttypes.h>
#include "colour.h"

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
    colour[0] = ((Y+4096)>>13) + (scale >> 16); // 4096 is our magic rounding number for 3.13
    colour[1] = ((Cb+4096)>>13) + ((scale >> 8) & 0xFF);
    colour[2] = ((Cr+4096)>>13) + (scale & 0xFF);
    return;
}

int YCCtoBGR(int colour)
{
    int yScaled = ((colour>>16) & 0xFF) - (scale >> 16);
    int CbScaled = ((colour>>8) & 0xFF) - ((scale >> 8) & 0xFF);
    int CrScaled = ((colour) & 0xFF) - (scale & 0xFF);

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
    colour += ((B < 0) ? 0: ((B+4096)>>13)) << 0;
    colour += ((G < 0) ? 0: ((G+4096)>>13)) << 8;
    colour += ((R < 0) ? 0: ((R+4096)>>13)) << 16;
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