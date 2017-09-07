/*********************************************************************************************************
*
* File                : touch_panel.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef _TOUCH_PANEL_H_
#define _TOUCH_PANEL_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/**
 * @brief Definition for TouchPanel
 */

#define TP_CS_PIN          LCDTP_CS_Pin
#define TP_CS_PORT         LCDTP_CS_GPIO_Port
#define TP_CS_CLK          __GPIOB_CLK_ENABLE();

#define TP_IRQ_PIN         LCDTP_IRQ_Pin
#define TP_IRQ_PORT        LCDTP_IRQ_GPIO_Port
#define TP_IRQ_CLK         __GPIOB_CLK_ENABLE();


#define TP_CS(x)  x ? HAL_GPIO_WritePin(TP_CS_PORT, TP_CS_PIN, GPIO_PIN_SET): HAL_GPIO_WritePin(TP_CS_PORT, TP_CS_PIN, GPIO_PIN_RESET)

#define TP_INT_IN   HAL_GPIO_ReadPin(TP_IRQ_PORT, TP_IRQ_PIN)

/**
 * @brief Definition for TouchPanel SPI
 */
 /* Configure TouchPanel pins:   TP_CLK-> PB13 and TP_MISO-> PB14 and TP_MOSI-> PB15 */
//#define TP_RCC_SPI                 __SPI2_CLK_ENABLE();

//#define TP_SPI                        SPI2
//#define TP_SPI_CLK_INIT               RCC_APB1PeriphClockCmd
//#define TP_SPI_IRQn                   SPI2_IRQn
//#define TP_SPI_IRQHANDLER             SPI2_IRQHandler

//#define TP_SPI_SCK_PIN                GPIO_Pin_13
//#define TP_SPI_SCK_GPIO_PORT          GPIOB
//#define TP_SPI_SCK_GPIO_CLK           RCC_AHB1Periph_GPIOB
//#define TP_SPI_SCK_SOURCE             GPIO_PinSource13

//#define TP_SPI_MISO_PIN               GPIO_Pin_14
//#define TP_SPI_MISO_GPIO_PORT         GPIOB
//#define TP_SPI_MISO_GPIO_CLK          RCC_AHB1Periph_GPIOB
//#define TP_SPI_MISO_SOURCE            GPIO_PinSource14

//#define TP_SPI_MOSI_PIN               GPIO_Pin_15
//#define TP_SPI_MOSI_GPIO_PORT         GPIOB
//#define TP_SPI_MOSI_GPIO_CLK          RCC_AHB1Periph_GPIOB
//#define TP_SPI_MOSI_SOURCE            GPIO_PinSource15    

/* Private typedef -----------------------------------------------------------*/
typedef  struct POINT 
{
   uint16_t x;
   uint16_t y;
}Coordinate;


typedef struct Matrix 
{            
long double An,  
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
} Matrix ;

/* Private variables ---------------------------------------------------------*/
extern Coordinate ScreenSample[3];
extern Coordinate DisplaySample[3];
extern Matrix matrix ;
extern Coordinate  display ;

/* Private define ------------------------------------------------------------*/

#define  CHX   0x90
#define  CHY   0xd0

/* Private function prototypes -----------------------------------------------*/        
void TP_Init(void);  
Coordinate *Read_Ads7846(void);
void TouchPanel_Calibrate(void);
void DrawCross(uint16_t Xpos, uint16_t Ypos);
void TP_DrawPoint(uint16_t Xpos, uint16_t Ypos);
FunctionalState setCalibrationMatrix( Coordinate * displayPtr, Coordinate * screenPtr, Matrix * matrixPtr);
FunctionalState getDisplayPoint(Coordinate * displayPtr, Coordinate * screenPtr, Matrix * matrixPtr );

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


