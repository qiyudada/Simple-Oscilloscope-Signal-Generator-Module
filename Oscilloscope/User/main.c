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

#define OSCILLO 0 // ʾ����
#define BRICK 1   // ��ש��
#define SPECTRO 2 // Ƶ��

// from tft_init.c  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����
extern int USE_HORIZONTAL;

int modeSelected = OSCILLO;
int isClicked = FALSE;

// ��������Ļѡ��
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

// ��ת��ť ѡ��ģʽ
void OnEc11RotateMainMenu(int direct)
{
    modeSelected = (modeSelected + direct + 3) % 3;
    DrawItem(modeSelected);
}

// ȷ����ť ȷ��ģʽ
void OnEc11ClickMainMenu(void)
{
    isClicked = TRUE;
    Open_LED(1);
}

// ���
int main(void)
{
    //	��ʼ���δ�ʱ��
    systick_config();

    // ��ʼ��LED����
    Init_LED_GPIO();

    // ���ú���
    USE_HORIZONTAL = 2;

    // ��ʼ��TFT��Ļ���ż�Ĭ������
    TFT_Init();

    // TFT ����ɫΪ����ɫ
    TFT_Fill(0, 0, 160, 128, BLACK);

    // ��ʼ��EC11����
    Init_EC11_GPIO();
    Init_EC11_EXITGPIO();

    Set_EC11_Rotate_Handler(OnEc11RotateMainMenu);
    Set_EC11_Click_Handler(OnEc11ClickMainMenu);

    Close_LED(1);
    Close_LED(2);

    // ����ѡ��
    DrawItem(modeSelected);

    while (!isClicked)
    {
        delay_1ms(10);
    }

    Set_EC11_Rotate_Handler(NULL);
    Set_EC11_Click_Handler(NULL);

    switch (modeSelected)
    {
    case OSCILLO: // ʾ����
    {
        InitOscillo();
        RunOscillo();
    }
    break;
    case BRICK: // ש��
    {
        InitGame();
        RunGame();
    }
    break;
    case SPECTRO: // Ƶ��
        InitSpectro();
        RunSpectro();
        break;
    default:
        break;
    }
}
