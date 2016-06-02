#pragma once

#include <cstdlib>
#include "PowerFlow.h"

class Adjustment
{
public:
	inline double Random01(); // 0-1间的随机数
	void Adjustment_p(double** adjust_p);

	Adjustment();
	~Adjustment();

	PowerFlow* pf;

	Loads* loads;
	Battery* battery;
	PV* pv;
	WindM* windm;
	Bus* bus;

	double** p_re;
	double** adj_vm2;
	double** pReturn;
	double** pBat;
	double** pBatMax;
	double** pBatMin;
	double** pPV;
	double** pWindM;
	double** SOC;
	double** adj_vm;

	int numBattery;
	int numPV;
	int numWindM;
	int numBus;
	int amountBattery;
	int amountPV;
	int amountWindM;
	int amountLoads;
	int timeDivide;

	double pn;
	double pBatN; // 蓄电池额定功率
};

inline double Adjustment::Random01()
{
	return (rand() / double(RAND_MAX));
}
