/*
 * wrapper_fsm.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

// ========================== //
// Librerias
// ========================== //
#include "wrapper_fsm.h"
#include "EXTRUDER_FSM.h"
#include "process_data.h"
#include "safety.h"

// ========================== //
// Variables privadas
// ========================== //
static bool cmd_auto = false;
static bool cmd_manual = false;
static bool cmd_stop = false;
static bool cmd_preheat = false;
static bool cmd_start = false;
static bool cmd_reset = false;

static bool init_ok = true;
static bool extrude_ok = true;
static bool stop_ok = false;
static bool finish_ok = false;

// ========================== //
// Funciones publicas
// ========================== //
void ExtruderFSM_Init(void)
{
    EXTRUDER_FSM_initialize();

    cmd_auto = false;
    cmd_manual = false;
    cmd_stop = false;
    cmd_preheat = false;
    cmd_start = false;
    cmd_reset = false;

    init_ok = true;
    extrude_ok = true;
    stop_ok = false;
    finish_ok = false;
}

void ExtruderFSM_Update(void)
{
    EXTRUDER_FSM_U.init_ok = init_ok;

    EXTRUDER_FSM_U.auto_cmd = cmd_auto;
    EXTRUDER_FSM_U.manual_cmd = cmd_manual;
    EXTRUDER_FSM_U.stop_cmd = cmd_stop;
    EXTRUDER_FSM_U.preheat_cmd = cmd_preheat;
    EXTRUDER_FSM_U.extrude_cmd = cmd_start;
    EXTRUDER_FSM_U.reset_ok = cmd_reset;

    EXTRUDER_FSM_U.stop_ok = stop_ok;
    EXTRUDER_FSM_U.extrude_ok = extrude_ok;
    EXTRUDER_FSM_U.finish_ok = finish_ok;

    EXTRUDER_FSM_U.fault = Safety_HasFault();

    EXTRUDER_FSM_step();

    ProcessData_SetExtruderState(EXTRUDER_FSM_Y.state_id);

    ProcessData_SetFSMOutputs(
        EXTRUDER_FSM_Y.heater_enable,
        EXTRUDER_FSM_Y.extruder_enable,
        EXTRUDER_FSM_Y.winder_enable,
        EXTRUDER_FSM_Y.manual_enable,
        EXTRUDER_FSM_Y.alarm_active
    );

    cmd_auto = false;
    cmd_manual = false;
    cmd_stop = false;
    cmd_preheat = false;
    cmd_start = false;
    cmd_reset = false;
}

void ExtruderFSM_CommandAuto(void)
{
    cmd_auto = true;
}

void ExtruderFSM_CommandManual(void)
{
    cmd_manual = true;
}

void ExtruderFSM_CommandStop(void)
{
    cmd_stop = true;
}

void ExtruderFSM_CommandPreheat(void)
{
    cmd_preheat = true;
}

void ExtruderFSM_CommandStart(void)
{
    cmd_start = true;
}

void ExtruderFSM_CommandReset(void)
{
    cmd_reset = true;
}

void ExtruderFSM_SetInitOk(bool ok)
{
    init_ok = ok;
}

void ExtruderFSM_SetExtrudeOk(bool ok)
{
    extrude_ok = ok;
}

void ExtruderFSM_SetStopOk(bool ok)
{
    stop_ok = ok;
}

void ExtruderFSM_SetFinishOk(bool ok)
{
    finish_ok = ok;
}

uint8_t ExtruderFSM_GetState(void)
{
    return EXTRUDER_FSM_Y.state_id;
}

bool ExtruderFSM_IsHeaterEnabled(void)
{
    return EXTRUDER_FSM_Y.heater_enable;
}

bool ExtruderFSM_IsExtruderEnabled(void)
{
    return EXTRUDER_FSM_Y.extruder_enable;
}

bool ExtruderFSM_IsWinderEnabled(void)
{
    return EXTRUDER_FSM_Y.winder_enable;
}

bool ExtruderFSM_IsManualEnabled(void)
{
    return EXTRUDER_FSM_Y.manual_enable;
}

bool ExtruderFSM_IsAlarmActive(void)
{
    return EXTRUDER_FSM_Y.alarm_active;
}
