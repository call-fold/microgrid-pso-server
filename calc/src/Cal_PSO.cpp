#include "mpi.h"
//#include <winsock2.h>
#include "mysql/mysql.h"
#include "Cal_PSO.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <ctime>

extern unsigned int extern_algorithmID;

void PSO::Initial()
{
	ad = new Adjustment();

	bus = ad->pf->readData->bus;
	loads = ad->pf->readData->loads;
	battery = ad->pf->readData->battery;
	windm = ad->pf->readData->windm;
	pv = ad->pf->readData->pv;

	host = ad->pf->readData->host;
	user = ad->pf->readData->user;
	pass = ad->pf->readData->pass;
	dbName = ad->pf->readData->dbName;
	port = ad->pf->readData->port;
	unixSocket = ad->pf->readData->unixSocket;
	flag = ad->pf->readData->flag;

	iter = ad->pf->readData->algorithm.iter; //各粒子迭代次数
	gathernum = ad->pf->readData->algorithm.gatherNum;
	g = ad->pf->readData->algorithm.gmax; //各进程粒子数
	w = ad->pf->readData->algorithm.w;
	c1 = ad->pf->readData->algorithm.c1;
	c2 = ad->pf->readData->algorithm.c2;

	pn = 100.0;
	pSlackMax = 1.3;
	pSlackMin = -3.5;
	f1 = 0;
	f2 = 0;
	f3 = 0;

	numBus = ad->pf->readData->numBus;
	numBattery = ad->pf->readData->numBattery;
	numPV = ad->pf->readData->numPV;
	numWindM = ad->pf->readData->numWindM;
	timeDivide = ad->pf->readData->timeDivide;
	amountBattery = numBattery / timeDivide;
	amountPV = numPV / timeDivide;
	amountWindM = numWindM / timeDivide;

	fx = new double[g];
	fpBest = new double[g];
	fpBestT = new double[3001];

	tmp = new double*[(amountBattery + amountPV + amountWindM + 1)];
	tmp[0] = new double[(amountBattery + amountPV + amountWindM + 1) * timeDivide];
	memset(tmp[0], 0, sizeof(double) * (amountBattery + amountPV + amountWindM + 1) * timeDivide);
	for (int i = 0; i < (amountBattery + amountPV + amountWindM + 1) - 1; i++)
	{
		tmp[i + 1] = tmp[i] + timeDivide;
	}

	vMax = new double*[(amountBattery + amountPV + amountWindM + 1)];
	vMax[0] = new double[(amountBattery + amountPV + amountWindM + 1) * timeDivide];
	memset(vMax[0], 0, sizeof(double) * (amountBattery + amountPV + amountWindM + 1) * timeDivide);
	for (int i = 0; i < (amountBattery + amountPV + amountWindM + 1) - 1; i++)
	{
		vMax[i + 1] = vMax[i] + timeDivide;
	}

	adj_p1 = new double*[(amountBattery + amountPV + amountWindM + 1)];
	adj_p1[0] = new double[(amountBattery + amountPV + amountWindM + 1) * timeDivide];
	memset(adj_p1[0], 0, sizeof(double) * (amountBattery + amountPV + amountWindM + 1) * timeDivide);
	for (int i = 0; i < (amountBattery + amountPV + amountWindM + 1) - 1; i++)
	{
		adj_p1[i + 1] = adj_p1[i] + timeDivide;
	}

	adj_pBat = new double*[amountBattery];
	adj_pBat[0] = new double[amountBattery * timeDivide];
	memset(adj_pBat[0], 0, sizeof(double) * amountBattery * timeDivide);
	for (int i = 0; i < amountBattery - 1; i++)
	{
		adj_pBat[i + 1] = adj_pBat[i] + timeDivide;
	}

	adj_pPV = new double*[amountPV];
	adj_pPV[0] = new double[amountPV * timeDivide];
	memset(adj_pPV[0], 0, sizeof(double) * amountPV * timeDivide);
	for (int i = 0; i < amountPV - 1; i++)
	{
		adj_pPV[i + 1] = adj_pPV[i] + timeDivide;
	}

	adj_pWindM = new double*[amountWindM];
	adj_pWindM[0] = new double[amountWindM * timeDivide];
	memset(adj_pWindM[0], 0, sizeof(double) * amountWindM * timeDivide);
	for (int i = 0; i < amountWindM - 1; i++)
	{
		adj_pWindM[i + 1] = adj_pWindM[i] + timeDivide;
	}

	adj_SOC = new double*[amountBattery];
	adj_SOC[0] = new double[amountBattery * (timeDivide + 1)];
	memset(adj_SOC[0], 0, sizeof(double) * amountBattery * (timeDivide + 1));
	for (int i = 0; i < amountBattery - 1; i++)
	{
		adj_SOC[i + 1] = adj_SOC[i] + (timeDivide + 1);
	}

	adj_p9 = new double*[numBus];
	adj_p9[0] = new double[numBus * timeDivide];
	memset(adj_p9[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		adj_p9[i + 1] = adj_p9[i] + timeDivide;
	}

	adj_vm = new double*[numBus];
	adj_vm[0] = new double[numBus * timeDivide];
	memset(adj_vm[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		adj_vm[i + 1] = adj_vm[i] + timeDivide;
	}

	gBest = new double*[(amountBattery + amountPV + amountWindM + 1)];
	gBest[0] = new double[(amountBattery + amountPV + amountWindM + 1) * timeDivide];
	memset(gBest[0], 0, sizeof(double) * (amountBattery + amountPV + amountWindM + 1) * timeDivide);
	for (int i = 0; i < (amountBattery + amountPV + amountWindM + 1) - 1; i++)
	{
		gBest[i + 1] = gBest[i] + timeDivide;
	}

	x = new double**[g];
	for (int i = 0; i < g; i++)
	{
		x[i] = new double*[(amountBattery + amountPV + amountWindM + 1)];
	}
	x[0][0] = new double[(amountBattery + amountPV + amountWindM + 1) * timeDivide * g];
	memset(x[0][0], 0, sizeof(double) * (amountBattery + amountPV + amountWindM + 1) * timeDivide * g);
	for (int i = 0; i < g - 1; i++)
	{
		x[i + 1][0] = x[i][0] + (amountBattery + amountPV + amountWindM + 1) * timeDivide;
	}
	for (int i = 0; i < g; i++)
	{
		for (int j = 0; j < (amountBattery + amountPV + amountWindM + 1) - 1; j++)
		{
			x[i][j + 1] = x[i][j] + timeDivide;
		}
	}

	v = new double**[g];
	for (int i = 0; i < g; i++)
	{
		v[i] = new double*[(amountBattery + amountPV + amountWindM + 1)];
	}
	v[0][0] = new double[(amountBattery + amountPV + amountWindM + 1) * timeDivide * g];
	memset(v[0][0], 0, sizeof(double) * (amountBattery + amountPV + amountWindM + 1) * timeDivide * g);
	for (int i = 0; i < g - 1; i++)
	{
		v[i + 1][0] = v[i][0] + (amountBattery + amountPV + amountWindM + 1) * timeDivide;
	}
	for (int i = 0; i < g; i++)
	{
		for (int j = 0; j < (amountBattery + amountPV + amountWindM + 1) - 1; j++)
		{
			v[i][j + 1] = v[i][j] + timeDivide;
		}
	}

	pBest = new double**[g];
	for (int i = 0; i < g; i++)
	{
		pBest[i] = new double*[(amountBattery + amountPV + amountWindM + 1)];
	}
	pBest[0][0] = new double[(amountBattery + amountPV + amountWindM + 1) * timeDivide * g];
	memset(pBest[0][0], 0, sizeof(double) * (amountBattery + amountPV + amountWindM + 1) * timeDivide * g);
	for (int i = 0; i < g - 1; i++)
	{
		pBest[i + 1][0] = pBest[i][0] + (amountBattery + amountPV + amountWindM + 1) * timeDivide;
	}
	for (int i = 0; i < g; i++)
	{
		for (int j = 0; j < (amountBattery + amountPV + amountWindM + 1) - 1; j++)
		{
			pBest[i][j + 1] = pBest[i][j] + timeDivide;
		}
	}

	best_SOC = new double*[amountBattery];
	best_SOC[0] = new double[amountBattery * (timeDivide + 1)];
	memset(best_SOC[0], 0, sizeof(double) * amountBattery * (timeDivide + 1));
	for (int i = 0; i < amountBattery - 1; i++)
	{
		best_SOC[i + 1] = best_SOC[i] + (timeDivide + 1);
	}


}

void PSO::Initial_PSO(int _numProcs)
{
	//vMax初始化
	for (int i = 0; i < amountBattery + amountPV + amountWindM + 1; i++)
	{
		for (int t = 0; t < timeDivide; t++)
		{
			if (i < amountBattery)
			{
				vMax[i][t] = (0.7 - (-0.7)) / 5;//蓄电池容量在0.3-1之间
			}
			else if (i < amountBattery + amountPV)
			{
				vMax[i][t] = (pv[i - amountBattery].pvPMax[t] / pn - pv[i - amountBattery].pvPMin[t] / pn) / 5;//pv
			}
			else if (i < amountBattery + amountPV + amountWindM)
			{
				vMax[i][t] = (windm[i - amountBattery - amountPV].windmPMax[t] / pn - windm[i - amountBattery - amountPV].windmPMin[t] / pn) / 5;//wind
			}
			else
			{
				vMax[i][t] = (pSlackMax - pSlackMin) / 5;
			}
		}
	}

	//初始化x,v,pBest
	for (int m = 0; m < g; m++)
	{
		for (int i = 0; i < amountBattery + amountPV + amountWindM + 1; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				if (i < amountBattery)
				{
					x[m][i][j] = -0.7 + Random01() * (0.7 - (-0.7));
				}
				else if (i < amountBattery + amountPV)
				{
					x[m][i][j] = pv[i - amountBattery].pvPMin[j] / pn + Random01() * (pv[i - amountBattery].pvPMax[j] / pn - pv[i - amountBattery].pvPMin[j] / pn);
				}
				else if (i < amountBattery + amountPV + amountWindM)
				{
					x[m][i][j] = windm[i - amountBattery - amountPV].windmPMin[j] / pn + Random01() * (windm[i - amountBattery - amountPV].windmPMax[j] / pn - windm[i - amountBattery - amountPV].windmPMin[j] / pn);
				}
				else
				{
					x[m][i][j] = pSlackMin + Random01() * (pSlackMax - pSlackMin);
				}

				tmp[i][j] = Random01() * vMax[i][j];
				v[m][i][j] = tmp[i][j];
				if (Random01() < 0.5)
				{
					v[m][i][j] = -tmp[i][j];
				}
				pBest[m][i][j] = x[m][i][j]; //初始化个体最好
			}
		}
	}

	//初始化fx, fpBest
	for (int m = 0; m < g; m++)
	{
		for (int i = 0; i < amountBattery + amountPV + amountWindM + 1; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				adj_p1[i][j] = x[m][i][j];
			}
		}

		//将x值赋给adjust_p;
		ad->Adjustment_p(adj_p1);
		adj_p9 = ad->pReturn;
		adj_vm = ad->adj_vm;
		adj_pBat = ad->pBat;
		adj_pPV = ad->pPV;
		adj_pWindM = ad->pWindM;
		adj_SOC = ad->SOC;
		best_SOC = adj_SOC;

		for (int i = 0; i < amountBattery + amountPV + amountWindM + 1; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				if (i < amountBattery)
				{
					x[m][i][j] = adj_pBat[i][j];
				}
				else if (i < amountBattery + amountPV)
				{
					x[m][i][j] = adj_pPV[i - amountBattery][j];
				}
				else if (i < amountBattery + amountPV + amountWindM)
				{
					x[m][i][j] = adj_pWindM[i - amountBattery - amountPV][j];
				}
				else
				{
					x[m][i][j] = adj_p9[numBus - 1][j];
				}
			}
		}

		//pv维护成本
		f1 = 0;
		for (int i = 0; i < amountPV; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				f1 = f1 + adj_pPV[i][j] * pn * 0.000708;
			}

		}

		//wind维护成本
		f2 = 0;
		for (int i = 0; i < amountWindM; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				f2 = f2 + adj_pWindM[i][j] * pn * 0.005992;
			}
		}

		//与外部电网交换电量
		f3 = 0;
		for (int j = 0; j < timeDivide; j++)
		{
			if (j <= 7)
			{
				if (adj_p9[numBus - 1][j] > 0)
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.17;
				}
				else
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.13;
				}
			}
			else if ((j >= 11 && j <= 15) || (j >= 19 && j <= 21))
			{
				if (adj_p9[numBus - 1][j] > 0)
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.83;
				}
				else
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.65;
				}
			}
			else
			{
				if (adj_p9[numBus - 1][j] > 0)
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.49;
				}
				else
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.38;
				}
			}

		}

		fx[m] = f1 + f2 + f3; //目标函数维护成本
		for (int i = 0; i < numBus; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				if (adj_vm[i][j] < 0.9 || adj_vm[i][j] > 1.1)
				{
					fx[m] = fx[m] + 100;
				}
			}
		}

		fpBest[m] = fx[m];
	}

	//初始化fgBest
	fgBest = fpBest[0];

	//总集合初始化
	total_gBest = new double**[_numProcs];
	for (int i = 0; i < _numProcs; i++)
	{
		total_gBest[i] = new double*[(amountBattery + amountPV + amountWindM + 1)];
	}
	total_gBest[0][0] = new double[(amountBattery + amountPV + amountWindM + 1) * timeDivide * total_g];
	memset(total_gBest[0][0], 0, sizeof(double) * _numProcs * (amountBattery + amountPV + amountWindM + 1) * timeDivide);
	for (int i = 0; i < _numProcs - 1; i++)
	{
		total_gBest[i + 1][0] = total_gBest[i][0] + (amountBattery + amountPV + amountWindM + 1) * timeDivide;
	}
	for (int i = 0; i < _numProcs; i++)
	{
		for (int j = 0; j < (amountBattery + amountPV + amountWindM + 1) - 1; j++)
		{
			total_gBest[i][j + 1] = total_gBest[i][j] + timeDivide;
		}
	}

	total_gBestMin = new double**[_numProcs];
	for (int i = 0; i < _numProcs; i++)
	{
		total_gBestMin[i] = new double*[(amountBattery + amountPV + amountWindM + 1)];
	}
	total_gBestMin[0][0] = new double[(amountBattery + amountPV + amountWindM + 1) * timeDivide * total_g];
	memset(total_gBestMin[0][0], 0, sizeof(double) * _numProcs * (amountBattery + amountPV + amountWindM + 1) * timeDivide);
	for (int i = 0; i < _numProcs - 1; i++)
	{
		total_gBestMin[i + 1][0] = total_gBestMin[i][0] + (amountBattery + amountPV + amountWindM + 1) * timeDivide;
	}
	for (int i = 0; i < _numProcs; i++)
	{
		for (int j = 0; j < (amountBattery + amountPV + amountWindM + 1) - 1; j++)
		{
			total_gBestMin[i][j + 1] = total_gBestMin[i][j] + timeDivide;
		}
	}

	compare_fpBestT = new double[_numProcs];
	memset(compare_fpBestT, 0, sizeof(double) * _numProcs);

	total_fpBestT = new double*[_numProcs];
	total_fpBestT[0] = new double[_numProcs * iter];
	memset(total_fpBestT[0], 0, sizeof(double) * _numProcs * iter);
	for (int i = 0; i < _numProcs - 1; i++)
	{
		total_fpBestT[i + 1] = total_fpBestT[i] + iter;
	}

	final_fpBestT = new double[iter];
	memset(final_fpBestT, 0, sizeof(double) * iter);

	scatterProc = 0;
}

