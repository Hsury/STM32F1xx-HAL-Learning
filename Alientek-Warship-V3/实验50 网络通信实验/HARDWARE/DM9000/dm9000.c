#include "dm9000.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "lwip_comm.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������V3
//DM9000��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/6/5
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

SRAM_HandleTypeDef DM9000_Handler;    		//DM9000���

struct dm9000_config dm9000cfg;				//DM9000���ýṹ��

//��ʼ��DM9000
//����ֵ:
//0,��ʼ���ɹ�
//1��DM9000A ID��ȡ����
u8 DM9000_Init(void)
{
	u32 temp;
	GPIO_InitTypeDef GPIO_Initure;
	FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
	
    __HAL_RCC_GPIOD_CLK_ENABLE();           	//����GPIODʱ��
	__HAL_RCC_GPIOE_CLK_ENABLE();           	//����GPIOEʱ��
	__HAL_RCC_GPIOF_CLK_ENABLE();           	//����GPIOFʱ��
	__HAL_RCC_GPIOG_CLK_ENABLE();           	//����GPIOGʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_7; 				//PD7
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          	//����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    	 	//����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);

	GPIO_Initure.Pin=GPIO_PIN_6; 				//PG6 DM9000�ж�����
	GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;  	//�жϣ��½���
	HAL_GPIO_Init(GPIOG,&GPIO_Initure);
	
	//PD0 1 4 5 8 8 9 10 14 15
	GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|\
                     GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;	
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;  		//�����������
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
	
	//PE7 8 9 10 11 12 13 14 15
	GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|\
                     GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;	
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;  		//�����������
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	//PF13
	GPIO_Initure.Pin=GPIO_PIN_13;
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;  		//�����������
	HAL_GPIO_Init(GPIOF,&GPIO_Initure);
	
	//PG9
	GPIO_Initure.Pin=GPIO_PIN_9;
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;  		//�����������
	HAL_GPIO_Init(GPIOG,&GPIO_Initure);
	
	//�ж���6=PG6
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,1,0);   	//��ռ���ȼ�Ϊ1�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);         	//ʹ���ж���6
	
	DM9000_Handler.Instance=FSMC_NORSRAM_DEVICE;                
	DM9000_Handler.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;    
    
	DM9000_Handler.Init.NSBank=FSMC_NORSRAM_BANK2;     					//ʹ��NE2
	DM9000_Handler.Init.DataAddressMux=FSMC_DATA_ADDRESS_MUX_DISABLE; 	//��ַ/�����߲�����
	DM9000_Handler.Init.MemoryType=FSMC_MEMORY_TYPE_SRAM;   			//SRAM
	DM9000_Handler.Init.MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16; 	//16λ���ݿ��
	DM9000_Handler.Init.BurstAccessMode=FSMC_BURST_ACCESS_MODE_DISABLE; 	//�Ƿ�ʹ��ͻ������,����ͬ��ͻ���洢����Ч,�˴�δ�õ�
	DM9000_Handler.Init.WaitSignalPolarity=FSMC_WAIT_SIGNAL_POLARITY_LOW;	//�ȴ��źŵļ���,����ͻ��ģʽ����������
	DM9000_Handler.Init.WaitSignalActive=FSMC_WAIT_TIMING_BEFORE_WS;   	//�洢�����ڵȴ�����֮ǰ��һ��ʱ�����ڻ��ǵȴ������ڼ�ʹ��NWAIT
	DM9000_Handler.Init.WriteOperation=FSMC_WRITE_OPERATION_ENABLE;    	//�洢��дʹ��
	DM9000_Handler.Init.WaitSignal=FSMC_WAIT_SIGNAL_DISABLE;           	//�ȴ�ʹ��λ,�˴�δ�õ�
	DM9000_Handler.Init.ExtendedMode=FSMC_EXTENDED_MODE_DISABLE;        	//��дʹ����ͬ��ʱ��
	DM9000_Handler.Init.AsynchronousWait=FSMC_ASYNCHRONOUS_WAIT_DISABLE;	//�Ƿ�ʹ��ͬ������ģʽ�µĵȴ��ź�,�˴�δ�õ�
	DM9000_Handler.Init.WriteBurst=FSMC_WRITE_BURST_DISABLE;           	//��ֹͻ��д
  
  	//FMC��ʱ����ƼĴ���
	FSMC_ReadWriteTim.AddressSetupTime=0x00;       	//��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 1/72M=13.8ns
	FSMC_ReadWriteTim.AddressHoldTime=0x00;			//��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
	FSMC_ReadWriteTim.DataSetupTime=0x03;			//���ݱ���ʱ��Ϊ3��HCLK	=4*13.8=55ns
	FSMC_ReadWriteTim.BusTurnAroundDuration=0X00;
	FSMC_ReadWriteTim.AccessMode=FSMC_ACCESS_MODE_A;//ģʽA
	HAL_SRAM_Init(&DM9000_Handler,&FSMC_ReadWriteTim,&FSMC_ReadWriteTim);	
  
	//FMC��ʱ����ƼĴ���
	FSMC_ReadWriteTim.AddressSetupTime=0x00;       	//��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 1/72M=13.8ns
	FSMC_ReadWriteTim.AddressHoldTime=0x00;			//��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
	FSMC_ReadWriteTim.DataSetupTime=0x03;			//���ݱ���ʱ��Ϊ3��HCLK	=4*13.8=55ns
	FSMC_ReadWriteTim.BusTurnAroundDuration=0X00;
	FSMC_ReadWriteTim.AccessMode=FSMC_ACCESS_MODE_A;//ģʽA
	HAL_SRAM_Init(&DM9000_Handler,&FSMC_ReadWriteTim,&FSMC_ReadWriteTim);	
	

	temp=*(vu32*)(0x1FFFF7E8);				//��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
	dm9000cfg.mode=DM9000_AUTO;	
 	dm9000cfg.queue_packet_len=0;
	//DM9000��SRAM�ķ��ͺͽ���ָ���Զ����ص���ʼ��ַ�����ҿ��������ж�
	dm9000cfg.imr_all = IMR_PAR|IMR_PRI; 
	//��ʼ��MAC��ַ
	dm9000cfg.mac_addr[0]=2;
	dm9000cfg.mac_addr[1]=0;
	dm9000cfg.mac_addr[2]=0;
	dm9000cfg.mac_addr[3]=(temp>>16)&0XFF;	//�����ֽ���STM32��ΨһID
	dm9000cfg.mac_addr[4]=(temp>>8)&0XFFF;
	dm9000cfg.mac_addr[5]=temp&0XFF;
	//��ʼ���鲥��ַ
	dm9000cfg.multicase_addr[0]=0Xff;
	dm9000cfg.multicase_addr[1]=0Xff;
	dm9000cfg.multicase_addr[2]=0Xff;
	dm9000cfg.multicase_addr[3]=0Xff;
	dm9000cfg.multicase_addr[4]=0Xff;
	dm9000cfg.multicase_addr[5]=0Xff;
	dm9000cfg.multicase_addr[6]=0Xff;
	dm9000cfg.multicase_addr[7]=0Xff; 
	
	DM9000_Reset();							//��λDM9000
	delay_ms(100);
	temp=DM9000_Get_DeiviceID();			//��ȡDM9000ID
	printf("DM9000 ID:%#x\r\n",temp);
	if(temp!=DM9000_ID) return 1; 			//��ȡID����
	DM9000_Set_PHYMode(dm9000cfg.mode);		//����PHY����ģʽ
	
	DM9000_WriteReg(DM9000_NCR,0X00);
	DM9000_WriteReg(DM9000_TCR,0X00);		//���Ϳ��ƼĴ�������
	DM9000_WriteReg(DM9000_BPTR,0X3F);	
	DM9000_WriteReg(DM9000_FCTR,0X38);
	DM9000_WriteReg(DM9000_FCR,0X00);
	DM9000_WriteReg(DM9000_SMCR,0X00);		//����ģʽ
	DM9000_WriteReg(DM9000_NSR,NSR_WAKEST|NSR_TX2END|NSR_TX1END);//�������״̬
	DM9000_WriteReg(DM9000_ISR,0X0F);		//����ж�״̬
	DM9000_WriteReg(DM9000_TCR2,0X80);		//�л�LED��mode1 	
	//����MAC��ַ���鲥��ַ
	DM9000_Set_MACAddress(dm9000cfg.mac_addr);		//����MAC��ַ
	DM9000_Set_Multicast(dm9000cfg.multicase_addr);	//�����鲥��ַ
	DM9000_WriteReg(DM9000_RCR,RCR_DIS_LONG|RCR_DIS_CRC|RCR_RXEN);
	DM9000_WriteReg(DM9000_IMR,IMR_PAR); 
	temp=DM9000_Get_SpeedAndDuplex();		//��ȡDM9000�������ٶȺ�˫��״̬
	if(temp!=0XFF)							//���ӳɹ���ͨ��������ʾ�����ٶȺ�˫��״̬
	{
		printf("DM9000 Speed:%dMbps,Duplex:%s duplex mode\r\n",(temp&0x02)?10:100,(temp&0x01)?"Full":"Half");
	}else printf("DM9000 Establish Link Failed!\r\n");
	DM9000_WriteReg(DM9000_IMR,dm9000cfg.imr_all);	//�����ж�
	return 0;		
}

