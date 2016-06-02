//#include <winsock2.h>
#include "mysql/mysql.h"
#include "ReadData.h"
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <cstdio>

//string类型转为double或int函数
template <class T>
void convertFromString(T& value, const std::string& s)
{
	std::stringstream ss(s);
	ss >> value;
}

ReadData::ReadData(char* _host, char* _user, char* _pass, char* _dbName, unsigned int _port, char* _unixSocket, unsigned int _flag, char* _sysIndex, char* _opfPlanIndex)
{
	this->host = _host;
	this->user = _user;
	this->pass = _pass;
	this->dbName = _dbName;
	this->port = _port;
	this->unixSocket = _unixSocket;
	this->flag = _flag;
	this->sysIndex = _sysIndex;
	this->opfPlanIndex = _opfPlanIndex;
	timeDivide = 24;
}

ReadData::~ReadData()
{
	this->host = NULL;
	this->user = NULL;
	this->pass = NULL;
	this->dbName = NULL;
	this->port = NULL;
	this->unixSocket = NULL;
	this->flag = NULL;
	this->sysIndex = NULL;
	this->opfPlanIndex = NULL;

	if (bus)
	{
		delete[] bus;
		bus = NULL;
	}
	if (loads)
	{
		delete[] loads;
		loads = NULL;
	}
	if (line)
	{
		delete[] line;
		line = NULL;
	}
	if (battery)
	{
		delete[] battery;
		battery = NULL;
	}
	if (windm)
	{
		delete[] windm;
		windm = NULL;
	}
	if (pv)
	{
		delete[] pv;
		pv = NULL;
	}
}

