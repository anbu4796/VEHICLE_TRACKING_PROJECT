#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void extract_data(char *str, int *time, float *latitude, float *longitude)
{
	int tokencnt = 0;
	char utc_time[7], latitude_str[10], longitude_str[11];
	char *token = strtok(str, ",");
	while (token != NULL && tokencnt < 7)
	{
		switch (tokencnt)
		{
			case 1: // Time
				strncpy(utc_time, token, 6);
				utc_time[6] = '\0';
				*time = atoi(utc_time);
				break;
			case 2: // Latitude
				strncpy(latitude_str, token, 9);
				latitude_str[9] = '\0';
				*latitude = atof(latitude_str);
				break;
			case 4: // Longitude
				strncpy(longitude_str, token, 10);
				longitude_str[10] = '\0';
				*longitude = atof(longitude_str);
				break;
		}
		token = strtok(NULL, ",");
		tokencnt++;
	}
}

void json_format(char *json_str, int time, float latitude, float longitude)
{
	// Creating the JSON string
	sprintf(json_str, "{\n  \"Time\": %d,\n  \"Latitude\": %0.3f,\n  \"Longitude\": %0.3f\n}\n", time, latitude, longitude);
}


void get_datafrom_gps(void)
{
	char gps_nmea[80],token[5]="GPGGA";
	char *ptr=NULL;
	float latitude, longitude;
	int time;
	char json_str[256]; // Adjust the size as needed
	__fpurge(stdin);


	printf("Enter the NMEA format data:\n");
	scanf("%[^\n]s", gps_nmea);


	// Check if the string contains "GPGGA"
	ptr=strstr(gps_nmea,"GPGGA");
	printf("ptr=%s\n",ptr);
	if(ptr!=NULL)
	{
		extract_data(gps_nmea, &time, &latitude, &longitude);

		// Creating a variable to store the JSON string


		// Generating the JSON string
		json_format(json_str, time, latitude, longitude);

		// Printing the JSON string
		printf("JSON String:\n%s\n", json_str);
		memset(gps_nmea,0,sizeof(gps_nmea));
	}
	else
	{
		printf("Invalid NMEA format\n");
		memset(gps_nmea,0,sizeof(gps_nmea));
	}
	//memset(gps_nmea,0,sizeof(gps_nmea));
	__fpurge(stdin);  // Clear the input buffer after scanf
}

int main()
{
	int choice;


	while(1)
	{
		printf("1. get_datafrom_gps\n2.exit\n");
		scanf("%d",&choice);
		switch(choice)
		{
			case 1: get_datafrom_gps();
				break;
			case 2:exit(0);
		}
	}
}
/*void get_datafrom_gps(void)
  {

  printf("Enter the NMEA format data:\n");
  scanf("%[^\n]s", gps_nmea);
// Convert the input string to uppercase
//to_uppercase(gps_nmea);
if(gps_nmea=="exit")
{
break;
}
else
{

// Check if the string contains "GPGGA"
ptr=strstr(gps_nmea,"GPGGA");
printf("ptr=%s\n",ptr);
if(ptr!=NULL)
{
get_data(gps_nmea, &time, &latitude, &longitude);

// Creating a variable to store the JSON string


// Generating the JSON string
json_format(json_str, time, latitude, longitude);

// Printing the JSON string
printf("JSON String:\n%s\n", json_str);
memset(gps_nmea,0,sizeof(gps_nmea));
}
else
{
printf("Invalid NMEA format\n");
memset(gps_nmea,0,sizeof(gps_nmea));
}
//memset(gps_nmea,0,sizeof(gps_nmea));
__fpurge(stdin);  // Clear the input buffer after scanf
}


}
*/