//��ȡDM9000ָ���Ĵ�����ֵ
//reg:�Ĵ�����ַ
//����ֵ��DM9000ָ���Ĵ�����ֵ
u16 DM9000_ReadReg(u16 reg)
{
	DM9000->REG=reg;
	return DM9000->DATA; 
}

//��DM9000ָ���Ĵ�����д��ָ��ֵ
//reg:Ҫд��ļĴ���
//data:Ҫд���ֵ
void DM9000_WriteReg(u16 reg,u16 data)
{
	DM9000->REG=reg;
	DM9000->DATA=data;
}

//��ȡDM9000��PHY��ָ���Ĵ���
//reg:Ҫ����PHY�Ĵ���
//����ֵ:��ȡ����PHY�Ĵ���ֵ
u16 DM9000_PHY_ReadReg(u16 reg)
{
	u16 temp;
	DM9000_WriteReg(DM9000_EPAR,DM9000_PHY|reg);
	DM9000_WriteReg(DM9000_EPCR,0X0C);				//ѡ��PHY�����Ͷ�����
	delay_ms(10);
	DM9000_WriteReg(DM9000_EPCR,0X00);				//���������
	temp=(DM9000_ReadReg(DM9000_EPDRH)<<8)|(DM9000_ReadReg(DM9000_EPDRL));
	return temp;
}