void ReadData::ReadData_Bus()
{
	char queryStr[80] = { "select * from bus" };
	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW row;
	conn = mysql_init(NULL);
	if (!(mysql_real_connect(conn, host, user, pass, dbName, port, unixSocket, flag)))
	{
		fprintf(stderr, "\nError:%s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	mysql_query(conn, queryStr);
	res = mysql_store_result(conn);

	numBus = mysql_num_rows(res);
	bus = new Bus[numBus];
	for (int i = 0; i < numBus; i++)
	{
		row = mysql_fetch_row(res);
		int x = 0;
		double y = 0;
		convertFromString(x, row[0]);
		bus[i].busIndex = x;
		convertFromString(x, row[1]);
		bus[i].sysNum = x;
		convertFromString(x, row[2]);
		bus[i].busType = x;
		convertFromString(y, row[3]);
		bus[i].vm = y;
		convertFromString(y, row[4]);
		bus[i].va = y;
		convertFromString(y, row[5]);
		bus[i].pn = y;
		convertFromString(y, row[6]);
		bus[i].qn = y;
		convertFromString(y, row[7]);
		bus[i].vMax = y;
		convertFromString(y, row[8]);
		bus[i].vMin = y;
	}

	numPq = 0;
	for (int i = 0; i < numBus; i++)
	{
		//1表示pq节点，2表示pv节点，3表示平衡节点
		if (1 == bus[i].busType)
		{
			numPq++;
		}
	}

	numPv = 0;
	for (int i = 0; i < numBus; i++)
	{
		if (2 == bus[i].busType)
		{
			numPv++;
		}
	}

	numSlack = 0;
	for (int i = 0; i < numBus; i++)
	{
		if (3 == bus[i].busType)
		{
			numSlack++;
		}
	}

	mysql_free_result(res);
	mysql_close(conn);
}

void ReadData::ReadData_Line()
{
	char queryStr[80] = { "select * from line" };
	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW row;
	conn = mysql_init(NULL);
	if (!(mysql_real_connect(conn, host, user, pass, dbName, port, unixSocket, flag)))
	{
		fprintf(stderr, "\nError:%s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	mysql_query(conn, queryStr);
	res = mysql_store_result(conn);

	numLine = mysql_num_rows(res);
	line = new Line[numLine];
	for (int i = 0; i < numLine; i++)
	{
		row = mysql_fetch_row(res);
		int x = 0;
		double y = 0;
		convertFromString(x, row[0]);
		line[i].lineIndex = x;
		convertFromString(x, row[1]);
		line[i].sysNum = x;
		convertFromString(x, row[2]);
		line[i].fBus = x;
		convertFromString(x, row[3]);
		line[i].tBus = x;
		convertFromString(y, row[4]);
		line[i].rBus = y;
		convertFromString(y, row[5]);
		line[i].xBus = y;
		convertFromString(y, row[6]);
		line[i].gGround = y;
		convertFromString(y, row[7]);
		line[i].bGround = y;
		convertFromString(y, row[8]);
		line[i].ratioPropotion = y;
		convertFromString(y, row[9]);
		line[i].linePMax = y;
	}

	mysql_free_result(res);
	mysql_close(conn);
}

void ReadData::ReadData_Loads()
{
	char queryStr[80] = { "select * from loads" };
	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW row;
	conn = mysql_init(NULL);
	if (!(mysql_real_connect(conn, host, user, pass, dbName, port, unixSocket, flag)))
	{
		fprintf(stderr, "\nError:%s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	mysql_query(conn, queryStr);
	res = mysql_store_result(conn);

	numLoads = mysql_num_rows(res);
	int num = numLoads / timeDivide;
	loads = new Loads[num];
	for (int i = 0; i < num; i++)
	{
		loads[i].Initial(timeDivide);
		row = mysql_fetch_row(res); //取结果集中的下一行
		int x = 0;
		double y = 0;
		convertFromString(x, row[0]);
		loads[i].busIndex = x;
		convertFromString(x, row[1]);
		loads[i].loadTime[0] = x;
		convertFromString(y, row[2]);
		loads[i].pd[0] = y;
		convertFromString(y, row[3]);
		loads[i].qd[0] = y;
		for (int j = 1; j < timeDivide; j++)
		{
			row = mysql_fetch_row(res);
			convertFromString(x, row[1]);
			loads[i].loadTime[j] = x;
			convertFromString(y, row[2]);
			loads[i].pd[j] = y;
			convertFromString(y, row[3]);
			loads[i].qd[j] = y;
		}
	}

	mysql_free_result(res);
	mysql_close(conn);
}

void ReadData::ReadData_Battery()
{
	char queryStr[80] = { "select * from battery" };
	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW row;
	conn = mysql_init(NULL);
	if (!(mysql_real_connect(conn, host, user, pass, dbName, port, unixSocket, flag)))
	{
		fprintf(stderr, "\nError:%s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	mysql_query(conn, queryStr);
	res = mysql_store_result(conn);

	numBattery = mysql_num_rows(res);
	int num = numBattery / timeDivide;
	battery = new Battery[num];
	for (int i = 0; i < num; i++)
	{
		battery[i].Initial(timeDivide);
		row = mysql_fetch_row(res); //取结果集中的下一行
		int x = 0;
		double y = 0;
		convertFromString(x, row[1]);
		battery[i].busIndex = x;
		convertFromString(x, row[0]);
		battery[i].batTime[0] = x;
		convertFromString(y, row[2]);
		battery[i].batPMax[0] = y;
		convertFromString(y, row[3]);
		battery[i].batPMin[0] = y;
		convertFromString(y, row[4]);
		battery[i].batPMin[0] = y;
		for (int j = 1; j < timeDivide; j++)
		{
			row = mysql_fetch_row(res);
			convertFromString(x, row[0]);
			battery[i].batTime[j] = x;
			convertFromString(y, row[2]);
			battery[i].batPMax[j] = y;
			convertFromString(y, row[3]);
			battery[i].batPMin[j] = y;
			convertFromString(y, row[4]);
			battery[i].batPMin[j] = y;
		}
	}

	mysql_free_result(res);
	mysql_close(conn);
}

void ReadData::ReadData_PV()
{
	char queryStr[80] = { "select * from pv" };
	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW row;
	conn = mysql_init(NULL);
	if (!(mysql_real_connect(conn, host, user, pass, dbName, port, unixSocket, flag)))
	{
		fprintf(stderr, "\nError:%s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	mysql_query(conn, queryStr);
	res = mysql_store_result(conn);

	numPV = mysql_num_rows(res);
	int num = numPV / timeDivide;
	pv = new PV[num];
	for (int i = 0; i < num; i++)
	{
		pv[i].Initial(timeDivide);
		row = mysql_fetch_row(res); //取结果集中的下一行
		int x = 0;
		double y = 0;
		convertFromString(x, row[1]);
		pv[i].busIndex = x;
		//convertFromString(x, row[0]);
		//pv[i].pvNum = x;
		convertFromString(x, row[2]);
		pv[i].pvTime[0] = x;
		convertFromString(y, row[3]);
		pv[i].pvP[0] = y;
		convertFromString(y, row[4]);
		pv[i].pvPMax[0] = y;
		convertFromString(y, row[5]);
		pv[i].pvPMin[0] = y;
		for (int j = 1; j < timeDivide; j++)
		{
			row = mysql_fetch_row(res);
			convertFromString(x, row[2]);
			pv[i].pvTime[j] = x;
			convertFromString(y, row[3]);
			pv[i].pvP[j] = y;
			convertFromString(y, row[4]);
			pv[i].pvPMax[j] = y;
			convertFromString(y, row[5]);
			pv[i].pvPMin[j] = y;
		}
	}

	mysql_free_result(res);
	mysql_close(conn);
}

void ReadData::ReadData_WindM()
{
	char queryStr[80] = { "select * from windm" };
	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW row;
	conn = mysql_init(NULL);
	if (!(mysql_real_connect(conn, host, user, pass, dbName, port, unixSocket, flag)))
	{
		fprintf(stderr, "\nError:%s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	mysql_query(conn, queryStr);
	res = mysql_store_result(conn);

	numWindM = mysql_num_rows(res);
	int num = numWindM / timeDivide;
	windm = new WindM[num];
	for (int i = 0; i < num; i++)
	{
		windm[i].Initial(timeDivide);
		row = mysql_fetch_row(res); //取结果集中的下一行
		int x = 0;
		double y = 0;
		convertFromString(x, row[1]);
		windm[i].busIndex = x;
		//convertFromString(x, row[0]);
		//windm[i].windmNum = x;
		convertFromString(x, row[2]);
		windm[i].windmTime[0] = x;
		convertFromString(y, row[3]);
		windm[i].windmP[0] = y;
		convertFromString(y, row[4]);
		windm[i].windmPMax[0] = y;
		convertFromString(y, row[5]);
		windm[i].windmPMin[0] = y;
		for (int j = 1; j < timeDivide; j++)
		{
			row = mysql_fetch_row(res);
			convertFromString(x, row[2]);
			windm[i].windmTime[j] = x;
			convertFromString(y, row[3]);
			windm[i].windmP[j] = y;
			convertFromString(y, row[4]);
			windm[i].windmPMax[j] = y;
			convertFromString(y, row[5]);
			windm[i].windmPMin[j] = y;
		}
	}

	mysql_free_result(res);
	mysql_close(conn);
}

void ReadData::ReadData_Algorithm(unsigned int _algorithm_id)
{
	char char_algorithm_id[10];
	sprintf(char_algorithm_id, "%d", _algorithm_id);
	char queryStr[100] = { "select * from algorithm where algorithm_id = "};
	strcat(queryStr, char_algorithm_id);
	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW row;
	conn = mysql_init(NULL);
	if (!(mysql_real_connect(conn, host, user, pass, dbName, port, unixSocket, flag)))
	{
		fprintf(stderr, "\nError:%s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	mysql_query(conn, queryStr);
	res = mysql_store_result(conn);

	row = mysql_fetch_row(res); //取结果集中的下一行
	int x = 0;
	double y = 0;
	convertFromString(x, row[1]);
	algorithm.iter = x;
	convertFromString(x, row[2]);
	algorithm.gatherNum = x;
	convertFromString(x, row[3]);
	algorithm.gmax = x;
	convertFromString(y, row[4]);
	algorithm.w = y;
	convertFromString(y, row[5]);
	algorithm.c1 = y;
	convertFromString(y, row[6]);
	algorithm.c2 = y;

	mysql_free_result(res);
	mysql_close(conn);
}
