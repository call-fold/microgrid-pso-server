#pragma once

#include "Adjustment.h"

class PSO
{
public:
	inline double Random01();
	void Initial();
	void Initial_PSO(int _numProcs);
	void Cal_Adjustment(double*** _x, int _deltat);
	void Cal_PSO();

	void WriteData();

	PSO();
	~PSO();

	double* fpBestT; 

	Adjustment* ad;

	Loads* loads;
	Battery* battery;
	PV* pv;
	WindM* windm;
	Bus* bus;

	//数据库
	char* host;
	char* user;
	char* pass;
	char* dbName;
	unsigned int port;
	char* unixSocket;
	unsigned int flag;

	int iter;
	int g; //粒子数
	double w;
	double c1;
	double c2;
	double pn;
	double pSlackMin;
	double pSlackMax;
	double f1;
	double f2;
	double f3;

	int numBattery;
	int numPV;
	int numWindM;
	int numBus;
	int amountBattery;
	int amountPV;
	int amountWindM;
	int timeDivide;

	double* fx;
	double* fpBest;

	double** tmp;
	double** vMax;
	double** adj_p1;
	double** adj_pBat;
	double** adj_pPV;
	double** adj_pWindM;
	double** adj_SOC;
	double** adj_p9;
	double** adj_vm;

	double*** x;
	double*** v;
	double*** pBest;
	double** gBest;
	double fgBest;
	double** best_SOC;

	int rank; //进程号
	int root; //根进程
	int gathernum; //集合迭代次数
	double*** total_x;
	double*** total_v;
	double*** total_pBest;
	int total_g; //总粒子数
	double*** total_gBest;
	double* compare_fpBestT;
	int scatterProc; //需要分散的进程编号
	inline int MinRank(double* _compare_fpBestT, int _compare_fpBestT_len);
	int numProcs; //进程数

	double startTime;
	double endTime;
	double costTime;

	double*** total_gBestMin;
	double** total_fpBestT;
	double* final_fpBestT;
};

inline double PSO::Random01()
{
	return (rand() / double(RAND_MAX));
}

inline int PSO::MinRank(double* _compare_fpBestT, int _compare_fpBestT_len)
{
	int id_min = 0;
	double temp = _compare_fpBestT[0];
	for (int i = 1; i < _compare_fpBestT_len; i++)
	{
		if (temp > _compare_fpBestT[i])
		{
			temp = _compare_fpBestT[i];
			id_min = i;
		}
	}

	return id_min;
}
