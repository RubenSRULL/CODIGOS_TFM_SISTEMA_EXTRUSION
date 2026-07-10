/*
 * wrapper_fsm.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

// =================================== //
// Librerias
// =================================== // 
#include "winder_fsm.h"
#include "WINDER_FSM.h"
#include "process_data.h"
#include "safety.h"

// =================================== //
// Variables privadas
// =================================== //
static bool cmd_home = false;
static bool cmd_run = false;
static bool cmd_stop = false;
static bool cmd_reset = false;
static bool init_ok = false;
static bool home_ok = false;

// -- Inicializacion de la FSM
void WinderFSM_Init(void)
{
    WINDER_FSM_initialize();

    cmd_home = false;
    cmd_run = false;
    cmd_stop = false;
    cmd_reset = false;

    init_ok = true;
    home_ok = false;
}

// -- Actualizacion de la FSM
void WinderFSM_Update(void)
{
    WINDER_FSM_U.init_ok = init_ok;

    WINDER_FSM_U.home_cmd = cmd_home;
    WINDER_FSM_U.run_cmd = cmd_run;
    WINDER_FSM_U.stop_cmd = cmd_stop;
    WINDER_FSM_U.reset_ok = cmd_reset;

    WINDER_FSM_U.home_ok = home_ok ? 1.0 : 0.0;
    WINDER_FSM_U.fault_cmd = Safety_HasFault();

    WINDER_FSM_step();

    ProcessData_SetWinderState(WINDER_FSM_Y.state_id);

    ProcessData_SetFSMOutputs(
        WINDER_FSM_Y.guide_enable,
        WINDER_FSM_Y.winder_enable,
        WINDER_FSM_Y.alarm_active
    );

    cmd_home = false;
    cmd_run = false;
    cmd_stop = false;
    cmd_reset = false;
}

// -- Comandos de la FSM
void WinderFSM_CommandHome(void)
{
	home_ok = true;
	cmd_home = true;
}

void WinderFSM_CommandRun(void)
{
    cmd_run = true;
}

void WinderFSM_CommandStop(void)
{
    cmd_stop = true;
}

void WinderFSM_CommandReset(void)
{
    cmd_reset = true;
}

void WinderFSM_SetInitOk(bool ok)
{
    init_ok = ok;
}

void WinderFSM_SetHomeOk(bool ok)
{
    home_ok = ok;
}

uint8_t WinderFSM_GetState(void)
{
    return WINDER_FSM_Y.state_id;
}

bool WinderFSM_IsGuideEnabled(void)
{
    return WINDER_FSM_Y.guide_enable;
}

bool WinderFSM_IsWinderEnabled(void)
{
    return WINDER_FSM_Y.winder_enable;
}

bool WinderFSM_IsAlarmActive(void)
{
    return WINDER_FSM_Y.alarm_active;
}
