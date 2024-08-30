#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "led.h"
#include "main.h"
#include "tft_init.h"
#include "tft.h"
#include "ec11.h"
#include "brick.h"
#include "oscilloscope.h"
#include "spectrometer.h"

#define TRUE 1
#define FALSE 0

#define OSCILLO 0 // 示波器
#define BRICK 1   // 打砖块
#define SPECTRO 2 // 频谱

// from tft_init.c  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏
extern int USE_HORIZONTAL;

int modeSelected = OSCILLO;
int isClicked = FALSE;

// 绘制主屏幕选项
void DrawItem(int i)
{
    TFT_Fill(0, 0, 20, 128, BLACK);

    TFT_Fill(2, 20 + 5 + i * 36, 10, 20 + 11 + i * 36, RED);

    char showData[32] = {0};
    sprintf(showData, "  OSCILLOSCOPE  ");
    TFT_ShowString(20, 20, (uint8_t *)showData, PURPLE, LIGHTBLUE, 16, 0);
    memset(showData, 0, 32);

    sprintf(showData, "  BRICKBREAKER  ");
    TFT_ShowString(20, 56, (uint8_t *)showData, PURPLE, LIGHTBLUE, 16, 0);
    memset(showData, 0, 32);

    sprintf(showData, "  SPECTROMETER  ");
    TFT_ShowString(20, 92, (uint8_t *)showData, PURPLE, LIGHTBLUE, 16, 0);
    memset(showData, 0, 32);
}

// 旋转旋钮 选择模式
void OnEc11RotateMainMenu(int direct)
{
    modeSelected = (modeSelected + direct + 3) % 3;
    DrawItem(modeSelected);
}

// 确定按钮 确认模式
void OnEc11ClickMainMenu(void)
{
    isClicked = TRUE;
    Open_LED(1);
}

// 入口
int main(void)
{
    //	初始化滴答定时器
    systick_config();

    // 初始化LED引脚
    Init_LED_GPIO();

    // 设置横屏
    USE_HORIZONTAL = 2;

    // 初始化TFT屏幕引脚及默认配置
    TFT_Init();

    // TFT 填充黑色为背景色
    TFT_Fill(0, 0, 160, 128, BLACK);

    // 初始化EC11引脚
    Init_EC11_GPIO();
    Init_EC11_EXITGPIO();

    Set_EC11_Rotate_Handler(OnEc11RotateMainMenu);
    Set_EC11_Click_Handler(OnEc11ClickMainMenu);

    Close_LED(1);
    Close_LED(2);

    // 绘制选项
    DrawItem(modeSelected);

    while (!isClicked)
    {
        delay_1ms(10);
    }

    Set_EC11_Rotate_Handler(NULL);
    Set_EC11_Click_Handler(NULL);

    switch (modeSelected)
    {
    case OSCILLO: // 示波器
    {
        InitOscillo();
        RunOscillo();
    }
    break;
    case BRICK: // 砖块
    {
        InitGame();
        RunGame();
    }
    break;
    case SPECTRO: // 频谱
        InitSpectro();
        RunSpectro();
        break;
    default:
        break;
    }
}