//��DM9000��PHY�Ĵ���д��ָ��ֵ
//reg:PHY�Ĵ���
//data:Ҫд���ֵ
void DM9000_PHY_WriteReg(u16 reg,u16 data)
{
	DM9000_WriteReg(DM9000_EPAR,DM9000_PHY|reg);
	DM9000_WriteReg(DM9000_EPDRL,(data&0xff));		//д����ֽ�
	DM9000_WriteReg(DM9000_EPDRH,((data>>8)&0xff));	//д����ֽ�
	DM9000_WriteReg(DM9000_EPCR,0X0A);				//ѡ��PHY,����д����
	delay_ms(50);
	DM9000_WriteReg(DM9000_EPCR,0X00);				//���д����	
}

//��ȡDM9000��оƬID
//����ֵ��DM9000��оƬIDֵ
u32 DM9000_Get_DeiviceID(void)
{
	u32 value;
	value =DM9000_ReadReg(DM9000_VIDL);
	value|=DM9000_ReadReg(DM9000_VIDH) << 8;
	value|=DM9000_ReadReg(DM9000_PIDL) << 16;
	value|=DM9000_ReadReg(DM9000_PIDH) << 24;
	return value;
}

//��ȡDM9000�������ٶȺ�˫��ģʽ
//����ֵ��	0,100M��˫��
//			1,100Mȫ˫��
//			2,10M��˫��
//			3,10Mȫ˫��
//			0XFF,����ʧ�ܣ�
u8 DM9000_Get_SpeedAndDuplex(void)
{
	u8 temp;
	u8 i=0;	
	if(dm9000cfg.mode==DM9000_AUTO)					//����������Զ�Э��ģʽһ��Ҫ�ȴ�Э�����
	{
		while(!(DM9000_PHY_ReadReg(0X01)&0X0020))	//�ȴ��Զ�Э�����
		{
			delay_ms(100);					
			i++;
			if(i>100)return 0XFF;					//�Զ�Э��ʧ��
		}	
	}else											//�Զ���ģʽ,һ��Ҫ�ȴ����ӳɹ�
	{
		while(!(DM9000_ReadReg(DM9000_NSR)&0X40))	//�ȴ����ӳɹ�
		{
			delay_ms(100);					
			i++;
			if(i>100)return 0XFF;					//����ʧ��			
		}
	}
	temp =((DM9000_ReadReg(DM9000_NSR)>>6)&0X02);	//��ȡDM9000�������ٶ�
	temp|=((DM9000_ReadReg(DM9000_NCR)>>3)&0X01);	//��ȡDM9000��˫��״̬
	return temp;
}

