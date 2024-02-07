/*
 * gps.c
 *
 *  Created on: Feb 7, 2024
 *      Author: ram77
 */
#include "main.h"
#include "gps.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define JSON_SIZE 300
#define NMEA_SIZE 750
//GPS Variables
char RxBuffer[NMEA_SIZE];
char buf_to_extract[NMEA_SIZE];
char json_str[JSON_SIZE]; // Adjust the size as needed



extern UART_HandleTypeDef huart1;

char time[9];
char date[9];
char latitude_dir,longitude_dir;

float gga_latitude, gga_longitude;
float rmc_latitude, rmc_longitude;
int gps_fix;


void get_data_from_gps(char *UART_receive_data)
{
	char *ptrgga=NULL, *ptrrmc=NULL;
	char GGA_buffer[250],RMC_buffer[200];
	int index,gga_length,rmc_length;
	//printf("received data from UART %s\n",UART_receive_data);

	if( (ptrgga=strstr(UART_receive_data,"GNGGA")))
	{
		printf("ptrGGA string has %s\n", ptrgga);
		strncpy(GGA_buffer,ptrgga,80);
		printf("ptrGGA 2 string has %s\n", ptrgga);
		printf("GGA string is %s\n", GGA_buffer);
		gga_length=strlen(GGA_buffer);
		if(GGA_buffer!=NULL)
	  	{
			for(int i=0;i<gga_length;i++)
			{

				if(GGA_buffer[i]=='M')
				{
					extract_data_for_GGA(GGA_buffer);
					// Generating the JSON string
					if(gps_fix==1)
					{
						printf("GPS Valid Fix\n");
						json_format_for_GGA(json_str);
					}
					else
					{
						printf("Invalid GPS fix not getting proper location\n Reset the GPS Module\n");
					}
				}

			}
	  	}
		else
	  	{
	  	 	printf("GGA Buffer is empty\n");
	  	}
	}
	if((ptrrmc=strstr(UART_receive_data,"GNRMC")))
	{
	  	/* if(*ptrrmc=='G')
	  	 {
	  	 	 index=0;
	  	 	 while(*(ptrrmc+index)!='*')
	  	 	 {
					RMC_buffer[index]=*(ptrrmc+index);
	  	 			index++;
	  	 	 }
	  	 	 RMC_buffer[index]='\0';
	  	 }*/
		strncpy(RMC_buffer,ptrrmc,85);
		printf("RMC string is %s\n", RMC_buffer);
	 	 rmc_length=strlen(RMC_buffer);
	 	 if(RMC_buffer!=NULL)
	 	 {

	 		 char *ptr=NULL;
	 		 if((ptr=strchr(RMC_buffer,'*')))
	 		 {

	 			extract_data_for_RMC(RMC_buffer);
	 		 }
	 		if(date!='0')
	 		{
	 			 json_format_for_RMC(json_str);
	 		}

	 	 }

	  	  else
	  	  {
	  	  	  printf("rmc buffer is empty\n");
	  	  }
	}



	memset(GGA_buffer,0,sizeof(GGA_buffer));
	memset(RMC_buffer,0,sizeof(RMC_buffer));
	//memset(buf_to_extract,0,sizeof(buf_to_extract));
}

void extract_data_for_GGA( char *GGA_buffer)
{

		int tokencnt = 0,minutes,degrees,wholeSeconds;
		double lat,longi,seconds,temp,fractionalSeconds;
		int utc_hours,utc_minutes,utc_seconds,ist_hours,ist_minutes,ist_seconds;
		//printf("in gga fun %s\n",GGA_buffer);
		char utc_time[7];
		char latitude_str[10], longitude_str[11],gpsfix[2],latidir;
		char *token = strtok(GGA_buffer, ",");
	while (token != NULL && tokencnt < 9)
	{
		switch (tokencnt)
		{

			case 1: // Time
						strncpy(utc_time, token, 6);
						utc_time[6] = '\0';
						// Extract hours, minutes, and seconds
						utc_hours = atoi(utc_time) / 10000;
						utc_minutes = (atoi(utc_time) % 10000) / 100;
						utc_seconds = atoi(utc_time) % 100;

						// Calculate IST (Indian Standard Time) with a time zone offset of 5 hours and 30 minutes
						ist_hours = (utc_hours + 5) % 24;
						ist_minutes = (utc_minutes + 30) % 60;
						ist_seconds = utc_seconds;

						// Print the result in the format "hrs:mins:secs"
						sprintf(time, "%02d:%02d:%02d", ist_hours, ist_minutes, ist_seconds);

						// Print the IST time
						printf("IST time: %s", time);
						break;
			case 2: // Latitude
				strncpy(latitude_str, token, 9);
				latitude_str[9] = '\0';
				gga_latitude = atof(latitude_str);
				//printf("latitude=%f\n",latitude);
				break;
			case 3: // latitude dir
				latitude_dir=token[0];
				//printf("latidir=%c\n",latitude_dir);
				break;
			case 4: // Longitude
				strncpy(longitude_str, token, 10);
				longitude_str[10] = '\0';
				gga_longitude = atof(longitude_str);
				//printf("longitude=%f\n",longitude);
				break;
			case 5: // longitude dir
				longitude_dir=token[0];
				//printf("longidir=%c\n",longitude_dir);
				break;
			case 6: // gps fix
				strncpy(gpsfix,token,1);
				gpsfix[1]='\0';
				gps_fix =atoi(gpsfix);
				//printf("gpsfix=%d\n",gps_fix);
				break;
		}
		token = strtok(NULL, ",");
		tokencnt++;
	}
}


