#pragma once

#include "Component.h"

class ReadData
{
public:
	//连接信息
	ReadData(char* _host, char* _user, char* _pass, char* _dbName, unsigned int _port, char* _unixSocket, unsigned int _flag, char* _sysIndex, char* _opfPlanIndex);
	~ReadData();

	char* host;
	char* user;
	char* pass;
	char* dbName;
	unsigned int port;
	char* unixSocket;
	unsigned int flag;
	char* sysIndex;
	char* opfPlanIndex;

	int numBus;
	int numLine;
	int numLoads;
	int numBattery;
	int numPV;
	int numWindM;
	int numPq;
	int numPv;
	int numSlack;
	int timeDivide;

	Bus* bus;
	Line* line;
	Loads* loads;
	Battery* battery;
	WindM* windm;
	PV* pv;
	Algorithm algorithm;

	void ReadData_Bus();
	void ReadData_Line();
	void ReadData_Loads();
	void ReadData_Battery();
	void ReadData_PV();
	void ReadData_WindM();
	void ReadData_Algorithm(unsigned int _algorithm_id);
};

