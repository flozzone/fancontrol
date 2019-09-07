/***********************************************************************************************************************
  | file       : OLED_Show
  | version    : V1.0
  | date       : 2017-12-12
  | function  :
  Brush screen use note: if using the internal RAM, RAM is small at the same time, in OLED_Driver.h
  to change the XByte and YByte value, these remarks related BYTE corresponding to the number
  of pixels, the corresponding changes.
  It is important to note that the X and Y values of the display function in the GUI should not
  exceed the display area specified above, otherwise the screen will be displayed.
  For the controller's RAM enough, it is recommended that the RAM be set to the maximum pixel
  of the screen to correspond, so that the control function is easy to use.
  For the controller's RAM, it is recommended to use external RAM, which is convenient for
  the controller, for example, this routine has 23K256 as the external RAM control.
***********************************************************************************************************************/
#include "OLED_Driver.h"
#include "OLED_GUI.h"
#include "DEV_Config.h"
#include "Show_Lib.h"
#include "Debug.h"

extern OLED_DIS sOLED_DIS;
void setup()
{
  System_Init();

  Serial.println("OLED Init ");
  OLED_SCAN_DIR OLED_ScanDir = SCAN_DIR_DFT;
  OLED_Init( OLED_ScanDir );

  Serial.println("OLED_Show ");
  GUI_Show();

  Serial.println("OLED Clear ");
  OLED_ClearScreen(OLED_BACKGROUND);//OLED_BACKGROUND

  Serial.print("Show Pic\r\n");
  GUI_Disbitmap(0  , 2, Signal816  , 16, 8);
  GUI_Disbitmap(24 , 2, Bluetooth88, 8 , 8);
  GUI_Disbitmap(40 , 2, Msg816     , 16, 8);
  GUI_Disbitmap(64 , 2, GPRS88     , 8 , 8);
  GUI_Disbitmap(90 , 2, Alarm88    , 8 , 8);
  GUI_Disbitmap(112, 2, Bat816     , 16, 8);

  Serial.print("Show 16 Gray Map\r\n");
  GUI_DisGrayMap(0, 73, gImage_flower);

  GUI_DisString_EN(0 , 52, "MUSIC", &Font12, FONT_BACKGROUND, WHITE);
  GUI_DisString_EN(48, 52, "MENU" , &Font12, FONT_BACKGROUND, WHITE);
  GUI_DisString_EN(90, 52, "PHONE", &Font12, FONT_BACKGROUND, WHITE);

  OLED_Display(0, 0, sOLED_DIS.OLED_Dis_Column, sOLED_DIS.OLED_Dis_Page);

}

void loop()
{
  uint8_t sec = 0;
  DEV_TIME sDev_time;
  sDev_time.Hour = 12;
  sDev_time.Min = 34;
  sDev_time.Sec = 56;
  Serial.print("Show time\r\n");
  for (;;) {
    sec++;
    sDev_time.Sec = sec;
    if (sec == 60) {
      sDev_time.Min = sDev_time.Min + 1;
      sec = 0;
      if (sDev_time.Min == 60) {
        sDev_time.Hour =  sDev_time.Hour + 1;
        sDev_time.Min = 0;
        if (sDev_time.Hour == 24) {
          sDev_time.Hour = 0;
          sDev_time.Min = 0;
          sDev_time.Sec = 0;
        }
      }
    }
    GUI_Showtime(0, 22, 127, 47, &sDev_time, WHITE);
    Driver_Delay_ms(1000);//Analog clock 1s
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

