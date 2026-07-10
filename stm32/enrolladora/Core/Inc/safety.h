/*
 * safety.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_SAFETY_H_
#define INC_SAFETY_H_

#include <stdint.h>
#include <stdbool.h>

#define WINDER_ALARM_NONE              0x00000000UL
#define WINDER_ALARM_DRIVER_FAULT      0x00000001UL
#define WINDER_ALARM_LIMIT_SWITCH      0x00000002UL
#define WINDER_ALARM_HOME_TIMEOUT      0x00000004UL
#define WINDER_ALARM_COMM_TIMEOUT      0x00000008UL
#define WINDER_ALARM_EMERGENCY_STOP    0x00000010UL

void Safety_Init(void);
void Safety_Update(void);

bool Safety_HasFault(void);
uint32_t Safety_GetAlarmFlags(void);

void Safety_SetFault(uint32_t flags);
void Safety_ClearFault(uint32_t flags);
void Safety_ClearAll(void);

#endif /* INC_SAFETY_H_ */