void json_format_for_GGA(char *json_str)
{

	// Creating the JSON string
	sprintf(json_str, "{\n Time : %s,\n Latitude : %f %c,\n Longitude : %f %c\n}\n", time, gga_latitude, latitude_dir, gga_longitude,longitude_dir);

	printf("JSON string format=%s\n",json_str);
}


void extract_data_for_RMC(char *RMC_buffer)
{
		int tokencnt = 0,minutes,degrees,dd,mm,yy;
		int utc_hours,utc_minutes,utc_seconds,ist_hours,ist_minutes,ist_seconds;
		double lat,longi,temp,seconds;
		char utc_time[7], latitude_str[10], longitude_str[11],Date_str[7];
		printf("rmc buffer in function = %s\n",RMC_buffer);
		char *token = strtok(RMC_buffer, ",");
		printf("token in function = %s\n",token);
	while (token != NULL && tokencnt < 12)
	{
		printf("token count =%d\n",tokencnt);
		switch (tokencnt)
		{
			case 1: // Time
						strncpy(utc_time, token, 6);
						utc_time[6] = '\0';
						//time = atoi(utc_time);
						// Extract hours, minutes, and seconds
						 utc_hours = atoi(utc_time) / 10000;
						 utc_minutes = (atoi(utc_time) % 10000) / 100;
						 utc_seconds = atoi(utc_time) % 100;

						 // Calculate IST (Indian Standard Time) with a time zone offset of 5 hours and 30 minutes
						 ist_hours = (utc_hours + 5) % 24 ;
						 ist_minutes = (utc_minutes + 30) % 60;
						 ist_seconds = utc_seconds;

						 // Print the result in the format "hrs:mins:secs"
						 sprintf(time,"%02d:%02d:%02d", ist_hours, ist_minutes, ist_seconds);

						 // Print the IST time
						 printf("IST time: %s", time);
						 break;
			case 3: // Latitude
				strncpy(latitude_str, token, 9);
				latitude_str[9] = '\0';
				rmc_latitude = atof(latitude_str);
				break;
			case 4: // latitude dir
				latitude_dir=token[0];
				printf("longidir=%c\n",latitude_dir);
				break;
			case 5: // Longitude
				strncpy(longitude_str, token, 10);
				longitude_str[10] = '\0';
				rmc_longitude = atof(longitude_str);
				printf("longitude in RMC =%f\n",rmc_longitude);
				break;
			case 6: // longitude dir
				longitude_dir=token[0];
				printf("longidir=%c\n",longitude_dir);
				break;
			case 9: // Date
				strncpy(Date_str, token, 7);
			    // printf("date= %s\n",Date_str);
				Date_str[6] = '\0';
				//date = atoi(Date_str);
				dd = atoi(Date_str) / 10000;
				mm = (atoi(Date_str) % 10000) / 100;
				yy = atoi(Date_str) % 100;
				sprintf(date,"%02d/%02d/%02d", dd, mm, yy);
				printf("date=%s\n",date);
				break;

		}
		token = strtok(NULL, ",");
		tokencnt++;
	}
}

void json_format_for_RMC(char *json_str)
{
	// Creating the JSON string
		//sprintf(json_str, "{\n\"Date\":%s,\n\"Time\": %s,\n\"Latitude\": %f %c,\n\"Longitude\": %f %c\n}\n", date, time, rmc_latitude, latitude_dir, rmc_longitude ,longitude_dir);

	sprintf(json_str, "{\n  Date :%s,\n  Time : %s,\n  Latitude : %0.3f %c,\n  Longitude : %0.3f %c\n}\n", date, time, rmc_latitude,latitude_dir, rmc_longitude,longitude_dir);
	printf("JSON String:\n%s\n", json_str);
}




