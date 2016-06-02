#include <cstdio>
#include <cstdlib>
#include "soapmgopfserverService.h"
#include "mgopfserver.nsmap"

#define MAX_SIZE 1024

int main(int argc, char* argv[])
{
    mgopfserverService mgopfserver;
    if (argc < 2)
    {
        mgopfserver.serve();
    }
    else
    {
        int port = atoi(argv[1]);
        if (!port)
        {
            fprintf(stderr, "Usage: mgopfserver++ <port>\n");
            exit(0);
        }

        if (mgopfserver.run(port))
        {
            mgopfserver.soap_stream_fault(std::cerr);
            exit(-1);
        }
    }

    return 0;
}

int mgopfserverService::mgopfserver(char* processNum, char* host, char* user, char* password, char* dbName, char* port, char* algorithmID, int* status)
{
	char result_buf[MAX_SIZE], command[MAX_SIZE];
	FILE *fp;
	int rc=0;
	/*将要执行的命令写入buf*/
	sprintf(command,"mpiexec -n %s -f ../../calc/bin/hosts ../../calc/bin/calc %s %s %s %s %s %s", processNum, host, user, password, dbName, port, algorithmID);

	/*执行预先设定的命令，并读出该命令的标准输出*/
	fp=popen(command,"r");
	if(fp==NULL)
	{
		perror("popen failed！");
		exit(-1);
	}
	while(fgets(result_buf,sizeof(result_buf),fp) != NULL)
	{
		/*为了输出好看，把命令返回的换行符去掉*/
		if('\n' == result_buf[strlen(result_buf)-1])
        	{
            		result_buf[strlen(result_buf)-1] = '\0';
        	}
        	fprintf(stdout,"command[%s] output[%s]\r\n", command, result_buf);
		fflush(stdout);
		*status = 1;
	}
	rc = pclose(fp);
    	if(rc==-1)
    	{
        	perror("close file point failed");
        	exit(-1);
    	}
    	else
    	{
        	fprintf(stdout,"command[%s]rank status[%d]return[%d]\r\n", command, rc, WEXITSTATUS(rc));
		fflush(stdout);
		return SOAP_OK;
    	}
	
}