//����DM900��PHY����ģʽ
//mode:PHYģʽ
void DM9000_Set_PHYMode(u8 mode)
{
	u16 BMCR_Value,ANAR_Value;	
	switch(mode)
	{
		case DM9000_10MHD:		//10M��˫��
			BMCR_Value=0X0000;
			ANAR_Value=0X21;
			break;
		case DM9000_10MFD:		//10Mȫ˫��
			BMCR_Value=0X0100;
			ANAR_Value=0X41;
			break;
		case DM9000_100MHD:		//100M��˫��
			BMCR_Value=0X2000;
			ANAR_Value=0X81;
			break;
		case DM9000_100MFD:		//100Mȫ˫��
			BMCR_Value=0X2100;
			ANAR_Value=0X101;
			break;
		case DM9000_AUTO:		//�Զ�Э��ģʽ
			BMCR_Value=0X1000;
			ANAR_Value=0X01E1;
			break;		
	}
	DM9000_PHY_WriteReg(DM9000_PHY_BMCR,BMCR_Value);
	DM9000_PHY_WriteReg(DM9000_PHY_ANAR,ANAR_Value);
 	DM9000_WriteReg(DM9000_GPR,0X00);	//ʹ��PHY
}

//����DM9000��MAC��ַ
//macaddr:ָ��MAC��ַ
void DM9000_Set_MACAddress(u8 *macaddr)
{
	u8 i;
	for(i=0;i<6;i++)
	{
		DM9000_WriteReg(DM9000_PAR+i,macaddr[i]);
	}
}
//����DM9000���鲥��ַ
//multicastaddr:ָ��ಥ��ַ
void DM9000_Set_Multicast(u8 *multicastaddr)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		DM9000_WriteReg(DM9000_MAR+i,multicastaddr[i]);
	}
}
//��λDM9000
void DM9000_Reset(void)
{
	//��λDM9000,��λ����ο�<DM9000 Application Notes V1.22>�ֲ�29ҳ
	DM9000_RST = 0;								//DM9000Ӳ����λ
	delay_ms(10);
	DM9000_RST = 1; 							//DM9000Ӳ����λ����
	delay_ms(100);								//һ��Ҫ�������ʱ����DM9000׼��������
 	DM9000_WriteReg(DM9000_GPCR,0x01);			//��һ��:����GPCR�Ĵ���(0X1E)��bit0Ϊ1 
	DM9000_WriteReg(DM9000_GPR,0);				//�ڶ���:����GPR�Ĵ���(0X1F)��bit1Ϊ0��DM9000�ڲ���PHY�ϵ�
 	DM9000_WriteReg(DM9000_NCR,(0x02|NCR_RST));	//������:�����λDM9000 
	do 
	{
		delay_ms(25); 	
	}while(DM9000_ReadReg(DM9000_NCR)&1);		//�ȴ�DM9000��λ���
	DM9000_WriteReg(DM9000_NCR,0);
	DM9000_WriteReg(DM9000_NCR,(0x02|NCR_RST));	//DM9000�ڶ�����λ
	do 
	{
		delay_ms(25);	
	}while (DM9000_ReadReg(DM9000_NCR)&1);
} 

