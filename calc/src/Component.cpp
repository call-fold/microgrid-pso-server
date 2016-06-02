#include <iostream>
#include "Component.h"
#include <cstring>

Bus::Bus()
{
	sysNum = 0;
	busIndex = 0;
	busType = 0;
	vm = 0;
	va = 0;
	pn = 0;
	qn = 0;
	vMax = 0;
	vMin = 0;
}

Bus::~Bus()
{
	sysNum = NULL;
	busIndex = NULL;
	busType = NULL;
	vm = NULL;
	va = NULL;
	pn = NULL;
	qn = NULL;
	vMax = NULL;
	vMin = NULL;
}

Line::Line()
{
	sysNum = 0;
	lineIndex = 0;
	fBus = 0;
	tBus = 0;
	rBus = 0;
	xBus = 0;
	gGround = 0;
	bGround = 0;
	ratioPropotion = 0;
	linePMax = 0;
}

Line::~Line()
{
	sysNum = NULL;
	lineIndex = NULL;
	fBus = NULL;
	tBus = NULL;
	rBus = NULL;
	xBus = NULL;
	gGround = NULL;
	bGround = NULL;
	ratioPropotion = NULL;
	linePMax = NULL;
}

void Loads::Initial(int len)
{
	busIndex = 0;
	loadNum = 0;
	loadTime = new int[len];
	pd = new double[len];
	qd = new double[len];
	memset(loadTime, 0, sizeof(int) * len);
	memset(pd, 0, sizeof(double) * len);
	memset(qd, 0, sizeof(double) * len);
}

Loads::~Loads()
{
	busIndex = NULL;
	loadNum = NULL;
	if (loadTime)
	{
		delete[] loadTime;
		loadTime = NULL;
	}
	if (pd)
	{
		delete[] pd;
		pd = NULL;
	}
	if (qd)
	{
		delete[] qd;
		qd = NULL;
	}
}

void Battery::Initial(int len)
{
	busIndex = 0;
	batTime = new int[len];
	batP = new double[len];
	batPMax = new double[len];
	batPMin = new double[len];
	SOC = new double[len];
	memset(batTime, 0, sizeof(int) * len);
	memset(batP, 0, sizeof(double) * len);
	memset(batPMax, 0, sizeof(double) * len);
	memset(batPMin, 0, sizeof(double) * len);
	memset(SOC, 0, sizeof(double) * len);
}

Battery::~Battery()
{
	busIndex = NULL;
	if (batTime)
	{
		delete[] batTime;
		batTime = NULL;
	}
	if (batP)
	{
		delete[] batP;
		batP = NULL;
	}
	if (batPMax)
	{
		delete[] batPMax;
		batPMax = NULL;
	}
	if (batPMin)
	{
		delete[] batPMin;
		batPMin = NULL;
	}
	if (SOC)
	{
		delete[] SOC;
		SOC = NULL;
	}
}

void PV::Initial(int len)
{
	busIndex = 0;
	pvNum = 0;
	pvTime = new int[len];
	pvP = new double[len];
	pvPMax = new double[len];
	pvPMin = new double[len];
	memset(pvTime, 0, sizeof(int) * len);
	memset(pvP, 0, sizeof(double) * len);
	memset(pvPMax, 0, sizeof(double) * len);
	memset(pvPMin, 0, sizeof(double) * len);
}

PV::~PV()
{
	busIndex = NULL;
	pvNum = NULL;
	if (pvTime)
	{
		delete[] pvTime;
		pvTime = NULL;
	}
	if (pvP)
	{
		delete[] pvP;
		pvP = NULL;
	}
	if (pvPMax)
	{
		delete[] pvPMax;
		pvPMax = NULL;
	}
	if (pvPMin)
	{
		delete[] pvPMin;
		pvPMin = NULL;
	}
}

void WindM::Initial(int len)
{
	busIndex = 0;
	windmNum = 0;
	windmTime = new int[len];
	windmP = new double[len];
	windmPMax = new double[len];
	windmPMin = new double[len];
	memset(windmTime, 0, sizeof(int) * len);
	memset(windmP, 0, sizeof(double) * len);
	memset(windmPMax, 0, sizeof(double) * len);
	memset(windmPMin, 0, sizeof(double) * len);
}

WindM::~WindM()
{
	busIndex = NULL;
	windmNum = NULL;
	if (windmTime)
	{
		delete[] windmTime;
		windmNum = NULL;
	}
	if (windmP)
	{
		delete[] windmP;
		windmP = NULL;
	}
	if (windmPMax)
	{
		delete[] windmPMax;
		windmPMax = NULL;
	}
	if (windmPMin)
	{
		delete[] windmPMin;
		windmPMin = NULL;
	}
}

Algorithm::Algorithm()
{
	iter = 0;
	gatherNum = 0;
	gmax = 0;
	w = 0;
	c1 = 0;
	c2 = 0;
}

Algorithm::~Algorithm()
{
	iter = NULL;
	gatherNum = NULL;
	gmax = NULL;
	w = NULL;
	c1 = NULL;
	c2 = NULL;
}
