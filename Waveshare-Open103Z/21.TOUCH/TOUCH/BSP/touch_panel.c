/*********************************************************************************************************
*
* File                : touch_panel.c
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

/* Includes ------------------------------------------------------------------*/
#include "touch_panel.h"
#include "stm3210x_lcd.h"
#include "spi.h"


/* Private variables ---------------------------------------------------------*/
Matrix matrix ;
Coordinate  display ;


Coordinate ScreenSample[3];

Coordinate DisplaySample[3] = {
                                {30, 45},
                                {220, 45},
                                {160,210}
                              };

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2
#define TP_hspi hspi2
/*******************************************************************************
* Function Name  : TP_Init
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void TP_Init(void) 
{ 

//  GPIO_InitTypeDef GPIO_InitStructure;
//  
//  RCC_AHB1PeriphClockCmd(Open_SPI_SCK_GPIO_CLK | Open_SPI_MISO_GPIO_CLK | Open_SPI_MOSI_GPIO_CLK,ENABLE);

//  RCC_AHB1PeriphClockCmd(Open_TP_CS_CLK | Open_TP_IRQ_CLK,ENABLE);
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE);
//  Open_SPI_CLK_INIT(Open_RCC_SPI,ENABLE);

//  GPIO_PinAFConfig(Open_SPI_SCK_GPIO_PORT,  Open_SPI_SCK_SOURCE,  Open_GPIO_AF_SPI);
//  GPIO_PinAFConfig(Open_SPI_MISO_GPIO_PORT, Open_SPI_MISO_SOURCE, Open_GPIO_AF_SPI);
//  GPIO_PinAFConfig(Open_SPI_MOSI_GPIO_PORT, Open_SPI_MOSI_SOURCE, Open_GPIO_AF_SPI);

//  GPIO_InitStructure.GPIO_Pin = Open_SPI_SCK_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;  
//  GPIO_Init(Open_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = Open_SPI_MISO_PIN;
//  GPIO_Init(Open_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = Open_SPI_MOSI_PIN;
//  GPIO_Init(Open_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);


//  /*7 inch TP_CS  */
//  GPIO_InitStructure.GPIO_Pin = Open_TP_CS_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(Open_TP_CS_PORT, &GPIO_InitStructure);

//   /*4.3 inch TP_CS  */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOI, &GPIO_InitStructure);
//  
//    /*TP_IRQ */
//    GPIO_InitStructure.GPIO_Pin = Open_TP_IRQ_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(Open_TP_IRQ_PORT, &GPIO_InitStructure);  

//  GPIO_SetBits(GPIOI,GPIO_Pin_3);
//    TP_CS(1); 
//    ADS7843_SPI_Init(); 
} 





/*******************************************************************************
* Function Name  : DelayUS
* Description    : 
* Input          : - cnt:
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void DelayUS(uint32_t cnt)
{
  uint32_t i;
  i = cnt * 4;
  while(i--);
}


/*******************************************************************************
* Function Name  : WR_CMD
* Description    : 
* Input          : - cmd: 
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void WR_CMD (uint8_t cmd)  
{ 
  HAL_SPI_Transmit(&TP_hspi,&cmd,1,1000);
} 



/*******************************************************************************
* Function Name  : RD_AD
* Description    : 
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
static int RD_AD(void)  
{ 
  uint8_t buf[2];
  int value;
  HAL_SPI_Receive(&TP_hspi,buf,2,1000);
  value = (uint16_t)((buf[0] << 8) + buf[1]) >> 3;
  return value;
} 


/*******************************************************************************
* Function Name  : Read_X
* Description    : Read ADS7843 ADC X 
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
int Read_X(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(CHX); 
  DelayUS(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;    
} 

/*******************************************************************************
* Function Name  : Read_Y
* Description    : Read ADS7843 ADC Y
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
int Read_Y(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(CHY); 
  DelayUS(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;     
} 


/*******************************************************************************
* Function Name  : TP_GetAdXY
* Description    : Read ADS7843
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
void TP_GetAdXY(int *x,int *y)  
{ 
  int adx,ady; 
  adx=Read_X(); 
  DelayUS(1); 
  ady=Read_Y(); 
  *x=adx; 
  *y=ady; 
} 

/*******************************************************************************
* Function Name  : TP_DrawPoint
* Description    : 
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos)
{
//   LCD_SetPoint(Xpos,Ypos,Blue);     /* Center point */
//   LCD_SetPoint(Xpos+1,Ypos,Blue);
//   LCD_SetPoint(Xpos,Ypos+1,Blue);
//   LCD_SetPoint(Xpos+1,Ypos+1,Blue);  
}  

