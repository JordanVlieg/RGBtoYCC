#include<stdio.h>
#include<math.h>

int scale[] = {16, 128, 128};
// 3.13 Representation
float RGBtoYCCMatrix[] = {2105, 4128, 802, 33980, 35151, 3596, 3596, 35782, 33349};
//float YCCtoRGBMatrix[] = {1.164, 0, 1.596, 1.164, -0.391, -.0813, 1.164, 2.017, 0};

float * matrixMult(float A[9], int B[3])
{
    static float f_product[] = {0,0,0};
    for(int x = 0; x < 3; x++)
    {
        f_product[0] = f_product[0] + A[x] * B[x];
        f_product[1] = f_product[1] + A[x+3] * B[x];
        f_product[2] = f_product[2] + A[x+6] * B[x];
    }

    return f_product;
}

int * RGBtoYCC(int RGB[3])
{
    float * f_product = matrixMult(RGBtoYCCMatrix, RGB);
    static int product[] = {0,0,0};

    for(int x = 0; x < 3; x++)
    {
        product[x] = round(f_product[x]) + scale[x];
    }
    return product;
}

/*int * YCCtoRGB(int YCC[3])
{
    for(int x = 0; x < 3; x++)
    {
        YCC[x] = YCC[x] - scale[x];
    }
    float * f_product = matrixMult(YCCtoRGBMatrix, YCC);
    static int product[] = {0,0,0};
    for(int x = 0; x < 3; x++)
    {
        product[x] = round(f_product[x]);
    }
    return product;
}*/

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
    int Colour[3];
    scanf("%d, %d, %d", Colour, Colour+1, Colour+2);
    fflush(stdout);
    
    printf("To convert RGB to YCbCr enter '1', for the reverse enter '2': ");
    int choice[1];
    scanf("%d", choice);
    fflush(stdout);
    if(choice[0] > 1)
        printArray(YCCtoRGB(Colour), 3);
    else
        printArray(RGBtoYCC(Colour), 3);
}
