#ifndef FLASH_H_
	#define FLASH_H_
	#include <stdint.h>
	
	typedef struct {
				volatile uint32_t ACR; // offset 0x00
				volatile uint32_t KEYR;// offset 0x04
				volatile uint32_t OPTKEYR;//offset 0x08
				volatile uint32_t SR;//0x0C
				volatile uint32_t CR;//0x10
				volatile uint32_t OPTCR;//	0x14
		
		
	}	Flash_Types;
	
	
#define FLASH_BASEADDR 0x40023C00
#define FLASH_ ((Flash_Types*)FLASH_BASEADDR)


void Flash_Unlock (void);
void Flash_Lock (void);
void Flash_EraseSector (uint32_t Sector);
void Flash_Program_Word ( uint32_t Address, uint32_t Data);
void Flash_Program_Half_Word ( uint32_t Address, uint16_t Data);	
void Flash_Write_Array (uint32_t address,uint8_t *arr, uint16_t len);
void Flash_Read_Array (uint32_t address,uint8_t *arr, uint8_t len);
#endif