#include "recorder.h"
#include "delay.h"
#include "usart.h"
#include "key.h"	  
#include "led.h"	  
#include "lcd.h"	    
#include "vs10xx.h"	  
#include "malloc.h"
#include "ff.h"
#include "exfuns.h"	    
#include "text.h"	    
#include "tpad.h"	    
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������V3
//wav¼������ ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/6/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved								  						    								  
//////////////////////////////////////////////////////////////////////////////////
	

//VS1053��WAV¼����bug,���plugin��������������� 							    
const u16 wav_plugin[40]=/* Compressed plugin */ 
{ 
0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14, /* 0 */ 
0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030, /* 8 */ 
0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101, /* 10 */ 
0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1, /* 18 */ 
0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e,  
}; 
//����PCM ¼��ģʽ
//agc:0,�Զ�����.1024�൱��1��,512�൱��0.5��,���ֵ65535=64��		  
void recoder_enter_rec_mode(u16 agc)
{
	//�����IMA ADPCM,�����ʼ��㹫ʽ����:
 	//������=CLKI/256*d;	
	//����d=0,��2��Ƶ,�ⲿ����Ϊ12.288M.��ôFc=(2*12288000)/256*6=16Khz
	//���������PCM,������ֱ�Ӿ�д����ֵ 
   	VS_WR_Cmd(SPI_BASS,0x0000);    
 	VS_WR_Cmd(SPI_AICTRL0,8000);	//���ò�����,����Ϊ8Khz
 	VS_WR_Cmd(SPI_AICTRL1,agc);		//��������,0,�Զ�����.1024�൱��1��,512�൱��0.5��,���ֵ65535=64��	
 	VS_WR_Cmd(SPI_AICTRL2,0);		//�����������ֵ,0,�������ֵ65536=64X
 	VS_WR_Cmd(SPI_AICTRL3,6);		//��ͨ��(MIC����������)
	VS_WR_Cmd(SPI_CLOCKF,0X2000);	//����VS10XX��ʱ��,MULT:2��Ƶ;ADD:������;CLK:12.288Mhz
	VS_WR_Cmd(SPI_MODE,0x1804);		//MIC,¼������    
 	delay_ms(5);					//�ȴ�����1.35ms 
 	VS_Load_Patch((u16*)wav_plugin,40);//VS1053��WAV¼����Ҫpatch
}

//��ʼ��WAVͷ.
void recoder_wav_init(__WaveHeader* wavhead) //��ʼ��WAVͷ			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//��δȷ��,�����Ҫ����
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//��СΪ16���ֽ�
	wavhead->fmt.AudioFormat=0X01; 		//0X01,��ʾPCM;0X01,��ʾIMA ADPCM
 	wavhead->fmt.NumOfChannels=1;		//������
 	wavhead->fmt.SampleRate=8000;		//8Khz������ ��������
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*2;//16λ,��2���ֽ�
 	wavhead->fmt.BlockAlign=2;			//���С,2���ֽ�Ϊһ����
 	wavhead->fmt.BitsPerSample=16;		//16λPCM
   	wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//���ݴ�С,����Ҫ����  
}
							    
//��ʾ¼��ʱ��
//x,y:��ַ
//tsec:������.
void recoder_show_time(u32 tsec)
{   
	//��ʾ¼��ʱ��			 
	LCD_ShowString(30,250,200,16,16,"TIME:");	  	  
	LCD_ShowxNum(30+40,250,tsec/60,2,16,0X80);	//����
	LCD_ShowChar(30+56,250,':',16,0);
	LCD_ShowxNum(30+64,250,tsec%60,2,16,0X80);	//����		
}  	   
//ͨ��ʱ���ȡ�ļ���
//������SD������,��֧��FLASH DISK����
//��ϳ�:����"0:RECORDER/REC20120321210633.wav"���ļ���
void recoder_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:RECORDER/REC%05d.wav",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//���Դ�����ļ�
		if(res==FR_NO_FILE)break;		//���ļ���������=����������Ҫ��.
		index++;
	}
}
//��ʾAGC��С
//x,y:����
//agc:����ֵ 1~15,��ʾ1~15��;0,��ʾ�Զ�����
void recoder_show_agc(u8 agc)
{  
	LCD_ShowString(30+110,250,200,16,16,"AGC:    ");	  	//��ʾ����,ͬʱ����ϴε���ʾ	  
	if(agc==0)LCD_ShowString(30+142,250,200,16,16,"AUTO");	//�Զ�agc	  	  
	else LCD_ShowxNum(30+142,250,agc,2,16,0X80);			//��ʾAGCֵ	 
} 