void PSO::Cal_Adjustment(double *** _x, int _deltat)
{
	for (int m = 0; m < g; m++)
	{
		for (int i = 0; i < amountBattery + amountPV + amountWindM + 1; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				adj_p1[i][j] = _x[m][i][j];
			}
		}

		//将_x值赋给adjust_p
		ad->Adjustment_p(adj_p1);
		adj_p9 = ad->pReturn;
		adj_vm = ad->adj_vm;
		adj_pBat = ad->pBat;
		adj_pPV = ad->pPV;
		adj_pWindM = ad->pWindM;
		adj_SOC = ad->SOC;

		for (int i = 0; i < amountBattery + amountPV + amountWindM + 1; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				if (i < amountBattery)
				{
					_x[m][i][j] = adj_pBat[i][j];
				}
				else if (i < amountBattery + amountPV)
				{
					_x[m][i][j] = adj_pPV[i - amountBattery][j];
				}
				else if (i < amountBattery + amountPV + amountWindM)
				{
					_x[m][i][j] = adj_pWindM[i - amountBattery - amountPV][j];
				}
				else
				{
					_x[m][i][j] = adj_p9[numBus - 1][j];
				}
			}
		}

		//pv维护成本
		f1 = 0;
		for (int i = 0; i < amountPV; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				f1 = f1 + adj_pPV[i][j] * pn * 0.000708;
			}

		}

		//wind维护成本
		f2 = 0;
		for (int i = 0; i < amountWindM; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				f2 = f2 + adj_pWindM[i][j] * pn * 0.005992;
			}
		}

		//与外部电网交换电量
		f3 = 0;
		for (int j = 0; j < timeDivide; j++)
		{
			if (j <= 7)
			{
				if (adj_p9[numBus - 1][j] > 0)
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.17;
				}
				else
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.13;
				}
			}
			else if ((j >= 11 && j <= 15) || (j >= 19 && j <= 21))
			{
				if (adj_p9[numBus - 1][j] > 0)
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.83;
				}
				else
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.65;
				}
			}
			else
			{
				if (adj_p9[numBus - 1][j] > 0)
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.49;
				}
				else
				{
					f3 = f3 + adj_p9[numBus - 1][j] * pn * 0.38;
				}
			}

		}

		fx[m] = f1 + f2 + f3;
		for (int i = 0; i < numBus; i++)
		{
			for (int j = 0; j < timeDivide; j++)
			{
				if (adj_vm[i][j] < 0.9 || adj_vm[i][j] > 1.1)
				{
					fx[m] = fx[m] + 100;
				}
			}
		}

		if (fx[m] < fpBest[m])
		{
			for (int i = 0; i < amountBattery + amountPV + amountWindM + 1; i++)
			{
				for (int j = 0; j < timeDivide; j++)
				{
					pBest[m][i][j] = _x[m][i][j]; //pBest更新
				}
			}

			best_SOC = adj_SOC;

			fpBest[m] = fx[m]; //fpBest更新
		}

		if (fpBest[m] < fgBest)
		{
			for (int i = 0; i < amountBattery + amountPV + amountWindM + 1; i++)
			{
				for (int j = 0; j < timeDivide; j++)
				{
					gBest[i][j] = pBest[m][i][j]; //gBest更新
				}
			}

			fgBest = fpBest[m]; //fgBest更新
		}

	}

	fpBestT[_deltat] = fgBest;
}

