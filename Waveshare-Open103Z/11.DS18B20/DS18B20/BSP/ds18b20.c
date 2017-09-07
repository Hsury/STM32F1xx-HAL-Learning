#include "ds18b20.h"


/**
  * @brief  DS18B20 Delay us
  * @param  None
  * @retval None
  */ 
void delay_us(uint32_t value)
{
  uint32_t i;
  i = value * 20;
  while(i--);
}

/**
  * @brief  Reset DS18B20
  * @param  None
  * @retval None
  */ 
void DS18B20_Rst(void)     
{                 
  CLR_DS18B20();    //DQ = 0
  delay_us(750);    //delay 750us
  SET_DS18B20();;   //DQ=1 
  delay_us(15);     //15us
}


/**
  * @brief  Check DS18B20
  * @param  None
  * @retval 1 : no exist
  *         0 : exist
  */ 
uint8_t DS18B20_Check(void)      
{   
  uint8_t retry=0;
  
   while (DS18B20_DQ_IN&&retry<200)
  {
    retry++;
    delay_us(1);
  };   
  if(retry>=200)return 1;
  else retry=0;
  while (!DS18B20_DQ_IN&&retry<240)
  {
    retry++;
    delay_us(1);
  };
  if(retry>=240)return 1;      
  return 0;
}

/**
  * @brief  Read Bit
  * @param  None
  * @retval 1/0
  */ 
uint8_t DS18B20_Read_Bit(void)        // read one bit
{
  uint8_t data;
  
  CLR_DS18B20();   //DQ=0
  delay_us(2);
  SET_DS18B20();;  //DQ=1 
  delay_us(12);
  if(DS18B20_DQ_IN)data=1;
  else data=0;   
  delay_us(50);  
  
  return data;
}

/**
  * @brief  Read One Byte From DS18B20
  * @param  None
  * @retval value 
  */ 
uint8_t DS18B20_Read_Byte(void)    // read one byte
{        
  uint8_t i,j,dat;
  dat=0;
  
  for (i=1;i<=8;i++) 
  {
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }                
    return dat;
}

/**
  * @brief  Write One Byte To DS18B20 
  * @param  Data To Write
  * @retval None
  */ 
void DS18B20_Write_Byte(uint8_t dat)     
{             
  uint8_t j;
  uint8_t testb;
  
  for (j=1;j<=8;j++) 
  {
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            CLR_DS18B20(); //DS18B20_DQ_OUT=0;// Write 1
            delay_us(2);                            
            SET_DS18B20(); //DS18B20_DQ_OUT=1;
            delay_us(60);             
        }
        else 
        {
            CLR_DS18B20(); //DS18B20_DQ_OUT=0;// Write 0
            delay_us(60);             
            SET_DS18B20(); //DS18B20_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}

/**
  * @brief  DS18B20 start convert
  * @param  None
  * @retval None
  */ 
void DS18B20_Start(void)
{                              
    DS18B20_Rst();     
    DS18B20_Check();   
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 


/**
  * @brief  Initialize DS18B20
  * @param  None
  * @retval None
  */ 
uint8_t DS18B20_Init(void)
{
  
  SET_DS18B20();                                 
  
  DS18B20_Rst();
  return DS18B20_Check();
} 


/**
  * @brief  read temp from ds18b20
  * @param  None
  * @retval temp (-55~125)
  */
short DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
    short tem;
  
    DS18B20_Start ();        // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();   
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert      
    TL=DS18B20_Read_Byte();  // LSB   
    TH=DS18B20_Read_Byte();  // MSB  
          
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;             
    }else temp=1;                
    tem=TH;                 
    tem<<=8;    
    tem+=TL;                
    tem=(float)tem*0.625f;    
  if(temp)return tem;       
  else return -tem;    
} 
 
