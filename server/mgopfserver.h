#pragma once
//gsoap ns service name:	mgopfserver
//gsoap ns service protocol:	SOAP
//gsoap ns service style:	rpc
//gsoap ns service encoding:	encoded
//gsoap ns service namespace:	http://210.34.23.37:9983/mgopfserver.wsdl
//gsoap ns service location:	http://210.34.23.37:9983/mgopfserver.cgi

//gsoap ns schema namespace:	urn:mgopfserver

//gsoap ns service method-documentation: mgopfserver
int ns__mgopfserver(char* processNum, char* host, char* user, char* password, char* dbName, char* port, char* algorithmID, int* status);
