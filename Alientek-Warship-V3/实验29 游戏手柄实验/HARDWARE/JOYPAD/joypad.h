#ifndef __JOYPAD_H
#define __JOYPAD_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32F103������V3
//��Ϸ�ֱ����� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/31
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//�ֱ���������
#define JOYPAD_CLK PDout(3)  	//ʱ��		PD3
#define JOYPAD_LAT PBout(11)  	//����     	PB11
#define JOYPAD_DAT PBin(10)	 	//����     	PB10    

void JOYPAD_Init(void);			//��ʼ��	
void JOYPAD_Delay(u16 t);		//��ʱ
u8 JOYPAD_Read(void);			//��ȡ��ֵ	 				    
#endif
