/*******************************************************************************
* Function Name  : DrawCross
* Description    : 
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void DrawCross(uint16_t Xpos,uint16_t Ypos)
{
  BSP_LCD_DrawHLine(Xpos-13, Ypos, 10);
  BSP_LCD_DrawHLine(Xpos+4, Ypos, 10);
  BSP_LCD_DrawVLine(Xpos, Ypos-13, 10);
  BSP_LCD_DrawVLine(Xpos, Ypos+4, 10);  

}  
  
/*******************************************************************************
* Function Name  : Read_Ads7846
* Description    : Get TouchPanel X Y
* Input          : None
* Output         : None
* Return         : Coordinate *
* Attention      : None
*******************************************************************************/
Coordinate *Read_Ads7846(void)
{
  static Coordinate  screen;
  int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
  uint8_t count=0;
  int buffer[2][9]={{0},{0}};
  
  do
  {       
    TP_GetAdXY(TP_X,TP_Y);  
    buffer[0][count]=TP_X[0];  
    buffer[1][count]=TP_Y[0];
    count++;  
  }
  while(!TP_INT_IN&& count<9);  /* TP_INT_IN  */
  if(count==9)   /* Average X Y  */ 
  {
    /* Average X  */
    temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
    temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
    temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
   
    m0=temp[0]-temp[1];
    m1=temp[1]-temp[2];
    m2=temp[2]-temp[0];
   
    m0=m0>0?m0:(-m0);
    m1=m1>0?m1:(-m1);
    m2=m2>0?m2:(-m2);
   
    if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;
   
    if(m0<m1)
    {
      if(m2<m0) 
        screen.x=(temp[0]+temp[2])/2;
      else 
        screen.x=(temp[0]+temp[1])/2;  
    }
    else if(m2<m1) 
      screen.x=(temp[0]+temp[2])/2;
    else 
      screen.x=(temp[1]+temp[2])/2;
   
    /* Average Y  */
    temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
    temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
    temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
    m0=temp[0]-temp[1];
    m1=temp[1]-temp[2];
    m2=temp[2]-temp[0];
    m0=m0>0?m0:(-m0);
    m1=m1>0?m1:(-m1);
    m2=m2>0?m2:(-m2);
    if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) return 0;
   
    if(m0<m1)
    {
      if(m2<m0) 
        screen.y=(temp[0]+temp[2])/2;
      else 
        screen.y=(temp[0]+temp[1])/2;  
      }
    else if(m2<m1) 
       screen.y=(temp[0]+temp[2])/2;
    else
       screen.y=(temp[1]+temp[2])/2;
   
    return &screen;
  }
  return 0; 
}
   

/*******************************************************************************
* Function Name  : setCalibrationMatrix
* Description    : Calculate K A B C D E F
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
FunctionalState setCalibrationMatrix( Coordinate * displayPtr,
                          Coordinate * screenPtr,
                          Matrix * matrixPtr)
{

  FunctionalState retTHRESHOLD = ENABLE ;
  /* K=(X0-X2) (Y1-Y2)-(X1-X2) (Y0-Y2) */
  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  if( matrixPtr->Divider == 0 )
  {
    retTHRESHOLD = DISABLE;
  }
  else
  {
    /* A=((XD0-XD2) (Y1-Y2)-(XD1-XD2) (Y0-Y2))/K  */
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  /* B=((X0-X2) (XD1-XD2)-(XD0-XD2) (X1-X2))/K  */
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* C=(Y0(X2XD1-X1XD2)+Y1(X0XD2-X2XD0)+Y2(X1XD0-X0XD1))/K */
    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
    /* D=((YD0-YD2) (Y1-Y2)-(YD1-YD2) (Y0-Y2))/K  */
    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    /* E=((X0-X2) (YD1-YD2)-(YD0-YD2) (X1-X2))/K  */
    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* F=(Y0(X2YD1-X1YD2)+Y1(X0YD2-X2YD0)+Y2(X1YD0-X0YD1))/K */
    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
  }
  return( retTHRESHOLD ) ;
}

/*******************************************************************************
* Function Name  : getDisplayPoint
* Description    : Touch panel X Y to display X Y
* Input          : None
* Output         : None
* Return         : 
* Attention      : None
*******************************************************************************/
FunctionalState getDisplayPoint(Coordinate * displayPtr,
                     Coordinate * screenPtr,
                     Matrix * matrixPtr )
{
  FunctionalState retTHRESHOLD =ENABLE ;
  /*
  An=168
  */
  if( matrixPtr->Divider != 0 )
  {
    /* XD = AX+BY+C */        
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                      (matrixPtr->Bn * screenPtr->y) + 
                       matrixPtr->Cn 
                    ) / matrixPtr->Divider ;
    /* YD = DX+EY+F */        
    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                      (matrixPtr->En * screenPtr->y) + 
                       matrixPtr->Fn 
                    ) / matrixPtr->Divider ;
  }
  else
  {
    retTHRESHOLD = DISABLE;
  }
  return(retTHRESHOLD);
} 


/*******************************************************************************
* Function Name  : TouchPanel_Calibrate
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void TouchPanel_Calibrate(void)
{
  uint8_t i;
  Coordinate * Ptr;

  for(i=0;i<3;i++)
  {
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE); 
    BSP_LCD_DisplayStringAtLine(1, (uint8_t*)"        Touch crosshair to calibrate");
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_DisplayStringAtLine(5, (uint8_t*)"   Waveshare LCD");
    HAL_Delay(200);
    DrawCross(DisplaySample[i].x,DisplaySample[i].y);
    do
    {
      Ptr=Read_Ads7846();
    }
    while( Ptr == (void*)0 );
    ScreenSample[i].x= Ptr->x; ScreenSample[i].y= Ptr->y;
  }
  setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix );
  BSP_LCD_Clear(LCD_COLOR_BLACK);
} 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
