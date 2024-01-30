/*
 * gsm.h
 *
 *  Created on: Jan 29, 2024
 *      Author: ram77
 */

#ifndef INC_GSM_H_
#define INC_GSM_H_



#define RX_BUFFER_SIZE             128
/* Header consists of GSM prototypes*/

//GSM initialization

void gsm_init(void);

// GSM sening
void gsm_send(char *);

//GSM Recieve
char * gsm_recv(void);

//GSM send AT cmd

void gsm_send_AT_cmd(char *);

//GSM TCP init

void gsm_TCP_init(void);

//GSM TCP Send

void gsm_TCP_send(char *);



#endif /* INC_GSM_H_ */
