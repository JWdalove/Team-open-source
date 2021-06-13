#include "stdio.h"
#include "stdlib.h"
#include "math.h"

//把二维的数组与一维数组的转换，也可以直接用二维数组，只是我的习惯是不用二维数组
#define ParaBuffer(Buffer,Row,Col) (*(Buffer + (Row) * (SizeSrc + 1) + (Col)))
//

/***********************************************************************************
从txt文件里读取double型的X，Y数据
txt文件里的存储格式为
X1  Y1
X2  Y2
X3  Y3
X4  Y4
X5  Y5
X6  Y6
X7  Y7
X8  Y8
函数返回X，Y，以及数据的数目（以组为单位）
***********************************************************************************/
static int GetXY(const char* FileName, double* X, double* Y, int* Amount)
{
        FILE* File = fopen(FileName, "r");
        if (!File)
                return -1;
        for (*Amount = 0; !feof(File); X++, Y++, (*Amount)++)
                if (2 != fscanf(File, (const char*)"%lf %lf", X, Y))
                        break;
        fclose(File);
        return 0;
}

/***********************************************************************************
打印系数矩阵，只用于调试，不具备运算功能
对于一个N阶拟合，它的系数矩阵大小是（N + 1）行（N + 2）列
double* Para：系数矩阵存储地址
int SizeSrc：系数矩阵大小（SizeSrc）行（SizeSrc + 1）列
***********************************************************************************/
static int PrintPara(double* Para, int SizeSrc)
{
        int i, j;
        for (i = 0; i < SizeSrc; i++)
        {
                for (j = 0; j <= SizeSrc; j++)
                        printf("%10.6lf ", ParaBuffer(Para, i, j));
                printf("\r\n");
        }
        printf("\r\n");
        return 0;
}

/***********************************************************************************
系数矩阵的限幅处理，防止它溢出，目前这个函数很不完善，并不能很好地解决这个问题
原理：矩阵解行列式，同一行乘以一个系数，行列式的解不变
当然，相对溢出问题，还有一个精度问题，也是同样的思路，现在对于这两块的处理很不完善，有待优化
以行为单位处理
***********************************************************************************/
static int ParalimitRow(double* Para, int SizeSrc, int Row)
{
        int i;
        double Max, Min, Temp;
        for (Max = abs(ParaBuffer(Para, Row, 0)), Min = Max, i = SizeSrc; i; i--)
        {
                Temp = abs(ParaBuffer(Para, Row, i));
                if (Max < Temp)
                        Max = Temp;
                if (Min > Temp)
                        Min = Temp;
        }
        Max = (Max + Min) * 0.000005;
        for (i = SizeSrc; i >= 0; i--)
                ParaBuffer(Para, Row, i) /= Max;
        return 0;
}

/***********************************************************************************
同上，以矩阵为单位处理
***********************************************************************************/
static int Paralimit(double* Para, int SizeSrc)
{
        int i;
        for (i = 0; i < SizeSrc; i++)
                if (ParalimitRow(Para, SizeSrc, i))
                        return -1;
        return 0;
}

/***********************************************************************************
系数矩阵行列式变换
***********************************************************************************/
static int ParaPreDealA(double* Para, int SizeSrc, int Size)
{
        int i, j;
        for (Size -= 1, i = 0; i < Size; i++)
        {
                for (j = 0; j < Size; j++)
                        ParaBuffer(Para, i, j) = ParaBuffer(Para, i, j) * ParaBuffer(Para, Size, Size) - ParaBuffer(Para, Size, j) * ParaBuffer(Para, i, Size);
                ParaBuffer(Para, i, SizeSrc) = ParaBuffer(Para, i, SizeSrc) * ParaBuffer(Para, Size, Size) - ParaBuffer(Para, Size, SizeSrc) * ParaBuffer(Para, i, Size);
                ParaBuffer(Para, i, Size) = 0;
                ParalimitRow(Para, SizeSrc, i);
        }
        return 0;
}

/***********************************************************************************
系数矩阵行列式变换，与ParaPreDealA配合
完成第一次变换，变换成三角矩阵
***********************************************************************************/
static int ParaDealA(double* Para, int SizeSrc)
{
        int i;
        for (i = SizeSrc; i; i--)
                if (ParaPreDealA(Para, SizeSrc, i))
                        return -1;
        return 0;
}

