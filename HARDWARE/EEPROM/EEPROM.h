#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "sys.h"

void EEPROM_Init(void);
u8 EEPROM_Read(u16 addr);
void EEPROM_Write(u16 addr,u8 SentData);
void EEPROM_WriteFloat(u16 addr,float SentData);
float EEPROM_ReadFloat(u16 addr);

#endif