void PSO::Cal_PSO()
{
	//mpi计算
	rank = 0; //进程号
	root = 0; //根进程
	total_g = 0; //总粒子数
	numProcs = 0; //总进程数
	int nameLength = 0; //机器名长度
	startTime = 0;
	endTime = 0;
	costTime = 0;
	char processorName[MPI_MAX_PROCESSOR_NAME];

	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //得到当前进程号
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs); //得到总的进程数
	MPI_Get_processor_name(processorName, &nameLength); //得到机器名
	srand((unsigned)time(NULL) + rank); //取当前机器时间为随机种子, 加上进程号是为了各个进程取得随机数不同

	//把数据进行发散
	MPI_Reduce(&g, &total_g, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD); //计算总粒子数
	MPI_Bcast(&total_g, 1, MPI_INT, root, MPI_COMM_WORLD);

	numProcs = total_g / g;

	if (rank == root)
	{
		startTime = MPI_Wtime();
	}

	//粒子群初始化
	Initial_PSO(numProcs);

	//更新位置
	for (int deltat = 0; deltat < iter; deltat++)
	{
		w = 0.9 - 0.5 * deltat / iter;
		for (int m = 0; m < g; m++)
		{
			for (int i = 0; i < amountBattery + amountPV + amountWindM + 1; i++)
			{
				for (int j = 0; j < timeDivide; j++)
				{
					v[m][i][j] = w * v[m][i][j] + c1 * Random01() * (pBest[m][i][j] - x[m][i][j]) + c2 * Random01() * (gBest[i][j] - x[m][i][j]);
					if (v[m][i][j] > vMax[i][j])
					{
						v[m][i][j] = vMax[i][j];
					}
					if (v[m][i][j] < -vMax[i][j])
					{
						v[m][i][j] = -vMax[i][j];
					}
					x[m][i][j] = x[m][i][j] + v[m][i][j];
				}
			}
		}

		//计算各粒子适应值
		Cal_Adjustment(x, deltat);

		//集合操作
		if (deltat != 0 && (deltat % (iter / gathernum) == 0 || deltat == iter - 1))
		{
			if (rank == root)
			{
				std::cout << "Gather begin..." << std::endl;
			}

			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Gather(gBest[0], (amountBattery + amountPV + amountWindM + 1) * timeDivide, MPI_DOUBLE, total_gBest[0][0], (amountBattery + amountPV + amountWindM + 1) * timeDivide, MPI_DOUBLE, root, MPI_COMM_WORLD);
			MPI_Gather(&fpBestT[deltat], 1, MPI_DOUBLE, compare_fpBestT, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);

			if (rank == root)
			{
				scatterProc = MinRank(compare_fpBestT, numProcs);

				for (int i = 0; i < numProcs; i++)
				{
					for (int j = 0; j < (amountBattery + amountPV + amountWindM + 1); j++)
					{
						for (int m = 0; m < timeDivide; m++)
						{
							total_gBestMin[i][j][m] = total_gBest[scatterProc][j][m];
						}
					}
				}

			}

			MPI_Scatter(total_gBestMin[0][0], (amountBattery + amountPV + amountWindM + 1) * timeDivide, MPI_DOUBLE, gBest[0], (amountBattery + amountPV + amountWindM + 1) * timeDivide, MPI_DOUBLE, root, MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);

			if (rank == root)
			{
				std::cout << "Scatter end..." << std::endl;
			}
		}

		//集合各进程，计算最终收敛曲线
		if (deltat == iter - 1)
		{
			MPI_Gather(fpBestT, iter, MPI_DOUBLE, total_fpBestT[0], iter, MPI_DOUBLE, root, MPI_COMM_WORLD);

			for (int i = 0; i < iter; i++)
			{
				final_fpBestT[i] = total_fpBestT[0][i];
				for (int j = 1; j < numProcs; j++)
				{
					if (final_fpBestT[i] > total_fpBestT[j][i])
					{
						final_fpBestT[i] = total_fpBestT[j][i];
					}
				}
			}
		}

		if (rank == root)
		{
			std::cout << deltat << "\t";
		}

	}

	if (rank == root)
	{
		endTime = MPI_Wtime();
		costTime = endTime - startTime;
	}
}

