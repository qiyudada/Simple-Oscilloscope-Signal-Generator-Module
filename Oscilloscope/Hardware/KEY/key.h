#ifndef __KEY_H
#define __KEY_H

#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"

enum
{
	KEY1 = 1,
	KEY2 = 2,
	KEY3 = 3,
	KEYD = 4,
	KEYA = 5,
	KEYB = 6,
};

void Init_Key_GPIO(void);
void Key_Handle(volatile struct Oscilloscope *value);
void Init_EC11_GPIO(void);

#endif