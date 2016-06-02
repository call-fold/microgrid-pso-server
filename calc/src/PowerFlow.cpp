#include "PowerFlow.h"
#include <iostream>
#include <cmath>
#include <cstring>
using namespace std;

char* extern_host;
char* extern_user;
char* extern_password;
char* extern_dbName;
unsigned int extern_port;
unsigned int extern_algorithmID;

void PowerFlow::Initial_ReadData()
{
	readData = new ReadData(extern_host, extern_user, extern_password, extern_dbName, extern_port, NULL, 0, "1", "1");
	readData->ReadData_Bus();
	readData->ReadData_Battery();
	readData->ReadData_Line();
	readData->ReadData_Loads();
	readData->ReadData_PV();
	readData->ReadData_WindM();
	readData->ReadData_Algorithm(extern_algorithmID);

	bus = readData->bus;
	line = readData->line;
	loads = readData->loads;
	battery = readData->battery;
	windm = readData->windm;
	pv = readData->pv;

	numBus = readData->numBus;
	numLine = readData->numLine;
	numLoads = readData->numLoads;
	numBattery = readData->numBattery;
	numPV = readData->numPV;
	numWindM = readData->numWindM;
	numPq = readData->numPq;
	numPv = readData->numPv;
	numSlack = readData->numSlack;
	timeDivide = readData->timeDivide;
	amountLoads = numLoads / timeDivide;
	amountBattery = numBattery / timeDivide;
	amountPV = numPV / timeDivide;
	amountWindM = numWindM / timeDivide;

	numPvPq = numPv + numPq;
}