/***********************************************************************************
系数矩阵行列式变换
***********************************************************************************/
static int ParaPreDealB(double* Para, int SizeSrc, int OffSet)
{
        int i, j;
        for (i = OffSet + 1; i < SizeSrc; i++)
        {
                for (j = OffSet + 1; j <= i; j++)
                        ParaBuffer(Para, i, j) *= ParaBuffer(Para, OffSet, OffSet);
                ParaBuffer(Para, i, SizeSrc) = ParaBuffer(Para, i, SizeSrc) * ParaBuffer(Para, OffSet, OffSet) - ParaBuffer(Para, i, OffSet) * ParaBuffer(Para, OffSet, SizeSrc);
                ParaBuffer(Para, i, OffSet) = 0;
                ParalimitRow(Para, SizeSrc, i);
        }
        return 0;
}

/***********************************************************************************
系数矩阵行列式变换，与ParaPreDealB配合
完成第一次变换，变换成对角矩阵，变换完毕
***********************************************************************************/
static int ParaDealB(double* Para, int SizeSrc)
{
        int i;
        for (i = 0; i < SizeSrc; i++)
                if (ParaPreDealB(Para, SizeSrc, i))
                        return -1;
        for (i = 0; i < SizeSrc; i++)
        {
                if (ParaBuffer(Para, i, i))
                {
                        ParaBuffer(Para, i, SizeSrc) /= ParaBuffer(Para, i, i);
                        ParaBuffer(Para, i, i) = 1.0;
                }
        }
        return 0;
}
/***********************************************************************************
系数矩阵变换
***********************************************************************************/
static int ParaDeal(double* Para, int SizeSrc)
{
        PrintPara(Para, SizeSrc);
        Paralimit(Para, SizeSrc);
        PrintPara(Para, SizeSrc);
        if (ParaDealA(Para, SizeSrc))
                return -1;
        PrintPara(Para, SizeSrc);
        if (ParaDealB(Para, SizeSrc))
                return -1;
        return 0;
}

/***********************************************************************************
最小二乘法的第一步就是从XY数据里面获取系数矩阵
double* Para：系数矩阵地址
const double* X：X数据地址
const double* Y：Y数据地址
int Amount：XY数据组数
int SizeSrc：系数矩阵大小（SizeSrc）行（SizeSrc + 1）列
***********************************************************************************/
static int GetParaBuffer(double* Para, const double* X, const double* Y, int Amount, int SizeSrc)
{
        int i, j;
        for (i = 0; i < SizeSrc; i++)
                for (ParaBuffer(Para, 0, i) = 0, j = 0; j < Amount; j++)
                        ParaBuffer(Para, 0, i) += pow(*(X + j), 2 * (SizeSrc - 1) - i);
        for (i = 1; i < SizeSrc; i++)
                for (ParaBuffer(Para, i, SizeSrc - 1) = 0, j = 0; j < Amount; j++)
                        ParaBuffer(Para, i, SizeSrc - 1) += pow(*(X + j), SizeSrc - 1 - i);
        for (i = 0; i < SizeSrc; i++)
                for (ParaBuffer(Para, i, SizeSrc) = 0, j = 0; j < Amount; j++)
                        ParaBuffer(Para, i, SizeSrc) += (*(Y + j)) * pow(*(X + j), SizeSrc - 1 - i);
        for (i = 1; i < SizeSrc; i++)
                for (j = 0; j < SizeSrc - 1; j++)
                        ParaBuffer(Para, i, j) = ParaBuffer(Para, i - 1, j + 1);
        return 0;
}

/***********************************************************************************
整个计算过程
***********************************************************************************/
int Cal(const double* BufferX, const double* BufferY, int Amount, int SizeSrc, double* ParaResK)
{
        double* ParaK = (double*)malloc(SizeSrc * (SizeSrc + 1) * sizeof(double));
        GetParaBuffer(ParaK, BufferX, BufferY, Amount, SizeSrc);
        ParaDeal(ParaK, SizeSrc);
        for (Amount = 0; Amount < SizeSrc; Amount++, ParaResK++)
                *ParaResK = ParaBuffer(ParaK, Amount, SizeSrc);
        free(ParaK);
        return 0;
}

/***********************************************************************************
测试main函数
数据组数：20
阶数：5
***********************************************************************************/
int main2(int argc, char* argv[])
{
        //数据组数
        int Amount;
        //XY缓存，系数矩阵缓存
        double BufferX[1024], BufferY[1024], ParaK[6];
        if (GetXY((const char*)"test.txt", (double*)BufferX, (double*)BufferY, &Amount))
                return 0;
        //运算
        Cal((const double*)BufferX, (const double*)BufferY, Amount, sizeof(ParaK) / sizeof(double), (double*)ParaK);
        //输出系数
        for (Amount = 0; Amount < sizeof(ParaK) / sizeof(double); Amount++)
                printf("ParaK[%d] = %lf!\r\n", Amount, ParaK[Amount]);
        //屏幕暂留
        system("pause");
        return 0;
}
