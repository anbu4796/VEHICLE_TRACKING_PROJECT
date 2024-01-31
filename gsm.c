void GSM_Send_AT_Command(char* AT_CMD)
{
	HAL_UART_Transmit(&huart2,(uint8_t*)AT_CMD, strlen(AT_CMD),2000);
}


int GSM_Compare_GSMData_With(const char* string)
{
	char* ptr = NULL;
  /*Compare given string with GSM data*/
	ptr = strstr(RX_Buffer, string);
	/* if ptr = NULL, then no match found else match found*/
  if(ptr!=NULL)
		return 1;
	else
		return 0;
}


/*AT+NETOPEN Start Socket Service
AT+NETCLOSE Stop Socket Service
AT+CIPOPEN Establish Connection in Multi-Socket Mode
AT+CIPSEND Send data through TCP or UDP Connection
AT+CIPRXGET Set the Mode to Retrieve Data
AT+CIPCLOSE Close TCP or UDP Socket
AT+IPADDR Inquire Socket PDP address
AT+CIPHEAD Add an IP Header When Receiving Data
AT+CIPSRIP ShowRemoteIP Address and Port
AT+CIPMODE Set TCP/IP Application Mode
AT+CIPSENDMODE Set Sending Mode
AT+CIPTIMEOUT Set TCP/IP Timeout Value
AT+CIPCCFG Configure Parameters of Socket
AT+SERVERSTART Startup TCP Server
AT+SERVERSTOP Stop TCP Server
AT+CIPACK Query TCP Connection Data Transmitting Status
AT+CDNSGIP Query the IP Address of Given Domain Name*/

void CHECK_NETWORK(void)
{
    GSM_Send_AT_Command("AT+CSQ"); //+CSQ: 23,0     OK
    GSM_Send_AT_Command("AT+CREG?");  //+CREG: 0,1      OK
    GSM_Send_AT_Command("AT+CCREG?");   //+CGREG: 0,1       OK
}

void TCP_IP_CONFIGURE(void)
{
    GSM_Send_AT_Command("AT+CGDCONT=1,"IP","CMNET"");       //OK
}

void TCP_IP_ACTIVATE(void)
{
    GSM_Send_AT_Command("AT+NETOPEN");  //OK    +NETOPEN: 0 
    GSM_Send_AT_Command("AT+IPADDR");   //+IPADDR: 10.148.0.17      OK
}

void TCP_IP_DEACTIVATE(void)
{
    GSM_Send_AT_Command("AT+NETCLOSE"); //OK    
    GSM_Send_AT_Command("AT+IPADDR");  //+NETCLOSE: 0   +IPADDR: Network not opened ERROR
}

/*
AT+CIPMODE=1
OK
AT+NETOPEN
OK
+NETOPEN: 0
AT+SERVERSTART=8080, 0
OK
//only <server_index>=0 is allowed to operate with
transparent mode.
+CLIENT: 0,0,192.168.108.5:57202
CONNECT 115200
//only <link_num> 0 can be used for transparent
mode operation.
OK // sequence of +++ to quit data mode
AT+CIPCLOSE=0
OK
CLOSED

// close client connection
+CIPCLOSE: 0,0
AT+SERVERSTOP=0
+SERVERSTOP: 0,0
OK */
    
    
