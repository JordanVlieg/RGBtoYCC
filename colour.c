#include<stdio.h>

#define uchar unsigned char

unsigned char scale[] = {16, 128, 128};
// 3.13 Representation
short RGBtoYCCMatrix[] = {0x839, 0x1021, 0x323, 0xfb44, 0xf6B0, 0xe0C, 0xe0C, 0xf439, 0xfdba};
//float RGBtoYCCMatrix[] = {0.257, 0.504, 0.098, -0.148, -0.291, 0.439, 0.439, -0.368, -0.071};
short YCCtoRGBMatrix[] = {0x253f, 0, 0x3312, 0x253f, 0xf37d, 0xfd66, 0x253f, 0x408b, 0};
//float YCCtoRGBMatrix[] = {1.164, 0, 1.596, 1.164, -0.391, -.0813, 1.164, 2.017, 0};

int * matrixMult(short A[9], int B[3])
{
    static int product[] = {0,0,0};
    for(int x = 0; x < 3; x++)
    {
        product[0] = product[0] + (A[x] * B[x]);
        product[1] = product[1] + (A[x+3] * B[x]);
        product[2] = product[2] + (A[x+6] * B[x]);
    }
    return product;
}

uchar * SubSample(uchar YCC[12])
{
    static uchar output[6];
    output[0] = YCC[0];
    output[1] = YCC[3];
    output[2] = YCC[6];
    output[3] = YCC[9];
    int Cb = (YCC[1] + YCC[4] + YCC[7] + YCC[10]) >> 2;
    int Cr = (YCC[2] + YCC[5] + YCC[8] + YCC[11]) >> 2;
    output[4] = Cb;
    output[5] = Cr;
    return output;
}

uchar * RGBtoYCC(int RGB[3])
{
    int * product = matrixMult(RGBtoYCCMatrix, RGB);
    static uchar ycc[3];
    // Loop unrolling for efficiency
    ycc[0] = (product[0]+4096>>13) + scale[0]; //4096 is our magic rounding number for 3.13
    ycc[1] = (product[1]+4096>>13) + scale[1];
    ycc[2] = (product[2]+4096>>13) + scale[2];
    return ycc;
}

uchar * YCCtoRGB(int YCC[3])
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
}

void printArray(uchar *array, int size)
{
    for(int x = 0; x < size; x++)
    {
        printf("%d\n", array[x]);
    }
}

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
    if(choice[0] > 1)
        printArray(YCCtoRGB(colour), 3);
    else
        printArray(RGBtoYCC(colour), 3);
}
