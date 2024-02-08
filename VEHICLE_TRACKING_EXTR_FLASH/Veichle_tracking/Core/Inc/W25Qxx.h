/*
 * W25Qxx.h
 *
 *  Created on: Jan 25, 2024
 *      Author: SIDDARTH
 */

#ifndef INC_W25QXX_H_
#define INC_W25QXX_H_


//#define W25_JEDEC_ID		0x9F
//#define W25_R_SFPD_REG		0x5A

//void W25Q32_Read(uint32_t address, uint8_t *data, uint32_t size);



void W25Q_Reset (void);
uint32_t W25Q_ReadID (void);
void W25Q_Read (uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData);
void W25Q_FastRead (uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData);
void W25Q_Erase_Sector (uint16_t numsector);
void W25Q_Write_Clean (uint32_t page, uint16_t offset, uint32_t size, uint8_t *data);
void W25Q_Write (uint32_t page, uint16_t offset, uint32_t size, uint8_t *data);

void W25Q_Write_NUM (uint32_t page, uint16_t offset, float data);
float W25Q_Read_NUM (uint32_t page, uint16_t offset);

void W25Q_Write_32B (uint32_t page, uint16_t offset, uint32_t size, uint32_t *data);
void W25Q_Read_32B (uint32_t page, uint16_t offset, uint32_t size, uint32_t *data);
int  Storing_GPS_Data_Into_Flash(uint8_t * );



#endif /* INC_W25QXX_H_ */

