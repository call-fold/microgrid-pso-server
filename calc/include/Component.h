#pragma once

#include <iostream>

class Bus //母线参数
{
public:
	Bus();
	~Bus();
	int sysNum; //系统编号
	int busIndex; //母线编号
	int busType; //母线类型
	double vm; //电压幅值
	double va; //电压相角
	double pn; //有功供应
	double qn; //无功供应
	double vMax; //电压上限
	double vMin; //电压下限
};

class Line //线路参数
{
public:
	Line();
	~Line();
	int sysNum; //系统编号
	int lineIndex; //线路编号
	int fBus; //上接母线编号
	int tBus; //下接母线编号
	double rBus; //母线电阻
	double xBus; //母线电抗
	double gGround; //对地电导
	double bGround; //对地电纳
	double ratioPropotion; //变压器变比
	double linePMax; //线路功率最大值
};

class Loads //负载参数
{
public:
	void Initial(int len);
	~Loads();
	int busIndex; //母线编号
	int loadNum; //负载编号
	int* loadTime; //负载时段
	double* pd; //有功负荷
	double* qd; //无功负荷
};

class Battery //蓄电池参数 （少了一个编号, 多了一个SOC）
{
public:
	void Initial(int len);
	~Battery();
	int busIndex; //母线编号
	int* batTime; //时间段
	double* batP; //蓄电池功率
	double* batPMax; //有功功率上限
	double* batPMin; //有功功率下限
	double* SOC;
};

class PV //太阳能电池
{
public:
	void Initial(int len);
	~PV();
	int busIndex; //母线编号
	int pvNum; //pv编号
	int* pvTime; //pv时间段
	double* pvP; //pv预测发电功率
	double* pvPMax; //pv有功功率上限
	double* pvPMin; //pv有功功率下限
};

class WindM //风机参数
{
public:
	void Initial(int len);
	~WindM();
	int busIndex; //母线编号
	int windmNum; //风机编号
	int* windmTime; //风机时间段
	double* windmP; //风机预测发电功率
	double* windmPMax; //风机有功功率上限
	double* windmPMin; //风机有功功率下限
};

class Algorithm //算法关键参数
{
public:
	Algorithm();
	~Algorithm();
	int iter; //粒子群迭代总次数
	int gatherNum; //集合操作次数
	int gmax; //粒子群总数
	double w; //惯性权重
	double c1; //加速常数1
	double c2; //加速常数2
};
