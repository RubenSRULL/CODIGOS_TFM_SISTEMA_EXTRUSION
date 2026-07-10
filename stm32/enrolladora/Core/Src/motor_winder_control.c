/*
 * motor_winder_control.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

// ================================= //
// Librerias
// ================================= //
#include "motor_winder_control.h"
#include "tmc2208_driver.h"
#include "process_data.h"
#include "main.h"
#include <stdbool.h>

// ================================= //
// Variables externas
// ================================= //
extern TIM_HandleTypeDef htim1;

// ================================= //
// Variables privadas
// ================================= //
static TMC2208_t winder_motor;

// -- Inicializacion del motor winder --//
void MotorWinderControl_Init(void)
{
    TMC2208_Init(
        &winder_motor,
        &htim1,
        TIM_CHANNEL_1,
        M1_DIR_GPIO_Port,
        M1_DIR_Pin,
        M1_EN_GPIO_Port,
        M1_EN_Pin,
        WINDER_TIMER_CLK_HZ,
        WINDER_STEPS_PER_REV,
        WINDER_RPM_MAX,
        WINDER_TIMER_ARR_MAX,
        false
    );

    TMC2208_Stop(&winder_motor);
}

// -- Actualizacion del motor winder --//
void MotorWinderControl_Update(void)
{
    ProcessData_t data;

    ProcessData_GetSnapshot(&data);

    // Control del motor winder (habilitado, sin alarma y con rpm mayor a 0)
    if ((data.winder_enable == true) &&
        (data.alarm_active == false) &&
        (data.rpm_setpoint > 0.0f))
    {
        TMC2208_SetRpm(&winder_motor, data.rpm_setpoint);
    }
    else
    {
        TMC2208_Stop(&winder_motor);
    }
}
