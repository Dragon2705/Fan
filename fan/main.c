#include "RccConfig.h"

#include "delay.h"

#include "uart.h"

#include "adc.h"

#include "timer_pwm.h"

#include "stdio.h"

#include "flash.h"


#define FAN_VALUE_0 0;
#define FAN_VALUE_1 4;
#define FAN_VALUE_2 9;
#define FlashAdd 0x8060000



typedef struct __attribute__ ((__packed__)) {
volatile uint8_t val ;
volatile uint8_t flag ;//ram
} Che_do_quat;

Che_do_quat che_do_quat;
Che_do_quat che_do_quat_ban_dau;
Che_do_quat *p_che_do_quat;

void Flash_Write_Choose (Che_do_quat che_do_quat );
void Flash_Read_Choose (Che_do_quat *che_do_quat );

volatile uint8_t mode = 0;

volatile uint8_t RxBuffer[5];
volatile uint16_t valfan,valadc;
volatile uint8_t ByteInRxBuffer = 0;
volatile uint8_t Update_Data_To_Flash =0;
//char bufMode[16];
//char bufflag[16];
//char bufvalfan[16];
//char bufADC[16];
volatile  uint16_t ADC_VAL[2] = {0,0};
int main() {

        SysClockConfig();
        TIM5Config();
        TIM4_Config();
        Uart2Config();
        ADC_Init();
        ADC_Enable();
				DMA_Init();
				DMA_Config((uint32_t)&ADC1->DR,(uint32_t)ADC_VAL,2);
				ADC_Start();
	
				//	----------- cho nay minh nap 1 lan roi cmd no lai---------------
	
// 				che_do_quat.val=FAN_VALUE_2;
//				che_do_quat.flag=1;
//				
//				Flash_EraseSector(7);
//				Flash_Write_Choose(che_do_quat);
				

				

				
				
				Flash_Read_Choose(&che_do_quat);
				Flash_Read_Choose(&che_do_quat_ban_dau);
//	
        while (1) {
//                ADC_Start(1);
//                ADC_WaitForConv();
//                ADC_VAL[0] = ADC_GetVal();

                //tu dong
					if(mode==1){
                if (che_do_quat.flag == 0) {
                        if (ADC_VAL[0] >= 3000) {
                                TIM4 -> CCR1 = FAN_VALUE_2;
																che_do_quat.val = TIM4 -> CCR1;
                        }
                        if (ADC_VAL[0] < 3000 && ADC_VAL[0] >= 1000) {
                                TIM4 -> CCR1 = FAN_VALUE_1;
																che_do_quat.val = TIM4 -> CCR1;
                        }
                        if (ADC_VAL[0] < 1000) {
                                TIM4 -> CCR1 = FAN_VALUE_0;
																che_do_quat.val = TIM4 -> CCR1;
                        }
                }
						}
if((che_do_quat.val!=che_do_quat_ban_dau.val)||(che_do_quat.flag!=che_do_quat_ban_dau.flag))
					{
						Update_Data_To_Flash=1;
			    }
                if(Update_Data_To_Flash ==1)
									{			
										//Delay_ms(1000);
										Update_Data_To_Flash =0;
										Flash_EraseSector(7);
										Flash_Write_Choose(che_do_quat);
										Flash_Read_Choose(&che_do_quat);
                	  Flash_Read_Choose(&che_do_quat_ban_dau);
									
                    //UART2_SendString("UpdateThanhCong");
										 										
                  }
								}	
}

void Flash_Write_Choose (Che_do_quat che_do_quat )
{
	Flash_Unlock();
	uint16_t len =sizeof(che_do_quat);
	uint8_t *p= (uint8_t*)&che_do_quat;
	Flash_Write_Array(FlashAdd,p,len);
	
	Flash_Lock();
}	

void Flash_Read_Choose (Che_do_quat *che_do_quat )
{
	Flash_Read_Array(FlashAdd,(uint8_t*)che_do_quat,sizeof(che_do_quat));
		
	
}	