//����pname���wav�ļ���Ҳ������MP3�ȣ�		 
u8 rec_play_wav(u8 *pname)
{	 
 	FIL* fmp3;
    u16 br;
	u8 res,rval=0;	  
	u8 *databuf;	   		   
	u16 i=0; 	 		  
	fmp3=(FIL*)mymalloc(SRAMIN,sizeof(FIL));//�����ڴ�
	databuf=(u8*)mymalloc(SRAMIN,512);		//����512�ֽڵ��ڴ�����
	if(databuf==NULL||fmp3==NULL)rval=0XFF ;//�ڴ�����ʧ��.
	if(rval==0)
	{	  
		VS_HD_Reset();		   	//Ӳ��λ
		VS_Soft_Reset();  		//��λ 
		VS_Set_All();			//���������Ȳ��� 			 
		VS_Reset_DecodeTime();	//��λ����ʱ�� 	  	 
		res=f_open(fmp3,(const TCHAR*)pname,FA_READ);//���ļ�	 
 		if(res==0)//�򿪳ɹ�.
		{ 
			VS_SPI_SpeedHigh();	//����						   
			while(rval==0)
			{
				res=f_read(fmp3,databuf,512,(UINT*)&br);//����4096���ֽ�  
				i=0;
				do//������ѭ��
			    {  	
					if(VS_Send_MusicData(databuf+i)==0)i+=32;//��VS10XX������Ƶ����
				 	else recoder_show_time(VS_Get_DecodeTime());//��ʾ����ʱ��	   	    
				}while(i<512);//ѭ������4096���ֽ� 
				if(br!=512||res!=0)
				{
					rval=0;
					break;//������.		  
				} 							 
			}
			f_close(fmp3);
		}else rval=0XFF;//���ִ���
		VS_SPK_Set(0);	//�رհ������� 
	}		
	myfree(SRAMIN,fmp3);
	myfree(SRAMIN,databuf);
	return rval;	  	 		  	    
}	 
//¼����
//����¼���ļ�,��������SD��RECORDER�ļ�����.
u8 recoder_play(void)
{
	u8 res;
	u8 key;
	u8 rval=0;
	__WaveHeader *wavhead=0;
	u32 sectorsize=0;
	FIL* f_rec=0;					//�ļ�		    
 	DIR recdir;	 					//Ŀ¼
	u8 *recbuf;						//�����ڴ�	 
 	u16 w;
	u16 idx=0;	    
	u8 rec_sta=0;					//¼��״̬
									//[7]:0,û��¼��;1,��¼��;
									//[6:1]:����
									//[0]:0,����¼��;1,��ͣ¼��;
 	u8 *pname=0;
	u8 timecnt=0;					//��ʱ��   
	u32 recsec=0;					//¼��ʱ��
 	u8 recagc=4;					//Ĭ������Ϊ4
  	while(f_opendir(&recdir,"0:/RECORDER"))//��¼���ļ���
 	{	 
		Show_Str(30,230,240,16,"RECORDER�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,230,240,246,WHITE);		//�����ʾ	     
		delay_ms(200);				  
		f_mkdir("0:/RECORDER");				//������Ŀ¼   
	} 
  	f_rec=(FIL *)mymalloc(SRAMIN,sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_rec==NULL)rval=1;	//����ʧ��
 	wavhead=(__WaveHeader*)mymalloc(SRAMIN,sizeof(__WaveHeader));//����__WaveHeader�ֽڵ��ڴ�����
	if(wavhead==NULL)rval=1; 
	recbuf=mymalloc(SRAMIN,512); 	
	if(recbuf==NULL)rval=1;	  		   
	pname=mymalloc(SRAMIN,30);					//����30���ֽ��ڴ�,����"0:RECORDER/REC00001.wav"
	if(pname==NULL)rval=1;
 	if(rval==0)									//�ڴ�����OK
	{      
 		recoder_enter_rec_mode(1024*recagc);				
   		while(VS_RD_Reg(SPI_HDAT1)>>8);			//�ȵ�buf ��Ϊ�����ٿ�ʼ  
  		recoder_show_time(recsec);				//��ʾʱ��
		recoder_show_agc(recagc);				//��ʾagc
		pname[0]=0;								//pnameû���κ��ļ���		 
 	   	while(rval==0)
		{
			key=KEY_Scan(0);
			switch(key)
			{		
				case KEY2_PRES:	//STOP&SAVE
					if(rec_sta&0X80)//��¼��
					{
						wavhead->riff.ChunkSize=sectorsize*512+36;	//�����ļ��Ĵ�С-8;
				   		wavhead->data.ChunkSize=sectorsize*512;		//���ݴ�С
						f_lseek(f_rec,0);							//ƫ�Ƶ��ļ�ͷ.
				  		f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����
						f_close(f_rec);
						sectorsize=0;
					}
					rec_sta=0;
					recsec=0;
				 	LED1=1;	 						//�ر�DS1
					LCD_Fill(30,230,240,246,WHITE);	//�����ʾ,���֮ǰ��ʾ��¼���ļ���	     
					recoder_show_time(recsec);		//��ʾʱ��
					break;	 
				case KEY0_PRES:	//REC/PAUSE
					if(rec_sta&0X01)//ԭ������ͣ,����¼��
					{
						rec_sta&=0XFE;//ȡ����ͣ
					}else if(rec_sta&0X80)//�Ѿ���¼����,��ͣ
					{
						rec_sta|=0X01;	//��ͣ
					}else				//��û��ʼ¼�� 
					{
	 					rec_sta|=0X80;	//��ʼ¼��	 	 
						recoder_new_pathname(pname);			//�õ��µ�����
						Show_Str(30,230,240,16,pname+11,16,0);	//��ʾ��ǰ¼���ļ�����
				 		recoder_wav_init(wavhead);				//��ʼ��wav����	
	 					res=f_open(f_rec,(const TCHAR*)pname, FA_CREATE_ALWAYS | FA_WRITE); 
						if(res)			//�ļ�����ʧ��
						{
							rec_sta=0;	//�����ļ�ʧ��,����¼��
							rval=0XFE;	//��ʾ�Ƿ����SD��
						}else res=f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����
	 				}
					if(rec_sta&0X01)LED1=0;	//��ʾ������ͣ
					else LED1=1;
					break;
				case WKUP_PRES:	//AGC+	 
				case KEY1_PRES:	//AGC-
					if(key==WKUP_PRES)recagc++;
					else if(recagc)recagc--;
					if(recagc>15)recagc=15;				//��Χ�޶�Ϊ0~15.0,�Զ�AGC.����AGC����										 
					recoder_show_agc(recagc);
					VS_WR_Cmd(SPI_AICTRL1,1024*recagc);	//��������,0,�Զ�����.1024�൱��1��,512�൱��0.5��
					break;
			} 
///////////////////////////////////////////////////////////
//��ȡ����			  
			if(rec_sta==0X80)//�Ѿ���¼����
			{
		  		w=VS_RD_Reg(SPI_HDAT1);	
				if((w>=256)&&(w<896))
				{
	 				idx=0;				   	 
		  			while(idx<512) 	//һ�ζ�ȡ512�ֽ�
					{	 
			 			w=VS_RD_Reg(SPI_HDAT0);				   	    
		 				recbuf[idx++]=w&0XFF;
						recbuf[idx++]=w>>8;
					}	  		 
	 				res=f_write(f_rec,recbuf,512,&bw);//д���ļ�
					if(res)
					{
						printf("err:%d\r\n",res);
						printf("bw:%d\r\n",bw);
						break;//д�����.	  
					}
					sectorsize++;//����������1,ԼΪ32ms	 
				}			
			}else//û�п�ʼ¼��������TPAD����
			{								  
				if(TPAD_Scan(0)&&pname[0])//�����������������,��pname��Ϊ��
				{				 
					Show_Str(30,230,240,16,"����:",16,0);		   
					Show_Str(30+40,230,240,16,pname+11,16,0);	//��ʾ�����ŵ��ļ�����   
					rec_play_wav(pname);						//����pname
					LCD_Fill(30,230,240,246,WHITE);				//�����ʾ,���֮ǰ��ʾ��¼���ļ���	  
					recoder_enter_rec_mode(1024*recagc);		//���½���¼��ģʽ		
			   		while(VS_RD_Reg(SPI_HDAT1)>>8);				//�ȵ�buf ��Ϊ�����ٿ�ʼ  
			  		recoder_show_time(recsec);					//��ʾʱ��
					recoder_show_agc(recagc);					//��ʾagc
 				}
				delay_ms(5);
				timecnt++;
				if((timecnt%20)==0)LED0=!LED0;//DS0��˸ 
			}
/////////////////////////////////////////////////////////////
 			if(recsec!=(sectorsize*4/125))//¼��ʱ����ʾ
			{	   
				LED0=!LED0;//DS0��˸ 
				recsec=sectorsize*4/125;
				recoder_show_time(recsec);//��ʾʱ��
			}
		}					   
	}	   		   				    
	myfree(SRAMIN,wavhead);
	myfree(SRAMIN,recbuf);	  
 	myfree(SRAMIN,f_rec);	 
	myfree(SRAMIN,pname);
	return rval;
}


























