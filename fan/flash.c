#include "flash.h"

void Flash_Unlock (void)
{
	if((FLASH_->CR &(1U<<31))!=0)
		{
			FLASH_->KEYR =0x45670123;
			FLASH_->KEYR =0xCDEF89AB;
		}	
}	

void Flash_Lock (void)
{
	FLASH_->CR |= (1U <<31);
}	

void Flash_EraseSector (uint32_t Sector)
{	
	Flash_Unlock();
	while( (FLASH_->SR&(1U<<16))!=0);
	FLASH_->CR |= (1U<<1)|( Sector <<3);
	FLASH_->CR |=(1U<<16);
	while( (FLASH_->SR&(1U<<16))!=0);
	
	FLASH_->CR &= ~(1U<<1);
	Flash_Lock();
}	
void Flash_Program_Word ( uint32_t Address, uint32_t Data)
{	
	Flash_Unlock();
	while ( (FLASH_->SR&(1U<<16))!=0);
	FLASH_->CR |=(1U<<0);
	FLASH_->CR |=(0x2U <<8);
	*(uint32_t*)Address =Data;
	while ( (FLASH_->SR&(1U<<16))!=0);
	FLASH_->CR &= ~(1U<<0);
	Flash_Lock();
}	

void Flash_Program_Half_Word ( uint32_t Address, uint16_t Data)
{
	Flash_Unlock();
	while ( (FLASH_->SR&(1U<<16))!=0);
	FLASH_->CR |=(1U<<0);
	FLASH_->CR |=(0x1U <<8);
	*( uint16_t*)Address =Data;
	while ( (FLASH_->SR&(1U<<16))!=0);
	FLASH_->CR &= ~(1U<<0);
	Flash_Lock();
}	
void Flash_Write_Array (uint32_t address,uint8_t *arr, uint16_t len)
{
	Flash_Unlock();
	uint16_t *pt = (uint16_t*)arr;
	for(uint16_t i=0 ;i<(len+1)/2;i++)
	{
		Flash_Program_Half_Word(address+2*i,*pt);
		pt++;
	}	
	Flash_Lock();
}	

void Flash_Read_Array (uint32_t address,uint8_t *arr, uint8_t len)
{
	
	for(uint16_t i =0;i<(len+1)/2;i++)
{
		*arr= *(volatile uint8_t*) (address+1*i);
	 arr++;
}	
	
	
}	

