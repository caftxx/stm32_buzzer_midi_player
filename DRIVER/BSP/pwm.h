#ifndef __PWM_H
#define __PWM_H

#include <stdint.h>

void PWM_Init(void);
void PWM_SetCompare1(uint8_t No, uint16_t Compare);
void PWM_SetAutoreload(uint8_t No, uint16_t Autoreload);

#endif
