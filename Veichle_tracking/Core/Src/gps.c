/*
 * gps.c
 *
 *  Created on: Feb 7, 2024
 *      Author: ram77
 */
/* including header files */

#include "main.h"
#include "gps.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* defining buffer sizes */

#define JSON_SIZE 300
#define NMEA_SIZE 750

/* Declaration of private variables */

char RxBuffer[NMEA_SIZE],buf_to_extract[NMEA_SIZE],json_str[JSON_SIZE];
char time[9],date[9],latitude_dir,longitude_dir;
int lat_deg,gps_fix,long_deg;
double lat_min,long_min,gga_latitude,gga_longitude,rmc_latitude, rmc_longitude;


/* Handler Declarations */

extern UART_HandleTypeDef huart1;

/* *************************************************************************************************
 * 																				                   *
 * Receiving the GPS data in NMEA formats and checking for the required sentences                  *
 * i.e, GNGGA, GNRMC sentences from the buffer (UART_receive_data), storing into                   *
 * separate buffers,checking the valid data if data is valid calling extract and JSON functions    *
 * *********************************************************************************************** */
void get_data_from_gps(char *UART_receive_data)
{
	char *ptrgga=NULL, *ptrrmc=NULL;
	char GGA_buffer[250],RMC_buffer[200];
	int rmc_length,gga_length;

	if( (ptrgga=strstr(UART_receive_data,"GNGGA")))
	{

		strncpy(GGA_buffer,ptrgga,80);    //copying received GNGGA data into GGA_buffer
		printf("GGA string is %s\n", GGA_buffer);
		gga_length=strlen(GGA_buffer);

		if(GGA_buffer!=NULL)              //checking weather GGA_buffer empty or not
	  	{
			for(int i=0;i<gga_length;i++)
			{

				if(GGA_buffer[i]=='M')
				{
					extract_data_for_GGA(GGA_buffer);    //calling extract function

					if(gps_fix==1)                       //Checking for received GGA data is valid or not
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
		 strncpy(RMC_buffer,ptrrmc,85);
		 printf("RMC string is %s\n", RMC_buffer);
	 	 rmc_length=strlen(RMC_buffer);
	 	 if(RMC_buffer!=NULL)
	 	 {
	 		 if(rmc_length>=84)
	 		 {
               char *ptr=NULL;
	 		   if((ptr=strchr(RMC_buffer,'*')))
	 		   {
              		extract_data_for_RMC(RMC_buffer);
              		json_format_for_RMC(json_str);
	 		   }
	 		 }
	 		 else
	 		 {
	 			 printf("Invalid RMC not getting proper location\n Reset the GPS Module\n");
	 		 }
	 	  }
	  	  else
	  	  {
	  	  	  printf("rmc buffer is empty\n");
	  	  }
	}
	memset(GGA_buffer,0,sizeof(GGA_buffer));   //Clearing the buffers
	memset(RMC_buffer,0,sizeof(RMC_buffer));
}

/**************************************************************************************
 * Extracting the required fields from GGA_buffer i.e time,fix,latitude and longitude *
 * ***********************************************************************************/

void extract_data_for_GGA( char *GGA_buffer)
{

		int comma_count = 0,utc_hours,utc_minutes,utc_seconds,ist_hours,ist_minutes,ist_seconds;
		char utc_time[7],latitude_str[10], longitude_str[11],gpsfix[2];
		char *token = strtok(GGA_buffer, ",");           //Extracting substrings from GGA sentences
	    while (token != NULL && comma_count < 9)         //checking extracted substrings NULL or not
	    {
		    switch (comma_count)
	        {
		    	case 1:
		    		    // Copying Time value from token to utc_time
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
						//Adjusting the hours,minutes and seconds to proper time stands
						if(ist_minutes==0)
						{
							 ist_hours+=1;
						}
						if(ist_hours>24)
						{
							ist_hours-=24;
						}
						if(ist_seconds>=60)
						{
							ist_minutes+=1;
							ist_seconds=60-ist_seconds;
						}
						// Print the result in the format "hrs:mins:secs"
						sprintf(time, "%02d:%02d:%02d", ist_hours, ist_minutes, ist_seconds);
						break;
		    	case 2:
		    		    //Copying latitude string from token to latitude_str
						strncpy(latitude_str, token, 9);
						latitude_str[9] = '\0';
						//Copying the degrees and minutes values of latitude into lat_deg,lat_min
						sscanf(latitude_str,"%2d%lf",&lat_deg,&lat_min);
						//Converting the latitude into proper location formation
						gga_latitude =lat_deg+lat_min/60.0;
						break;
		    	case 3:
		    		    // latitude direction
						latitude_dir=token[0];
						break;
			    case 4:
			    	    // Copying Longitude value from token to longitude string
						strncpy(longitude_str, token, 10);
						longitude_str[10] = '\0';
						//Copying the degrees and minutes values of latitude into long_deg,long_min
						sscanf(longitude_str,"%3d%lf",&long_deg,&long_min);
						//Converting the longitude into proper location
						gga_longitude=long_deg+long_min/60.0;
						break;
			    case 5:
			    	    // longitude dir
						longitude_dir=token[0];
						//printf("longidir=%c\n",longitude_dir);
						break;
			    case 6:
			    	    // gps fix
						strncpy(gpsfix,token,1);
						gpsfix[1]='\0';
						//converting the fix value i.e string to integer
						gps_fix =atoi(gpsfix);
						break;
		   }
		token = strtok(NULL, ",");
		comma_count++;
	}
}

/* Creating the JSON string */

void json_format_for_GGA(char *json_str)
{

	// Creating the JSON string
	sprintf(json_str, "$%s,%lf%c,%lf%c#\n", time, gga_latitude, latitude_dir, gga_longitude,longitude_dir);
//	printf("JSON string format=%s\n",json_str);
}

/**************************************************************************************
 * Extracting the required fields from RMC_buffer i.e time,fix,latitude and longitude *
 * ***********************************************************************************/

void extract_data_for_RMC(char *RMC_buffer)
{
		int comma_count = 0,dd,mm,yy;
		int utc_hours,utc_minutes,utc_seconds,ist_hours,ist_minutes,ist_seconds;
		char utc_time[7], latitude_str[10], longitude_str[11],Date_str[7];
		printf("rmc buffer in function = %s\n",RMC_buffer);

		char *token = strtok(RMC_buffer, ",");

		while (token != NULL && comma_count < 12)
		{
			switch (comma_count)
			{
				case 1: // Time
						 strncpy(utc_time, token, 6);
						 utc_time[6] = '\0';
						 // Extract hours, minutes, and seconds
						 utc_hours = atoi(utc_time) / 10000;
						 utc_minutes = (atoi(utc_time) % 10000) / 100;
						 utc_seconds = atoi(utc_time) % 100;

						 // Calculate IST (Indian Standard Time) with a time zone offset of 5 hours and 30 minutes
						 ist_hours = (utc_hours + 5) % 24 ;
						 ist_minutes = (utc_minutes + 30) % 60;
						 ist_seconds = utc_seconds;
						 //Adjusting the hours,minutes and seconds to proper time stands
						 if(ist_minutes==0)
						 {
						 	 ist_hours+=1;
						 }
						 if(ist_hours>24)
						 {
						 	ist_hours-=24;
						 }
						 if(ist_seconds>=60)
						 {
						 	ist_minutes+=1;
						 	ist_seconds=60-ist_seconds;
						 }
						 // Print the result in the format "hrs:mins:secs"
						 sprintf(time,"%02d:%02d:%02d", ist_hours, ist_minutes, ist_seconds);
						 break;
				case 3:
					 	// Copying Longitude value from token to latitude string
						strncpy(latitude_str, token, 9);
						latitude_str[9] = '\0';
						//Copying the degrees and minutes values of latitude into lat_deg,lat_min
						sscanf(latitude_str,"%2d%lf",&lat_deg,&lat_min);
						//Converting the latitude into proper location formation
						rmc_latitude =lat_deg+lat_min/60.0;
						break;
				case 4:
					    // latitude direction
						latitude_dir=token[0];
						break;
				case 5:
					    // Copying Longitude value from token to longitude string
						strncpy(longitude_str, token, 10);
						longitude_str[10] = '\0';
						//Copying the degrees and minutes values of latitude into long_deg,long_min
						sscanf(longitude_str,"%3d%lf",&long_deg,&long_min);
						//Converting the longitude into proper location
						rmc_longitude=long_deg+long_min/60.0;
						break;
				case 6:
						// longitude direction
						longitude_dir=token[0];
						break;
				case 9: // Date
						strncpy(Date_str, token, 7);
						Date_str[6] = '\0';
						// converting the date into dd/mm/yy format
						dd = atoi(Date_str) / 10000;
						mm = (atoi(Date_str) % 10000) / 100;
						yy = atoi(Date_str) % 100;
						// Print the result in the format "dd/mm/yy"
						sprintf(date,"%02d/%02d/%02d", dd, mm, yy);
						break;

			}
			token = strtok(NULL, ",");
			comma_count++;
		}
}
/* Creating the JSON string */

void json_format_for_RMC(char *json_str)
{

	sprintf(json_str, "$%s,%s,%lf%c,%lf%c#\n", date, time, rmc_latitude,latitude_dir, rmc_longitude,longitude_dir);
	//printf("JSON String:\n%s\n", json_str);
}