void USART2_IRQHandler() {

        RxBuffer[ByteInRxBuffer] = (uint8_t)(USART2 -> DR & (uint8_t) 0xFF);// lay Data tu Buffer
        ByteInRxBuffer++;
        if (RxBuffer[4] == 0x03) {
                ByteInRxBuffer = 0; // cho Byte ve 0
								RxBuffer[4] = 0;
                // check buffer
                if (RxBuffer[1] == 0x01)
									      {
												if(RxBuffer[2]!= 0x01) {
																UART2_SendChar(0x02);
																UART2_SendChar(0x81);
																UART2_SendChar(0x01);
																UART2_SendChar(0xff);
																UART2_SendChar(0x03);
													
												}else{
                        if (RxBuffer[3] == 0x01) {
                                mode = 1;
                                TIM4 -> CCR1 = che_do_quat.val;
                                UART2_SendChar(0x02);
																UART2_SendChar(0x02);
																UART2_SendChar(0x81);
																UART2_SendChar(0x01);
																UART2_SendChar(0x00);
																UART2_SendChar(0x03);
                        } else if (RxBuffer[3] == 0x00) {
                                mode = 0;
                                che_do_quat.val = TIM4 -> CCR1;
                                TIM4 -> CCR1 = FAN_VALUE_0;
                                UART2_SendChar(0x02);
																UART2_SendChar(0x81);
																UART2_SendChar(0x01);
																UART2_SendChar(0x00);
																UART2_SendChar(0x03);
                                che_do_quat.flag = 1;
                        } else {
                                UART2_SendChar(0x02);
																UART2_SendChar(0x81);
																UART2_SendChar(0x01);
																UART2_SendChar(0x01);
																UART2_SendChar(0x03);
                        }

											}
												
												
                }
								else if (RxBuffer[1] == 0x02) 
												{
													if(RxBuffer[2]!= 0x01) {
																UART2_SendChar(0x02);
																UART2_SendChar(0x82);
																UART2_SendChar(0x01);
																UART2_SendChar(0xff);
																UART2_SendChar(0x03);
													
												}else{
                        //frame chon che do
                        if (RxBuffer[3] == 0x01) {
                                if (mode == 0){
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x82);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x03);
																}
                                else {
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x82);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x00);
																		UART2_SendChar(0x03);
																		che_do_quat.flag = 1;
																}
                        } else if (RxBuffer[3] == 0x00) {
                                if (mode == 0){
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x82);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x03);
																}
                                else {
                                    che_do_quat.flag = 0;
                                    UART2_SendChar(0x02);
																		UART2_SendChar(0x82);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x00);
																		UART2_SendChar(0x03);
                                }
                        } else {
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x82);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x03);
                        }
											}                      																								
              }
							else if (RxBuffer[1] == 0x03) {
								if(RxBuffer[2]!= 0x01) {
																UART2_SendChar(0x02);
																UART2_SendChar(0x83);
																UART2_SendChar(0x01);
																UART2_SendChar(0xff);
																UART2_SendChar(0x03);
													
												}else{
                        //frame chon muc gio
                        if (RxBuffer[3] == 0x00) {
                                if (mode == 0){
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x03);
																}
                                else {
                                    TIM4 -> CCR1 = FAN_VALUE_0;
																		che_do_quat.val = TIM4 -> CCR1;
																		che_do_quat.flag = 1;
                                    UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x00);
																		UART2_SendChar(0x03);
                                }
                        } else if (RxBuffer[3] == 0x01) {
                                if (mode == 0){
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x03);
																}
                                else {
                                    TIM4 -> CCR1 = FAN_VALUE_1;
																		che_do_quat.val = TIM4 -> CCR1;
																		che_do_quat.flag = 1;
                                    UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x00);
																		UART2_SendChar(0x03);
                                }
                        } else if (RxBuffer[3] == 0x02) {
                                if (mode == 0){
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x03);
																}
                                else {
                                    TIM4 -> CCR1 = FAN_VALUE_2;
																		che_do_quat.val = TIM4 -> CCR1;
																		che_do_quat.flag = 1;
                                    UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x00);
																		UART2_SendChar(0x03);
                                }
                        } else {
                                UART2_SendChar(0x02);
																UART2_SendChar(0x83);
																UART2_SendChar(0x01);
																UART2_SendChar(0x01);
																UART2_SendChar(0x03);
                        }
                     
											}
												
                }
        }
				if (RxBuffer[1] == 0x04 && RxBuffer[3] == 0x03) {
											ByteInRxBuffer = 0;
											RxBuffer[3] = 0;
											if(RxBuffer[2]!= 0x00) {
																UART2_SendChar(0x02);
																UART2_SendChar(0x84);
																UART2_SendChar(0x01);
																UART2_SendChar(0xff);
																UART2_SendChar(0x03);
													
												}else{
                        //frame doc trang thai
													valfan = TIM4 -> CCR1;
													if(valfan == 0){
														valfan = 0x00;
													}
													if(valfan == 4){
														valfan = 0x01;
													}
													if(valfan == 9){
														valfan = 0x02;
													}
													if(ADC_VAL[0]<1000){
														valadc = 0x00;
													}
													if(ADC_VAL[0]>=3000){
														valadc = 0xff;
													}
													if(ADC_VAL[0] < 3000 && ADC_VAL[0] >= 1000){
														valadc = 0x01;
													}
	
																UART2_SendChar(0x02);
																UART2_SendChar(0x84);
																UART2_SendChar(0x05);
																UART2_SendChar(0x00);
																UART2_SendChar(mode);
																UART2_SendChar(che_do_quat.flag);
																UART2_SendChar(valfan);
																UART2_SendChar(valadc);
																UART2_SendChar(0x03);
												}
				}

}