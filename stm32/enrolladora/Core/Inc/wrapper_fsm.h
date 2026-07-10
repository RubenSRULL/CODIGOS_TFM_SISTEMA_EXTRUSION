/*
 * wrapper_fsm.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_WRAPPER_FSM_H_
#define INC_WRAPPER_FSM_H_

// ==================== //
// Librerias
// ==================== //
#include <stdint.h>
#include <stdbool.h>

// ==================== //
// Prototipos de funciones
// ==================== //
void WinderFSM_Init(void);
void WinderFSM_Update(void);

void WinderFSM_CommandHome(void);
void WinderFSM_CommandRun(void);
void WinderFSM_CommandStop(void);
void WinderFSM_CommandReset(void);

void WinderFSM_SetInitOk(bool ok);
void WinderFSM_SetHomeOk(bool ok);

uint8_t WinderFSM_GetState(void);

bool WinderFSM_IsGuideEnabled(void);
bool WinderFSM_IsWinderEnabled(void);
bool WinderFSM_IsAlarmActive(void);

#endif /* INC_WRAPPER_FSM_H_ */
