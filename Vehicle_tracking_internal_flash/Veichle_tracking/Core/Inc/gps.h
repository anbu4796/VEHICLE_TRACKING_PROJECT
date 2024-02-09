/*
 * gps.h
 *
 *  Created on: Feb 7, 2024
 *      Author: ram77
 */

#ifndef INC_GPS_H_
#define INC_GPS_H_




void extract_data_for_GGA(char *);
void json_format_for_GGA(char *);
void json_format_for_RMC(char *);
void extract_data_for_RMC(char *);
void get_data_from_gps(char *);


#endif /* INC_GPS_H_ */