//ͨ��DM9000�������ݰ�
//p:pbuf�ṹ��ָ��
void DM9000_SendPacket(struct pbuf *p)
{
	struct pbuf *q;
	u16 pbuf_index = 0;
	u8 word[2], word_index = 0;	

	DM9000_WriteReg(DM9000_IMR,IMR_PAR);		//�ر������ж� 
	DM9000->REG=DM9000_MWCMD;					//���ʹ������Ϳ��Խ�Ҫ���͵����ݰᵽDM9000 TX SRAM��	
	q=p;
	//��DM9000��TX SRAM��д�����ݣ�һ��д�������ֽ�����
	//��Ҫ���͵����ݳ���Ϊ������ʱ��������Ҫ�����һ���ֽڵ���д��DM9000��TX SRAM��
 	while(q)
	{
		if (pbuf_index < q->len)
		{
			word[word_index++] = ((u8_t*)q->payload)[pbuf_index++];
			if (word_index == 2)
			{
				DM9000->DATA=((u16)word[1]<<8)|word[0];
				word_index = 0;
			}
		}else
		{
			q=q->next;
			pbuf_index = 0;
		}
	}
	//����һ���ֽ�δд��TX SRAM
	if(word_index==1)DM9000->DATA=word[0];
	//��DM9000д�뷢�ͳ���
	DM9000_WriteReg(DM9000_TXPLL,p->tot_len&0XFF);
	DM9000_WriteReg(DM9000_TXPLH,(p->tot_len>>8)&0XFF);		//����Ҫ�������ݵ����ݳ���
	DM9000_WriteReg(DM9000_TCR,0X01);						//�������� 
	while((DM9000_ReadReg(DM9000_ISR)&0X02)==0);			//�ȴ�������� 
	DM9000_WriteReg(DM9000_ISR,0X02);						//�����������ж� 
 	DM9000_WriteReg(DM9000_IMR,dm9000cfg.imr_all);			//DM9000���������ж�ʹ��						
}

//DM9000�������ݰ�
//���յ������ݰ������DM9000��RX FIFO�У���ַΪ0X0C00~0X3FFF
//���յ������ݰ���ǰ�ĸ��ֽڲ�������ʵ�����ݣ��������ض������
//byte1:�����Ƿ���յ����ݣ�Ϊ0x00����0X01��������������ǵĻ�һ��Ҫ�����λDM9000
//		0x01�����յ�����
//		0x00��δ���յ�����
//byte2:�ڶ����ֽڱ�ʾһЩ״̬��Ϣ����DM9000��RSR(0X06)�Ĵ���һ�µ�
//byte3:��֡���ݳ��ȵĵ��ֽ�
//byte4:��֡���ݳ��ȵĸ��ֽ�
//����ֵ��pbuf��ʽ�Ľ��յ������ݰ�
struct pbuf *DM9000_Receive_Packet(void)
{
	struct pbuf* p;
	struct pbuf* q;
    u32 rxbyte;
	vu16 rx_status, rx_length;
    u16* data;
	u16 dummy; 
	int len;

