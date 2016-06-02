#include <iostream>
#include "InverseMatrix.h"
#include <cstring>

double InverseMatrix::MatrixValue(double** MatrixList, int Level) //求得|A| 如果为0 说明不可逆
{
	//计算行列式的方法
	//  a1 a2 a3
	//  b1 b2 b3
	//  c1 c2 c3
	// 结果为 a1·b2·c3+b1·c2·a3+c1·a2·b3-a3·b2·c1-b3·c2·a1-c3·a2·b1(注意对角线就容易记住了）

	//new dMatrix
	double** dMatrix = new double*[Level];
	dMatrix[0] = new double[Level * Level];
	memset(dMatrix[0], 0, sizeof(double) * Level * Level);
	for (int i = 0; i < Level - 1; i++)
	{
		dMatrix[i + 1] = dMatrix[i] + Level;
	}

	for (int i = 0; i < Level; i++)
		for (int j = 0; j < Level; j++)
			dMatrix[i][j] = MatrixList[i][j];     //将参数的值，付给定义的数组

	double c, x;
	int k = 1;
	for (int i = 0, j = 0; i < Level && j < Level; i++, j++)
	{
		if (dMatrix[i][j] == 0)   //判断对角线上的数据是否为0
		{
			int m = i;
			for (; (m < Level) && (dMatrix[m][j] == 0); m++);  //如果对角线上数据为0，从该数据开始依次往后判断是否为0
			if (m == Level)                      //当该行从对角线开始数据都为0 的时候 返回0
				return 0;
			else
			{
				// Row change between i-row and m-row
				for (int n = j; n < Level; n++)
				{
					c = dMatrix[i][n];
					dMatrix[i][n] = dMatrix[m][n];
					dMatrix[m][n] = c;
				}
				// Change value pre-value
				k *= (-1);
			}
		}
		// Set 0 to the current column in the rows after current row
		for (int s = Level - 1; s > i; s--)
		{
			x = dMatrix[s][j];
			for (int t = j; t < Level; t++)
				dMatrix[s][t] -= dMatrix[i][t] * (x / dMatrix[i][j]);
		}
	}
	double sn = 1;
	for (int i = 0; i < Level; i++)
	{
		if (dMatrix[i][i] != 0)
			sn *= dMatrix[i][i];
		else
			return 0;
	}

	//delete dMatrix
	if (dMatrix[0])
	{
		delete[] dMatrix[0];
		dMatrix[0] = NULL;
	}
	if (dMatrix)
	{
		delete[] dMatrix;
		dMatrix = NULL;
	}

	return k * sn;
}

