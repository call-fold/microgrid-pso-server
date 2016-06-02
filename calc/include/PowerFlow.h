#pragma once

#include "Component.h"
#include "ReadData.h"
#include "InverseMatrix.h"

extern char* extern_host;
extern char* extern_user;
extern char* extern_password;
extern char* extern_dbName;
extern unsigned int extern_port;
extern unsigned int extern_algorithmID;

class PowerFlow
{
public:
	void Initial_ReadData();
	void Initial_Matrix(); //这个里面会调用上一个初始化
	void PowerFlow_MatrixY(); //这个里面会调用上一个初始化, 这个在构造函数中先调用

	//void PowerFlow_Calculation(int p, int t);
	void PowerFlow_Calculation_YL(int t, double** ppv, double** pwind, double** pbat);

	PowerFlow();
	~PowerFlow();
	
	ReadData* readData;

	int numBus;
	int numLine;
	int numLoads;
	int numBattery;
	int numPV; //光伏
	int numWindM;
	int numPq;
	int numPv;
	int numSlack;
	int timeDivide;
	int amountLoads;
	int amountBattery;
	int amountPV;
	int amountWindM;

	int numPvPq;

	Bus* bus;
	Line* line;
	Loads* loads;
	Battery* battery;
	WindM* windm;
	PV* pv;
	InverseMatrix* inv;

	double** matrixY_G;
	double** matrixY_B;
	double** vm;

	//double* powerflow_pd;
	//double* powerflow_qd;
	//double** ykb;

	double* dv;
	double* dw;
	double** p;
	double** q;
	double** ps;
	double** qs;
	double** vs;
	double** pij;
	double** qij;
	double** J;
	double** dMatrix;
	double** dReturn;
	double* dp;
	double* dq;
	double* dv2;
};

