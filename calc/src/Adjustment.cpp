#include "Adjustment.h"
#include <cstring>

Adjustment::Adjustment()
{
	pf = new PowerFlow();

	bus = pf->readData->bus;
	loads = pf->readData->loads;
	battery = pf->readData->battery;
	windm = pf->readData->windm;
	pv = pf->readData->pv;

	numBus = pf->readData->numBus;
	numBattery = pf->readData->numBattery;
	numPV = pf->readData->numPV;
	numWindM = pf->readData->numWindM;
	timeDivide = pf->readData->timeDivide;
	amountBattery = numBattery / timeDivide;
	amountPV = numPV / timeDivide;
	amountWindM = numWindM / timeDivide;

	pn = 100.0;
	pBatN = 100.0;

	p_re = new double*[numBus];
	p_re[0] = new double[numBus * timeDivide];
	memset(p_re[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		p_re[i + 1] = p_re[i] + timeDivide;
	}

	pReturn = new double*[numBus];
	pReturn[0] = new double[numBus * timeDivide];
	memset(pReturn[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		pReturn[i + 1] = pReturn[i] + timeDivide;
	}

	adj_vm = new double*[numBus];
	adj_vm[0] = new double[numBus * timeDivide];
	memset(adj_vm[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		adj_vm[i + 1] = adj_vm[i] + timeDivide;
	}

	adj_vm2 = new double*[numBus];
	adj_vm2[0] = new double[numBus * timeDivide];
	memset(adj_vm2[0], 0, sizeof(double) * numBus * timeDivide);
	for (int i = 0; i < numBus - 1; i++)
	{
		adj_vm2[i + 1] = adj_vm2[i] + timeDivide;
	}

	pBat = new double*[amountBattery];
	pBat[0] = new double[amountBattery * timeDivide];
	memset(pBat[0], 0, sizeof(double) * amountBattery * timeDivide);
	for (int i = 0; i < amountBattery - 1; i++)
	{
		pBat[i + 1] = pBat[i] + timeDivide;
	}

	pBatMax = new double*[amountBattery];
	pBatMax[0] = new double[amountBattery * timeDivide];
	memset(pBatMax[0], 0, sizeof(double) * amountBattery * timeDivide);
	for (int i = 0; i < amountBattery - 1; i++)
	{
		pBatMax[i + 1] = pBatMax[i] + timeDivide;
	}

	pBatMin = new double*[amountBattery];
	pBatMin[0] = new double[amountBattery * timeDivide];
	memset(pBatMin[0], 0, sizeof(double) * amountBattery * timeDivide);
	for (int i = 0; i < amountBattery - 1; i++)
	{
		pBatMin[i + 1] = pBatMin[i] + timeDivide;
	}

	SOC = new double*[amountBattery];
	SOC[0] = new double[amountBattery * (timeDivide + 1)];
	memset(SOC[0], 0, sizeof(double) * amountBattery * (timeDivide + 1));
	for (int i = 0; i < amountBattery - 1; i++)
	{
		SOC[i + 1] = SOC[i] + (timeDivide + 1);
	}

	pPV = new double*[amountPV];
	pPV[0] = new double[amountPV * timeDivide];
	memset(pPV[0], 0, sizeof(double) * amountPV * timeDivide);
	for (int i = 0; i < amountPV - 1; i++)
	{
		pPV[i + 1] = pPV[i] + timeDivide;
	}

	pWindM = new double*[amountWindM];
	pWindM[0] = new double[amountWindM * timeDivide];
	memset(pWindM[0], 0, sizeof(double) * amountWindM * timeDivide);
	for (int i = 0; i < amountWindM - 1; i++)
	{
		pWindM[i + 1] = pWindM[i] + timeDivide;
	}
}

void Adjustment::Adjustment_p(double** adjust_p)
{
	for (int i = 0; i < amountBattery; i++)
	{
		SOC[i][0] = 0.5;
	}
	for (int i = 0; i < amountBattery + amountPV + amountWindM; i++)
	{
		if (i < amountBattery)
		{
			for (int t = 0; t < 24; t++)
			{
				pBat[i][t] = adjust_p[i][t];
			}
		}
		else if (i < amountBattery + amountPV)
		{
			for (int t = 0; t < 24; t++)
			{
				pPV[i - amountBattery][t] = adjust_p[i][t];
			}
		}
		else
		{
			for (int t = 0; t < 24; t++)
			{
				pWindM[i - amountBattery - amountPV][t] = adjust_p[i][t];
			}
		}

	}

	for (int t = 0; t < 24; t++)
	{


		for (int i = 0; i < amountBattery; i++)
		{
			pBatMax[i][t] = (SOC[i][t] - 0.3) * pBatN / pn;
			if (pBatMax[i][t] > 1)
			{
				pBatMax[i][t] = 1;
			}

			pBatMin[i][t] = (SOC[i][t] - 1) * pBatN / pn;
			if (pBatMin[i][t] < -1)
			{
				pBatMin[i][t] = -1;
			}
		}

		//计算pbat范围
		for (int c = 0; c < 100; c++)
		{

			pf->PowerFlow_Calculation_YL(t, pPV, pWindM, pBat); //调用函数
			p_re = pf->p;
			adj_vm2 = pf->vm;

			for (int i = 0; i < numBus; i++)
			{
				pReturn[i][t] = p_re[i][t];
				adj_vm[i][t] = adj_vm2[i][t];
			}
			int times10 = 0, times11 = 0;
			for (int i = 0; i < amountPV; i++)
			{
				if (pPV[i][t] == pv[i].pvPMax[t] / pn)
				{
					times10++;
				}
				if (pPV[i][t] == pv[i].pvPMin[t] / pn)
				{
					times11++;
				}
			}
			for (int i = 0; i < amountWindM; i++)
			{
				if (pWindM[i][t] == windm[i].windmPMax[t] / pn)
				{
					times10++;
				}
				if (pWindM[i][t] == windm[i].windmPMin[t] / pn)
				{
					times11++;
				}
			}
			for (int i = 0; i < amountBattery; i++)
			{
				if (pBat[i][t] == pBatMax[i][t])
				{
					times10++;
				}
				if (pBat[i][t] == pBatMin[i][t])
				{
					times11++;
				}

			}
			if ((times10 == (amountBattery + amountPV + amountWindM)) || (times11 == (amountBattery + amountPV + amountWindM)))
			{
				break;
			}


			if (pReturn[numBus - 1][t] >= 0.3) //0.3可调整
			{
				double delta1, delta2;
				delta1 = pReturn[numBus - 1][t];
				delta2 = Random01() * delta1;

				for (int i = 0; i < amountPV; i++)
				{
					pPV[i][t] = pPV[i][t] + delta2;
					if (pPV[i][t] > pv[i].pvPMax[t] / pn)
					{
						pPV[i][t] = pv[i].pvPMax[t] / pn;
					}
					if (pPV[i][t] < pv[i].pvPMin[t] / pn)
					{
						pPV[i][t] = pv[i].pvPMin[t] / pn;
					}
				}
				for (int i = 0; i < amountWindM; i++)
				{
					pWindM[i][t] = pWindM[i][t] + (delta1 - delta2);
					if (pWindM[i][t] > windm[i].windmPMax[t] / pn)
					{
						pWindM[i][t] = windm[i].windmPMax[t] / pn;
					}
					if (pWindM[i][t] < windm[i].windmPMin[t] / pn)
					{
						pWindM[i][t] = windm[i].windmPMin[t] / pn;
					}
				}
				int times2 = 0;
				for (int i = 0; i < amountPV; i++)
				{

					if (pPV[i][t] == pv[i].pvPMax[t] / pn)
					{
						times2++;
					}
				}
				for (int i = 0; i < amountWindM; i++)
				{

					if (pWindM[i][t] == windm[i].windmPMax[t] / pn)
					{
						times2++;
					}

				}
				if (times2 == (amountPV + amountWindM))
				{

					for (int i = 0; i < amountBattery; i++)
					{

						pBat[i][t] = pBat[i][t] + delta1;
						if (pBat[i][t] > pBatMax[i][t])
						{
							pBat[i][t] = pBatMax[i][t];
						}
						if (pBat[i][t] < pBatMin[i][t])
						{
							pBat[i][t] = pBatMin[i][t];
						}

					}
				}
			}

			else if (pReturn[numBus - 1][t] <= -3.5)
			{
				double delta1, delta2;
				delta1 = pReturn[numBus - 1][t] + 3.5;
				delta2 = Random01() * delta1;

				for (int i = 0; i < amountBattery; i++)
				{
					pBat[i][t] = pBat[i][t] + delta1;
					if (pBat[i][t] > pBatMax[i][t])
					{
						pBat[i][t] = pBatMax[i][t];
					}
					if (pBat[i][t] < pBatMin[i][t])
					{
						pBat[i][t] = pBatMin[i][t];
					}

				}
				int times3 = 0;
				for (int i = 0; i < amountBattery; i++)
				{


					if (pBat[i][t] == pBatMin[i][t])
					{
						times3++;
					}

				}
				if (times3 == amountBattery)
				{

					for (int i = 0; i < amountPV; i++)
					{
						pPV[i][t] = pPV[i][t] + delta2;
						if (pPV[i][t] > pv[i].pvPMax[t] / pn)
						{
							pPV[i][t] = pv[i].pvPMax[t] / pn;
						}
						if (pPV[i][t] < pv[i].pvPMin[t] / pn)
						{
							pPV[i][t] = pv[i].pvPMin[t] / pn;
						}
					}
					for (int i = 0; i < amountWindM; i++)
					{
						pWindM[i][t] = pWindM[i][t] + delta1 - delta2;
						if (pWindM[i][t] > windm[i].windmPMax[t] / pn)
						{
							pWindM[i][t] = windm[i].windmPMax[t] / pn;
						}
						if (pWindM[i][t] < windm[i].windmPMin[t] / pn)
						{
							pWindM[i][t] = windm[i].windmPMin[t] / pn;
						}
					}
				}
			}
			else
			{
				int times = 0;
				for (int i = 0; i < amountPV; i++)
				{

					if ((pPV[i][t] <= pv[i].pvPMax[t] / pn) && (pPV[i][t] >= pv[i].pvPMin[t] / pn))
					{
						times++;
					}

				}
				for (int i = 0; i < amountWindM; i++)
				{

					if ((pWindM[i][t] <= windm[i].windmPMax[t] / pn) && (pWindM[i][t] >= windm[i].windmPMin[t] / pn))
					{
						times++;
					}

				}
				for (int i = 0; i < amountBattery; i++)
				{
					if ((pBat[i][t] <= pBatMax[i][t]) && (pBat[i][t] >= pBatMin[i][t]))
					{
						times++;
					}
				}
				if (times == (amountPV + amountWindM + amountBattery))
				{
					break;
				}

				//检验约束
				for (int i = 0; i < amountPV; i++)
				{
					if (pPV[i][t] > pv[i].pvPMax[t] / pn)
					{
						pPV[i][t] = pv[i].pvPMax[t] / pn;
					}
					if (pPV[i][t] < pv[i].pvPMin[t] / pn)
					{
						pPV[i][t] = pv[i].pvPMin[t] / pn;
					}
				}
				for (int i = 0; i < amountWindM; i++)
				{
					if (pWindM[i][t] > windm[i].windmPMax[t] / pn)
					{
						pWindM[i][t] = windm[i].windmPMax[t] / pn;
					}
					if (pWindM[i][t] < windm[i].windmPMin[t] / pn)
					{
						pWindM[i][t] = windm[i].windmPMin[t] / pn;
					}
				}
				for (int i = 0; i < amountBattery; i++)
				{
					if (pBat[i][t] > pBatMax[i][t])
					{
						pBat[i][t] = pBatMax[i][t];
					}
					if (pBat[i][t] < pBatMin[i][t])
					{
						pBat[i][t] = pBatMin[i][t];
					}
				}
			}
		}

		for (int i = 0; i < amountBattery; i++)
		{
			SOC[i][t + 1] = SOC[i][t] - pBat[i][t];
		}
	}
}

Adjustment::~Adjustment()
{
	if (pf)
	{
		delete pf;
		pf = NULL;
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

	if (p_re[0])
	{
		delete[] p_re[0];
		p_re[0] = NULL;
	}
	if (p_re)
	{
		delete[] p_re;
		p_re = NULL;
	}

	if (adj_vm2[0])
	{
		delete[] adj_vm2[0];
		adj_vm2[0] = NULL;
	}
	if (adj_vm2)
	{
		delete[] adj_vm2;
		adj_vm2 = NULL;
	}

	if (pReturn[0])
	{
		delete[] pReturn[0];
		pReturn[0] = NULL;
	}
	if (pReturn)
	{
		delete[] pReturn;
		pReturn = NULL;
	}

	if (pBat[0])
	{
		delete[] pBat[0];
		pBat[0] = NULL;
	}
	if (pBat)
	{
		delete[] pBat;
		pBat = NULL;
	}

	if (pBatMax[0])
	{
		delete[] pBatMax[0];
		pBatMax[0] = NULL;
	}
	if (pBatMax)
	{
		delete[] pBatMax;
		pBatMax = NULL;
	}

	if (pBatMin[0])
	{
		delete[] pBatMin[0];
		pBatMin[0] = NULL;
	}
	if (pBatMin)
	{
		delete[] pBatMin;
		pBatMin = NULL;
	}

	if (pPV[0])
	{
		delete[] pPV[0];
		pPV[0] = NULL;
	}
	if (pPV)
	{
		delete[] pPV;
		pPV = NULL;
	}

	if (pWindM[0])
	{
		delete[] pWindM[0];
		pWindM[0] = NULL;
	}
	if (pWindM)
	{
		delete[] pWindM;
		pWindM = NULL;
	}

	if (SOC[0])
	{
		delete[] SOC[0];
		SOC[0] = NULL;
	}
	if (SOC)
	{
		delete[] SOC;
		SOC = NULL;
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
}