double** InverseMatrix::ReverseMatrix(double** dMatrix, int Level)
{
	tempLevel = Level;

	double dMatrixValue = MatrixValue(dMatrix, Level);
	if (dMatrixValue == 0) return NULL;       //A为该矩阵 若|A| =0 则该矩阵不可逆 返回空

	//new dReverseMatrix
	double** dReverseMatrix = new double*[Level];
	dReverseMatrix[0] = new double[Level * 2 * Level];
	memset(dReverseMatrix[0], 0, sizeof(double) * Level * 2 * Level);
	for (int i = 0; i < Level - 1; i++)
	{
		dReverseMatrix[i + 1] = dReverseMatrix[i] + 2 * Level;
	}

	double x, c;
	// Init Reverse matrix
	for (int i = 0; i < Level; i++)     //创建一个矩阵（A|I） 以对其进行初等变换 求得其矩阵的逆
	{
		for (int j = 0; j < 2 * Level; j++)
		{
			if (j < Level)
				dReverseMatrix[i][j] = dMatrix[i][j];   //该 （A|I）矩阵前 Level列为矩阵A  后面为数据全部为0
			else
				dReverseMatrix[i][j] = 0;
		}
		dReverseMatrix[i][Level + i] = 1;
		//将Level+1行开始的Level阶 矩阵装换为单位矩阵 （起初的时候该矩阵都为0 现在在把对角线位置装换为1 ）
		//参考http://www.shuxuecheng.com/gaosuzk/content/lljx/wzja/12/12-6.htm
	}

	for (int i = 0, j = 0; i < Level && j < Level; i++, j++)
	{
		if (dReverseMatrix[i][j] == 0)   //判断一行对角线 是否为0
		{
			int m = i;
			for (; dMatrix[m][j] == 0; m++);
			if (m == Level)
				return NULL;  //某行对角线为0的时候 判断该行该数据所在的列在该数据后 是否为0 都为0 的话不可逆 返回空值
			else
			{
				// Add i-row with m-row
				for (int n = j; n < 2 * Level; n++)   //如果对角线为0 则该i行加上m行 m行为（初等变换要求对角线为1，0-->1先加上某行，下面在变1）
					dReverseMatrix[i][n] += dReverseMatrix[m][n];
			}
		}
		//  此时数据： 第二行加上第一行为第一行的数据
		//    1   1   3      1    1    0
		//    1   0   1      0    1    0
		//    4   2   1      0    0    1
		//
		// Format the i-row with "1" start
		x = dReverseMatrix[i][j];
		if (x != 1)                  //如果对角线元素不为1  执行以下
		{
			for (int n = j; n < 2 * Level; n++)
				if (dReverseMatrix[i][n] != 0)
					dReverseMatrix[i][n] /= x;   //相除  使i行第一个数字为1
		}
		// Set 0 to the current column in the rows after current row
		for (int s = Level - 1; s > i; s--)         //该对角线数据为1 时，这一列其他数据 要转换为0
		{
			x = dReverseMatrix[s][j];
			// 第一次时
			//    1      1   3      1    1    0
			//    1      0   1      0    1    0
			//   4(x)   2   1      0    0    1
			//
			for (int t = j; t < 2 * Level; t++)
				dReverseMatrix[s][t] -= (dReverseMatrix[i][t] * x);
			//第一个轮回   用第一行*4 减去第三行 为第三行的数据  依次类推
			//     1      1   3      1    1    0
			//    1      0   1      0    1    0
			//    0(x)   -2  -11    -4   -4   1

		}
	}
	// Format the first matrix into unit-matrix
	for (int i = Level - 2; i >= 0; i--)
		//处理第一行二列的数据 思路如上 就是把除了对角线外的元素转换为0 
	{
		for (int j = i + 1; j < Level; j++)
			if (dReverseMatrix[i][j] != 0)
			{
				c = dReverseMatrix[i][j];
				for (int n = j; n < 2 * Level; n++)
					dReverseMatrix[i][n] -= (c * dReverseMatrix[j][n]);
			}
	}

	for (int i = 0; i < Level; i++)
		for (int j = 0; j < Level; j++)
			dReturn[i][j] = dReverseMatrix[i][j + Level];
	//就是把Level阶的矩阵提取出来（减去原先为单位矩阵的部分）

	//delete dReverseMatrix
	if (dReverseMatrix[0])
	{
		delete[] dReverseMatrix[0];
		dReverseMatrix[0] = NULL;
	}
	if (dReverseMatrix)
	{
		delete[] dReverseMatrix;
		dReverseMatrix = NULL;
	}

	return dReturn;
}

InverseMatrix::InverseMatrix(int Level)
{
	dReturn = new double*[Level];
	dReturn[0] = new double[Level * Level];
	memset(dReturn[0], 0, sizeof(double) * Level * Level);
	for (int i = 0; i < Level - 1; i++)
	{
		dReturn[i + 1] = dReturn[i] + Level;
	}
}

InverseMatrix::~InverseMatrix()
{
	if (dReturn[0])
	{
		delete[] dReturn[0];
		dReturn[0] = NULL;
	}
	if (dReturn)
	{
		delete[] dReturn;
		dReturn = NULL;
	}
}