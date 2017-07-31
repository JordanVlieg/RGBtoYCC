#include<stdio.h>
#include<math.h>

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
    return output;
}

int * RGBtoYCC(int RGB[3])
{
    int * product = matrixMult(RGBtoYCCMatrix, RGB);

    for(int x = 0; x < 3; x++)
    {
        product[x] = (product[x]+4096>>13) + scale[x]; //4096 is our magic truncation number
    }
    return product;
}

int * YCCtoRGB(int YCC[3])
{
    for(int x = 0; x < 3; x++)
    {
        YCC[x] = YCC[x] - scale[x];
    }
    int * product = matrixMult(YCCtoRGBMatrix, YCC);
    for(int x = 0; x < 3; x++)
    {
        product[x] = (product[x]+4096>>13);
    }
    return product;
}

void printArray(int *array, int size)
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
