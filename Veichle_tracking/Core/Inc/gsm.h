/*
 * gsm.h
 *
 *  Created on: Feb 6, 2024
 *      Author: ram77
 */

#ifndef INC_GSM_H_
#define INC_GSM_H_






int GSM_SIM_INIT(void);
int DATA_SEND_READY(void);
int GSM_TCP_init(void);
int GSM_APN_SET(void);
int GSM_Send_Data(uint8_t * data);

#endif /* INC_GSM_H_ */