void PowerFlow::Initial_Matrix()
{
	PowerFlow::Initial_ReadData();

	inv = new InverseMatrix(2 * numPvPq);

	dv = new double[2 * numPvPq];
	memset(dv, 0, sizeof(double) * 2 * numPvPq);
	dw = new double[2 * numPvPq];
	memset(dw, 0, sizeof(double) * 2 * numPvPq);
	dp = new double[numBus];
	memset(dp, 0, sizeof(double) * numBus);
	dq = new double[numBus];
	memset(dq, 0, sizeof(double) * numBus);
	dv2 = new double[numBus];
	memset(dv2, 0, sizeof(double) * numBus);

	p = new double*[numBus];
	p[0] = new double[numBus * timeDivide];
	memset(p[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		p[i + 1] = p[i] + timeDivide;
	}

	q = new double*[numBus];
	q[0] = new double[numBus * timeDivide];
	memset(q[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		q[i + 1] = q[i] + timeDivide;
	}

	ps = new double*[numBus];
	ps[0] = new double[numBus * timeDivide];
	memset(ps[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		ps[i + 1] = ps[i] + timeDivide;
	}

	qs = new double*[numBus];
	qs[0] = new double[numBus * timeDivide];
	memset(qs[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		qs[i + 1] = qs[i] + timeDivide;
	}

	vs = new double*[numBus];
	vs[0] = new double[numBus * timeDivide];
	memset(vs[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		vs[i + 1] = vs[i] + timeDivide;
	}

	pij = new double*[numLine];
	pij[0] = new double[numLine * timeDivide];
	memset(pij[0], 0, sizeof(double) * numLine * timeDivide);
	for (int i = 0; i < numLine - 1; i++)
	{
		pij[i + 1] = pij[i] + timeDivide;
	}

	qij = new double*[numLine];
	qij[0] = new double[numLine * timeDivide];
	memset(qij[0], 0, sizeof(double) * numLine * timeDivide);
	for (int i = 0; i < numLine - 1; i++)
	{
		qij[i + 1] = qij[i] + timeDivide;
	}

	J = new double*[2 * numPvPq];
	J[0] = new double[2 * numPvPq * 2 * numPvPq];
	memset(J[0], 0, sizeof(double) * 2 * numPvPq * 2 * numPvPq);
	for (int i = 0; i < 2 * numPvPq - 1; i++)
	{
		J[i + 1] = J[i] + 2 * numPvPq;
	}

	dMatrix = new double*[2 * numPvPq];
	dMatrix[0] = new double[2 * numPvPq * 2 * numPvPq];
	memset(dMatrix[0], 0, sizeof(double) * 2 * numPvPq * 2 * numPvPq);
	for (int i = 0; i < 2 * numPvPq - 1; i++)
	{
		dMatrix[i + 1] = dMatrix[i] + 2 * numPvPq;
	}

	dReturn = new double*[2 * numPvPq];
	dReturn[0] = new double[2 * numPvPq * 2 * numPvPq];
	memset(dReturn[0], 0, sizeof(double) * 2 * numPvPq * 2 * numPvPq);
	for (int i = 0; i < 2 * numPvPq - 1; i++)
	{
		dReturn[i + 1] = dReturn[i] + 2 * numPvPq;
	}

	matrixY_G = new double*[numBus];
	matrixY_G[0] = new double[numBus * numBus];
	memset(matrixY_G[0], 0, sizeof(double) * numBus * numBus);
	for (int i = 0; i < numBus - 1; i++)
	{
		matrixY_G[i + 1] = matrixY_G[i] + numBus;
	}

	matrixY_B = new double*[numBus];
	matrixY_B[0] = new double[numBus * numBus];
	memset(matrixY_B[0], 0, sizeof(double) * numBus * numBus);
	for (int i = 0; i < numBus - 1; i++)
	{
		matrixY_B[i + 1] = matrixY_B[i] + numBus;
	}

	vm = new double*[numBus];
	vm[0] = new double[numBus * timeDivide];
	memset(vm[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		vm[i + 1] = vm[i] + timeDivide;
	}
}

void PowerFlow::PowerFlow_MatrixY()
{
	PowerFlow::Initial_Matrix();

	//计算节点导纳矩阵
	for (int i = 0; i < numLine; i++)
	{
		double Z2;
		Z2 = line[i].rBus * line[i].rBus + line[i].xBus * line[i].xBus;
		if (0 == line[i].ratioPropotion)
		{
			matrixY_G[line[i].fBus][line[i].fBus] = matrixY_G[line[i].fBus][line[i].fBus] + line[i].rBus / Z2 + line[i].gGround;
			matrixY_B[line[i].fBus][line[i].fBus] = matrixY_B[line[i].fBus][line[i].fBus] - line[i].xBus / Z2 + line[i].bGround / 2;
			matrixY_G[line[i].tBus][line[i].tBus] = matrixY_G[line[i].tBus][line[i].tBus] + line[i].rBus / Z2 + line[i].gGround;
			matrixY_B[line[i].tBus][line[i].tBus] = matrixY_B[line[i].tBus][line[i].tBus] - line[i].xBus / Z2 + line[i].bGround / 2;
			matrixY_G[line[i].tBus][line[i].fBus] = -line[i].rBus / Z2;
			matrixY_B[line[i].tBus][line[i].fBus] = line[i].xBus / Z2;
			matrixY_G[line[i].fBus][line[i].tBus] = matrixY_G[line[i].tBus][line[i].fBus];
			matrixY_B[line[i].fBus][line[i].tBus] = matrixY_B[line[i].tBus][line[i].fBus];
		}
		else
		{
			matrixY_G[line[i].fBus][line[i].fBus] += line[i].rBus / Z2;
			matrixY_B[line[i].fBus][line[i].fBus] -= line[i].xBus / Z2;
			matrixY_G[line[i].tBus][line[i].tBus] += line[i].rBus / Z2 / line[i].ratioPropotion / line[i].ratioPropotion;
			matrixY_B[line[i].tBus][line[i].tBus] -= line[i].xBus / Z2 / line[i].ratioPropotion / line[i].ratioPropotion;
			matrixY_G[line[i].tBus][line[i].fBus] -= line[i].rBus / Z2 / line[i].ratioPropotion;
			matrixY_B[line[i].tBus][line[i].fBus] += line[i].xBus / Z2 / line[i].ratioPropotion;
			matrixY_G[line[i].fBus][line[i].tBus] = matrixY_G[line[i].tBus][line[i].fBus];
			matrixY_B[line[i].fBus][line[i].tBus] = matrixY_B[line[i].tBus][line[i].fBus];
		}
	}

	////输出节点导纳矩阵
	//for (int i = 0; i < numBus; i++)
	//{
	//	for (int j = 0; j < numBus; j++)
	//	{
	//		cout << matrixY_G[i][j] << " " << matrixY_B[i][j];
	//	}
	//	cout << endl;
	//}
}

void PowerFlow::PowerFlow_Calculation_YL(int t, double** ppv, double** pwind, double** pbat)
{
	for (int i = 0; i < numBus; i++)
	{
		bus[i].va = 0;
		bus[i].vm = 1;
	}

	//生成ps数组
	for (int i = 0; i < numBus; i++)
	{
		ps[i][t] = 0;
	}
	//ps初始化
	for (int i = 0; i < amountBattery; i++)
	{
		int a;
		a = battery[i].busIndex;
		ps[a][t] = ps[a][t] + pbat[i][t];
	}
	for (int i = 0; i < amountLoads; i++)
	{
		int a;
		a = loads[i].busIndex;
		ps[a][t] = ps[a][t] - loads[i].pd[t] / 100;
	}

	for (int i = 0; i < amountPV; i++)
	{
		int a;
		a = pv[i].busIndex;
		ps[a][t] = ps[a][t] + ppv[i][t];
	}
	for (int i = 0; i < amountWindM; i++)
	{
		int a;
		a = windm[i].busIndex;
		ps[a][t] = ps[a][t] + pwind[i][t];
	}

	//生成qs数组
	for (int i = 0; i < numBus; i++)
	{
		qs[i][t] = 0;
	}
	//qs初始化

	for (int i = 0; i < amountLoads; i++)
	{
		int a;
		a = loads[i].busIndex;
		qs[a][t] = qs[a][t] - loads[i].qd[t] / 100;
	}


	//生成vs数组
	for (int i = 0; i < numBus; i++)
	{
		vs[i][t] = 0;
	}
	for (int i = 0; i < numBus; i++)
	{
		if (bus[i].busType == 2)
		{
			vs[i][t] = bus[i].vm;
		}
	}


	//计算dp，dq，dw
	for (int i = 0; i < numBus; i++)
	{
		dp[i] = 0;
		dq[i] = 0;
		dv2[i] = 0;
	}


	for (int c = 0; c < 50; c++)
	{

		for (int i = 0; i < numBus; i++)
		{
			p[i][t] = 0;
			q[i][t] = 0;
		}

		for (int i = 0; i < numBus - 1; i++)
		{
			for (int j = 0; j < numBus; j++)
			{
				p[i][t] = p[i][t] + bus[i].vm * (matrixY_G[i][j] * bus[j].vm - matrixY_B[i][j] * bus[j].va) + bus[i].va * (matrixY_G[i][j] * bus[j].va + matrixY_B[i][j] * bus[j].vm);
				q[i][t] = q[i][t] + bus[i].va * (matrixY_G[i][j] * bus[j].vm - matrixY_B[i][j] * bus[j].va) - bus[i].vm * (matrixY_G[i][j] * bus[j].va + matrixY_B[i][j] * bus[j].vm);
			}

		}
		for (int i = 0; i < numBus - 1; i++)
		{
			dp[i] = ps[i][t] - p[i][t];
		}
		for (int i = 0; i < numPq; i++)
		{
			dq[i] = qs[i][t] - q[i][t];
		}
		for (int i = numPq; i < numBus - 1; i++)
		{
			dv2[i] = vs[i][t] * vs[i][t] - bus[i].vm * bus[i].vm - bus[i].va * bus[i].va;
		}
		int b = 0;
		for (int i = 0; i < numBus - 1; i++)
		{
			dw[b] = dp[i];
			b = b + 2;
		}
		int d = 1;
		for (int i = 0; i < numPq; i++)
		{
			dw[d] = dq[i];
			d = d + 2;
		}
		int f = 2 * numPq + 1;
		for (int i = numPq; i < numBus - 1; i++)
		{
			dw[f] = dv2[i];
			f = f + 2;
		}

		//校验收敛
		double max;
		max = fabs(dw[0]);
		for (int i = 1; i < 2 * numPvPq; i++)
		{

			if (max < fabs(dw[i]))
			{
				max = fabs(dw[i]);
			}
		}

		if (max < 1E-5)
		{
			int times1 = 0;
			double delta1;
			for (int i = 0; i < numLine; i++)
			{
				if (pij[i][t] <= line[i].linePMax / 100)
				{
					times1++;

				}
				else
				{
					delta1 = line[i].linePMax / 100 - pij[i][t];
					if (i == 0 || i == 1 || i == 8)
					{
						p[8][t] = p[8][t] - 0.1 * delta1;
					}
					if (i == 2 || i == 3 || i == 4)
					{
						p[7][t] = p[7][t] - 0.1 * delta1;
					}
					if (i == 5 || i == 6 || i == 7)
					{
						p[6][t] = p[6][t] - 0.1 * delta1;
					}

				}

			}
			if (times1 == numLine)
			{
				for (int i = 0; i < numBus; i++)
				{
					vm[i][t] = bus[i].vm;
				}
				break;
			}
		}

		//雅格比矩阵
		for (int i = 0; i < numBus - 1; i++)
		{
			for (int j = 0; j < numBus - 1; j++)
			{
				//先计算pq节点
				if (i < numPq)
				{
					if (i == j)
					{
						J[2 * i][2 * j] = -matrixY_G[i][i] * bus[i].vm - matrixY_B[i][i] * bus[i].va;
						J[2 * i][2 * j + 1] = matrixY_B[i][i] * bus[i].vm - matrixY_G[i][i] * bus[i].va;
						J[2 * i + 1][2 * j] = matrixY_B[i][i] * bus[i].vm - matrixY_G[i][i] * bus[i].va;
						J[2 * i + 1][2 * j + 1] = matrixY_G[i][i] * bus[i].vm + matrixY_B[i][i] * bus[i].va;
					}
					else
					{
						J[2 * i][2 * j] = -matrixY_G[i][j] * bus[i].vm - matrixY_B[i][j] * bus[i].va;
						J[2 * i][2 * j + 1] = matrixY_B[i][j] * bus[i].vm - matrixY_G[i][j] * bus[i].va;
						J[2 * i + 1][2 * j] = matrixY_B[i][j] * bus[i].vm - matrixY_G[i][j] * bus[i].va;
						J[2 * i + 1][2 * j + 1] = matrixY_G[i][j] * bus[i].vm + matrixY_B[i][j] * bus[i].va;
					}
				}
				//pv节点
				else
				{
					if (i == j)
					{
						J[2 * i][2 * j] = -matrixY_G[i][i] * bus[i].vm - matrixY_B[i][i] * bus[i].va;
						J[2 * i][2 * j + 1] = matrixY_B[i][i] * bus[i].vm - matrixY_G[i][i] * bus[i].va;
						J[2 * i + 1][2 * j] = -2 * bus[i].vm;
						J[2 * i + 1][2 * j + 1] = -2 * bus[i].va;
					}
					else
					{
						J[2 * i][2 * j] = -matrixY_G[i][j] * bus[i].vm - matrixY_B[i][j] * bus[i].va;
						J[2 * i][2 * j + 1] = matrixY_B[i][j] * bus[i].vm - matrixY_G[i][j] * bus[i].va;
						J[2 * i + 1][2 * j] = 0;
						J[2 * i + 1][2 * j + 1] = 0;
					}
				}
			}
		}

		for (int i = 0; i < numPq; i++)
		{
			for (int j = 0; j < numBus; j++)
			{
				J[2 * i][2 * i] = J[2 * i][2 * i] - matrixY_G[i][j] * bus[j].vm + matrixY_B[i][j] * bus[j].va;
				J[2 * i][2 * i + 1] = J[2 * i][2 * i + 1] - matrixY_B[i][j] * bus[j].vm - matrixY_G[i][j] * bus[j].va;
				J[2 * i + 1][2 * i] = J[2 * i + 1][2 * i] + matrixY_B[i][j] * bus[j].vm + matrixY_G[i][j] * bus[j].va;
				J[2 * i + 1][2 * i + 1] = J[2 * i + 1][2 * i + 1] - matrixY_G[i][j] * bus[j].vm + matrixY_B[i][j] * bus[j].va;
			}
		}
		for (int i = numPq; i < numBus - 1; i++)
		{
			for (int j = 0; j < numBus; j++)
			{
				J[2 * i][2 * i] = J[2 * i][2 * i] - matrixY_G[i][j] * bus[j].vm + matrixY_B[i][j] * bus[j].va;
				J[2 * i][2 * i + 1] = J[2 * i][2 * i + 1] - matrixY_B[i][j] * bus[j].vm - matrixY_G[i][j] * bus[j].va;
			}
		}

		//求dv;
		//for (int i = 0; i < 2 * numPvPq; i++)
		//{
		//	for (int j = 0; j < 2 * numPvPq; j++)
		//	{
		//		cout << J[i][j] << " ";
		//	}
		//	cout << endl;
		//}

		dMatrix = J;
		dReturn = inv->ReverseMatrix(dMatrix, 2 * numPvPq);

		for (int i = 0; i < 2 * numPvPq; i++)
		{
			dv[i] = 0;
		}
		for (int i = 0; i < 2 * numPvPq; i++)
		{
			for (int j = 0; j < 2 * numPvPq; j++)
			{
				dv[i] = dv[i] - dReturn[i][j] * dw[j];
			}
		}

		//修正节点电压
		for (int i = 0; i < numBus - 1; i++)
		{
			bus[i].vm = bus[i].vm + dv[2 * i];
			bus[i].va = bus[i].va + dv[2 * i + 1];
		}
	}

	//for (int i = 0; i < numBus - 1; i++)
	//{
	//	cout << bus[i].vm << " " << bus[i].va << endl;
	//}

	//平衡节点功率
	for (int i = 0; i < numBus; i++)
	{
		p[numBus - 1][t] = p[numBus - 1][t] + bus[numBus - 1].vm * (matrixY_G[numBus - 1][i] * bus[i].vm - matrixY_B[numBus - 1][i] * bus[i].va) + bus[numBus - 1].va * (matrixY_G[numBus - 1][i] * bus[i].va + matrixY_B[numBus - 1][i] * bus[i].vm);
		q[numBus - 1][t] = q[numBus - 1][t] - bus[numBus - 1].vm * (matrixY_G[numBus - 1][i] * bus[i].va + matrixY_B[numBus - 1][i] * bus[i].vm) + bus[numBus - 1].va * (matrixY_G[numBus - 1][i] * bus[i].vm - matrixY_B[numBus - 1][i] * bus[i].va);
	}
	for (int i = 0; i < numLine; i++)
	{
		for (int j = 0; j < 24; j++)
		{
			pij[i][j] = 0;
			qij[i][j] = 0;
		}
	}

	for (int i = 0; i < numLine; i++)
	{
		double Z2;
		Z2 = line[i].rBus * line[i].rBus + line[i].xBus * line[i].xBus;
		pij[i][t] = 0.5 * line[i].gGround * (bus[line[i].fBus].vm * bus[line[i].fBus].vm + bus[line[i].fBus].va * bus[line[i].fBus].va) + 1 / Z2 * (line[i].rBus * (bus[line[i].fBus].vm * bus[line[i].fBus].vm + bus[line[i].fBus].va * bus[line[i].fBus].va - bus[line[i].fBus].vm * bus[line[i].tBus].vm - bus[line[i].fBus].va * bus[line[i].tBus].va) - line[i].xBus * (bus[line[i].fBus].vm * bus[line[i].tBus].va - bus[line[i].tBus].vm * bus[line[i].fBus].va));
		qij[i][t] = -0.5 * line[i].bGround * (bus[line[i].fBus].vm * bus[line[i].fBus].vm + bus[line[i].fBus].va * bus[line[i].fBus].va) + 1 / Z2 * (line[i].xBus * (bus[line[i].fBus].vm * bus[line[i].fBus].vm + bus[line[i].fBus].va * bus[line[i].fBus].va - bus[line[i].fBus].vm * bus[line[i].tBus].vm - bus[line[i].fBus].va * bus[line[i].tBus].va) + line[i].rBus * (bus[line[i].fBus].vm * bus[line[i].tBus].va - bus[line[i].tBus].vm * bus[line[i].fBus].va));
	}

}

PowerFlow::PowerFlow()
{
	this->PowerFlow_MatrixY();
}

PowerFlow::~PowerFlow()
{
	if (readData)
	{
		delete readData;
		readData = NULL;
	}

	if (bus)
	{
		delete bus;
		bus = NULL;
	}

	if (line)
	{
		delete line;
		line = NULL;
	}

	if (loads)
	{
		delete loads;
		loads = NULL;
	}

	if (battery)
	{
		delete battery;
		battery = NULL;
	}

	if (windm)
	{
		delete windm;
		windm = NULL;
	}

	if (pv)
	{
		delete pv;
		pv = NULL;
	}

	if (inv)
	{
		delete inv;
		inv = NULL;
	}

	if (matrixY_G[0])
	{
		delete[] matrixY_G[0];
		matrixY_G[0] = NULL;
	}
	if (matrixY_G)
	{
		delete[] matrixY_G;
		matrixY_G = NULL;
	}

	if (matrixY_B[0])
	{
		delete[] matrixY_B[0];
		matrixY_B[0] = NULL;
	}
	if (matrixY_B)
	{
		delete[] matrixY_B;
		matrixY_B = NULL;
	}

	if (dv)
	{
		delete[] dv;
		dv = NULL;
	}

	if (dw)
	{
		delete[] dw;
		dw = NULL;
	}

	if (dp)
	{
		delete[] dp;
		dp = NULL;
	}

	if (dq)
	{
		delete[] dq;
		dq = NULL;
	}

	if (dv2)
	{
		delete[] dv2;
		dv2 = NULL;
	}

	if (p[0])
	{
		delete[] p[0];
		p[0] = NULL;
	}
	if (p)
	{
		delete[] p;
		p = NULL;
	}

	if (q[0])
	{
		delete[] q[0];
		q[0] = NULL;
	}
	if (q)
	{
		delete[] q;
		q = NULL;
	}

	if (ps[0])
	{
		delete[] ps[0];
		ps[0] = NULL;
	}
	if (ps)
	{
		delete[] ps;
		ps = NULL;
	}

	if (qs[0])
	{
		delete[] qs[0];
		qs[0] = NULL;
	}
	if (qs)
	{
		delete[] qs;
		qs = NULL;
	}

	if (vs[0])
	{
		delete[] vs[0];
		vs[0] = NULL;
	}
	if (vs)
	{
		delete[] vs;
		vs = NULL;
	}

	if (pij[0])
	{
		delete[] pij[0];
		pij[0] = NULL;
	}
	if (pij)
	{
		delete[] pij;
		pij = NULL;
	}

	if (qij[0])
	{
		delete[] qij[0];
		qij[0] = NULL;
	}
	if (qij)
	{
		delete[] qij;
		qij = NULL;
	}

	if (J[0])
	{
		delete[] J[0];
		J[0] = NULL;
	}
	if (J)
	{
		delete[] J;
		J = NULL;
	}

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

	if (vm[0])
	{
		delete[] vm[0];
		vm[0] = NULL;
	}
	if (vm)
	{
		delete[] vm;
		vm = NULL;
	}
}