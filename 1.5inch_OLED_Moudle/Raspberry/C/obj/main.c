#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()

#include "OLED_Driver.h"
#include "OLED_GUI.h"
#include "DEV_Config.h"
#include <time.h>

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_ModuleExit();

    exit(0);
}

int main(void)
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
    
    printf("1.5inch OLED Moudle Demo\r\n");
	//1.System Initialization
	if(DEV_ModuleInit())
		exit(0);
	
	//2.show
	printf("**********Init OLED**********\r\n");
	OLED_SCAN_DIR OLED_ScanDir = SCAN_DIR_DFT;//SCAN_DIR_DFT = D2U_L2R
	OLED_Init(OLED_ScanDir );	
	
	printf("OLED Show \r\n");
	GUI_Show();
	
	OLED_Clear(OLED_BACKGROUND);//OLED_BACKGROUND
	OLED_Display();
	
	printf("Show Pic\r\n");
	GUI_Disbitmap(0  , 2, Signal816  , 16, 8);
	GUI_Disbitmap(24 , 2, Bluetooth88, 8 , 8);
	GUI_Disbitmap(40 , 2, Msg816     , 16, 8);
	GUI_Disbitmap(64 , 2, GPRS88     , 8 , 8);
	GUI_Disbitmap(90 , 2, Alarm88    , 8 , 8);
	GUI_Disbitmap(112, 2, Bat816     , 16, 8);

	printf("Show 16 Gray Map\r\n");
	GUI_DisGrayMap(0,73,gImage_flower);

	GUI_DisString_EN(0 , 52, "MUSIC", &Font12, FONT_BACKGROUND, WHITE); 
	GUI_DisString_EN(48, 52, "MENU" , &Font12, FONT_BACKGROUND, WHITE); 
	GUI_DisString_EN(90, 52, "PHONE", &Font12, FONT_BACKGROUND, WHITE);
	OLED_Display();
	
	printf("Show time\r\n");
	time_t now;
	struct tm *timenow;
	uint8_t now_time,new_time = 11;
	DEV_TIME sDev_time;
	while(1){
		time(&now);
		timenow = localtime(&now);
		 
		sDev_time.Hour = timenow->tm_hour;
		sDev_time.Min = timenow->tm_min;
		sDev_time.Sec = timenow->tm_sec;
		now_time = sDev_time.Sec;
		
		OLED_ClearWindow(0, 22, 127, 47, WHITE);
		GUI_Showtime(0, 22, 127, 47, &sDev_time, WHITE);
		if(now_time != new_time){
			OLED_DisWindow(0, 22, 127, 47);	
			new_time = now_time;
		}
	}
	
	//3.System Exit
	DEV_ModuleExit();
	return 0;
	
}

