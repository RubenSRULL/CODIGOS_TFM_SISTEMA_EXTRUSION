/*
 * wrapper_fsm.h
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

#ifndef INC_WRAPPER_FSM_H_
#define INC_WRAPPER_FSM_H_

// ======================== //
// Librerias
// ======================== //
#include <stdint.h>
#include <stdbool.h>

// ======================== //
// Funciones del FSM
// ======================== //
void ExtruderFSM_Init(void);
void ExtruderFSM_Update(void);

void ExtruderFSM_CommandAuto(void);
void ExtruderFSM_CommandManual(void);
void ExtruderFSM_CommandStop(void);
void ExtruderFSM_CommandPreheat(void);
void ExtruderFSM_CommandStart(void);
void ExtruderFSM_CommandReset(void);

void ExtruderFSM_SetInitOk(bool ok);
void ExtruderFSM_SetExtrudeOk(bool ok);
void ExtruderFSM_SetStopOk(bool ok);
void ExtruderFSM_SetFinishOk(bool ok);

uint8_t ExtruderFSM_GetState(void);

bool ExtruderFSM_IsHeaterEnabled(void);
bool ExtruderFSM_IsExtruderEnabled(void);
bool ExtruderFSM_IsWinderEnabled(void);
bool ExtruderFSM_IsManualEnabled(void);
bool ExtruderFSM_IsAlarmActive(void);

#endif /* INC_WRAPPER_FSM_H_ */