	p=NULL; 
__error_retry:	
	DM9000_ReadReg(DM9000_MRCMDX);					//�ٶ�
	rxbyte=(u8)DM9000->DATA;						//���еڶ��ζ�ȡ 
	if(rxbyte)										//���յ�����
	{
		if(rxbyte>1)								//rxbyte����1�����յ������ݴ���,����		
		{
            printf("dm9000 rx: rx error, stop device\r\n");
			DM9000_WriteReg(DM9000_RCR,0x00);
			DM9000_WriteReg(DM9000_ISR,0x80);		 
			return (struct pbuf*)p;
		}
		DM9000->REG=DM9000_MRCMD;
		rx_status=DM9000->DATA;
        rx_length=DM9000->DATA;  
		//if(rx_length>512)printf("rxlen:%d\r\n",rx_length);
        p=pbuf_alloc(PBUF_RAW,rx_length,PBUF_POOL);	//pbufs�ڴ�ط���pbuf
		if(p!=NULL)									//�ڴ�����ɹ�
        {
            for(q=p;q!=NULL;q=q->next)
            {
                data=(u16*)q->payload;
                len=q->len;
                while(len>0)
                {
					*data=DM9000->DATA;
                    data++;
                    len-= 2;
                }
            }
        }else										//�ڴ�����ʧ��
		{
			printf("pbuf�ڴ�����ʧ��:%d\r\n",rx_length);
            data=&dummy;
			len=rx_length;
			while(len)
			{
				*data=DM9000->DATA;
				len-=2;
			}
        }	
		//����rx_status�жϽ��������Ƿ�������´���FIFO�����CRC����
		//���������������������κ�һ�����ֵĻ�����������֡��
		//��rx_lengthС��64���ߴ���������ݳ��ȵ�ʱ��Ҳ����������֡
		if((rx_status&0XBF00) || (rx_length < 0X40) || (rx_length > DM9000_PKT_MAX))
		{
			printf("rx_status:%#x\r\n",rx_status);
			if (rx_status & 0x100)printf("rx fifo error\r\n");
            if (rx_status & 0x200)printf("rx crc error\r\n");
            if (rx_status & 0x8000)printf("rx length error\r\n");
            if (rx_length>DM9000_PKT_MAX)
			{
				printf("rx length too big\r\n");
				DM9000_WriteReg(DM9000_NCR, NCR_RST); 	//��λDM9000
				delay_ms(5);
			}
			if(p!=NULL)pbuf_free((struct pbuf*)p);		//�ͷ��ڴ�
			p=NULL;
			goto __error_retry;
		}
	}else
    {
        DM9000_WriteReg(DM9000_ISR,ISR_PTS);			//��������жϱ�־λ
        dm9000cfg.imr_all=IMR_PAR|IMR_PRI;				//���½����ж� 
        DM9000_WriteReg(DM9000_IMR, dm9000cfg.imr_all);
    } 
	return (struct pbuf*)p; 
}
//�жϴ�����
void DMA9000_ISRHandler(void)
{
	u16 int_status;
	u16 last_io; 
	last_io = DM9000->REG;
	int_status=DM9000_ReadReg(DM9000_ISR); 
	DM9000_WriteReg(DM9000_ISR,int_status);				//����жϱ�־λ��DM9000��ISR�Ĵ�����bit0~bit5д1����
	if(int_status & ISR_ROS)printf("overflow \r\n");
    if(int_status & ISR_ROOS)printf("overflow counter overflow \r\n");	
	if(int_status & ISR_PRS)		//�����ж�
	{  
 		//��������жϣ��û���������������
	} 
	if(int_status&ISR_PTS)			//�����ж�
	{ 
		//��������жϣ��û���������������
	}
	DM9000->REG=last_io;	
}

//�ⲿ�ж���6���жϷ�����
void EXTI9_5_IRQHandler(void)
{
	while(DM9000_INT == 0)
	{
		DMA9000_ISRHandler();
	}
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);		//����ж���6�����־λ
}
