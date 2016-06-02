// Microgrid_PSO_Server_slf.cpp : 定义控制台应用程序的入口点。
//

#include "mpi.h"
#include <iostream>
#include "Cal_PSO.h"
#include <cstring>
#include <ctime>

extern char* extern_host;
extern char* extern_user;
extern char* extern_password;
extern char* extern_dbName;
extern unsigned int extern_port;
extern unsigned int extern_algorithmID;

int AddFactorial(int n)
{
	if (1 == n)
	{
		return 1;
	}
	else
	{
		int num_old = 1;
		int num = 2;
		int mul = num_old * num;
		int sum = num_old + mul;
		for (int i = 2; i < n; i++)
		{
			num_old = num;
			num++;
			mul *= num;
			sum += mul;
		}
		return sum;
	}
}

int main(int argc, char **argv)
{
	extern_host = argv[1];
	extern_user = argv[2];
	extern_password = argv[3];
	extern_dbName = argv[4];
	extern_port = atoi(argv[5]);
	extern_algorithmID = atoi(argv[6]);

	//extern_host = "192.168.1.127";
	//extern_user = "root";
	//extern_password = "gpops";
	//extern_dbName = "eco_dis1";
	//extern_port = 3306;
	//extern_algorithmID = 1;

	MPI_Init(&argc, &argv);/*程序初始化*/

	PSO* pso = new PSO();

	MPI_Finalize();

    return 0;
}
