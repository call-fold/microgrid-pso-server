#pragma once


class InverseMatrix
{
public:
	double MatrixValue(double** MatrixList, int Level);
	double** ReverseMatrix(double** dMatrix, int Level);
	InverseMatrix(int Level);
	~InverseMatrix();

	double** dReturn;
	int tempLevel;
};
