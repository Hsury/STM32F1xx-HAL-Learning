#ifndef __DS18B20_H
#define __DS18B20_H 

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"   


/** @defgroup GPIO_pins_define GPIO pins define
  * @{
  */
  
/* DS18B20 IO operation functions *****************************************************/
#define CLR_DS18B20()    HAL_GPIO_WritePin (ONEWIRE_GPIO_Port,ONEWIRE_Pin,GPIO_PIN_RESET )
#define SET_DS18B20()    HAL_GPIO_WritePin (ONEWIRE_GPIO_Port,ONEWIRE_Pin,GPIO_PIN_SET )
#define DS18B20_DQ_IN    HAL_GPIO_ReadPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)

/* Exported functions --------------------------------------------------------*/
/** @addtogroup DS18B20_Exported_Functions
  * @{
  */
uint8_t DS18B20_Init(void);       
short DS18B20_Get_Temp(void);   
void DS18B20_Start(void);       
void DS18B20_Write_Byte(uint8_t dat);
uint8_t DS18B20_Read_Byte(void);    
uint8_t DS18B20_Read_Bit(void);     
uint8_t DS18B20_Check(void);        
void DS18B20_Rst(void);         

#endif

