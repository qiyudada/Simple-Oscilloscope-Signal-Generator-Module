#ifndef __EC11_H
#define __EC11_H

#include "gd32e23x.h"
#include "systick.h"
#include "key.h"
#include <stdio.h>


typedef void (*Ec11RotateHandler) (int);
typedef void (*Ec11ClickHandler) (void);

typedef void (*Key1ClickHander) (void);
typedef void (*Key2ClickHander) (void);
typedef void (*Key3ClickHander) (void);

void Init_EC11_GPIO(void);
void Init_EC11_EXITGPIO(void);

void Set_EC11_Rotate_Handler(Ec11RotateHandler handler);
void Set_EC11_Click_Handler(Ec11ClickHandler handler);

void Set_Key1_Click_Handler(Key1ClickHander handler);
void Set_Key2_Click_Handler(Key2ClickHander handler);
void Set_Key3_Click_Handler(Key3ClickHander handler);

#endif