PSO::PSO()
{
	this->Initial();
	this->Cal_PSO();

	if (rank == root)
	{
		this->WriteData(); //根进程才写数据
	}
}

void PSO::WriteData()
{
	//写入数据库
	MYSQL* conn;
	conn = mysql_init(NULL);

	if (!(mysql_real_connect(conn, host, user, pass, dbName, port, unixSocket, flag)))
	{
		fprintf(stderr, "\nError:%s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}

	//写入新数据
	char strWriHistory[300];
	snprintf(strWriHistory, sizeof(strWriHistory), "insert into history(algorithm_id, cost_time, iter, gather_num, g_max, process_num, date_time, result) values(%d, %f, %d, %d, %d, %d, FROM_UNIXTIME(%d), %f)", extern_algorithmID, costTime, iter, gathernum, g, numProcs, time(NULL), final_fpBestT[iter - 1]);
	mysql_query(conn, strWriHistory);

	int current_history_id = mysql_insert_id(conn);

	for (int i = 0; i < iter; i++)
	{
		char strWriResults[300];
		snprintf(strWriResults, sizeof(strWriResults),
			"insert into results(history_id, algorithm_id, n, f) values(%d, %d, %d, %f)", current_history_id, extern_algorithmID, i, final_fpBestT[i]);
		mysql_query(conn, strWriResults);
	}

	for (int i = 0; i < timeDivide; i++)
	{
		char strWriDistribution[300];
		snprintf(strWriDistribution, sizeof(strWriDistribution),
			"insert into power_distribution(history_id, algorithm_id, output_time, output_P_Battery, output_P_PV, output_P_WindM, output_P_Exchange, output_P_Total, SOC) values(%d, %d, %d, %f, %f, %f, %f, %f, %f)",
			current_history_id, extern_algorithmID, i, gBest[0][i], gBest[1][i], gBest[2][i], gBest[3][i], gBest[0][i] + gBest[1][i] + gBest[2][i] + gBest[3][i], best_SOC[0][i]);
		mysql_query(conn, strWriDistribution);
	}

	mysql_close(conn);
}

PSO::~PSO()
{
	if (fpBestT)
	{
		delete[] fpBestT;
		fpBestT = NULL;
	}

	if (ad)
	{
		delete ad;
		ad = NULL;
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

	if (pv)
	{
		delete pv;
		pv = NULL;
	}

	if (windm)
	{
		delete windm;
		windm = NULL;
	}

	if (bus)
	{
		delete bus;
		bus = NULL;
	}

	if (fx)
	{
		delete[] fx;
		fx = NULL;
	}

	if (fpBest)
	{
		delete[] fpBest;
		fpBest = NULL;
	}

	if (tmp[0])
	{
		delete[] tmp[0];
		tmp[0] = NULL;
	}
	if (tmp)
	{
		delete[] tmp;
		tmp = NULL;
	}

	if (vMax[0])
	{
		delete[] vMax[0];
		vMax[0] = NULL;
	}
	if (vMax)
	{
		delete[] vMax;
		vMax = NULL;
	}

	if (adj_p1[0])
	{
		delete[] adj_p1[0];
		adj_p1[0] = NULL;
	}
	if (adj_p1)
	{
		delete[] adj_p1;
		adj_p1 = NULL;
	}

	if (adj_pBat[0])
	{
		delete[] adj_pBat[0];
		adj_pBat[0] = NULL;
	}
	if (adj_pBat)
	{
		delete[] adj_pBat;
		adj_pBat = NULL;
	}

	if (adj_pPV[0])
	{
		delete[] adj_pPV[0];
		adj_pPV[0] = NULL;
	}
	if (adj_pPV)
	{
		delete[] adj_pPV;
		adj_pPV = NULL;
	}

	if (adj_pWindM[0])
	{
		delete[] adj_pWindM[0];
		adj_pWindM[0] = NULL;
	}
	if (adj_pWindM)
	{
		delete[] adj_pWindM;
		adj_pWindM = NULL;
	}

	if (adj_SOC[0])
	{
		delete[] adj_SOC[0];
		adj_SOC[0] = NULL;
	}
	if (adj_SOC)
	{
		delete[] adj_SOC;
		adj_SOC = NULL;
	}

	if (adj_p9[0])
	{
		delete[] adj_p9[0];
		adj_p9[0] = NULL;
	}
	if (adj_p9)
	{
		delete[] adj_p9;
		adj_p9 = NULL;
	}

	if (adj_vm[0])
	{
		delete[] adj_vm[0];
		adj_vm[0] = NULL;
	}
	if (adj_vm)
	{
		delete[] adj_vm;
		adj_vm = NULL;
	}

	if (best_SOC[0])
	{
		delete[] best_SOC[0];
		best_SOC[0] = NULL;
	}
	if (best_SOC)
	{
		delete[] best_SOC;
		best_SOC = NULL;
	}

	if (x[0][0])
	{
		delete[] x[0][0];
		x[0][0] = NULL;
	}
	for (int i = 0; i < (amountBattery + amountPV + amountWindM + 1); i++)
	{
		if (x[i])
		{
			delete[] x[i];
			x[i] = NULL;
		}
		if (x)
		{
			delete[] x;
			x = NULL;
		}
	}

	if (v[0][0])
	{
		delete[] v[0][0];
		v[0][0] = NULL;
	}
	for (int i = 0; i < (amountBattery + amountPV + amountWindM + 1); i++)
	{
		if (v[i])
		{
			delete[] v[i];
			v[i] = NULL;
		}
		if (v)
		{
			delete[] v;
			v = NULL;
		}
	}

	if (pBest[0][0])
	{
		delete[] pBest[0][0];
		pBest[0][0] = NULL;
	}
	for (int i = 0; i < (amountBattery + amountPV + amountWindM + 1); i++)
	{
		if (pBest[i])
		{
			delete[] pBest[i];
			pBest[i] = NULL;
		}
		if (pBest)
		{
			delete[] pBest;
			pBest = NULL;
		}
	}
}