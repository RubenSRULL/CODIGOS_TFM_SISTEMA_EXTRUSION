/*
 * motor_control.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

// ============================== //
// Librerias                      //
// ============================== //
#include "motor_control.h"
#include "edu301d_driver.h"
#include "process_data.h"
#include "temperature_control.h"
#include <stdbool.h>

// ============================== //
// Variables globales               //
// ============================== //
static bool melt_ready_latched = false;                                 // Variable para almacenar el estado de melt_ready_latched

// ============================== //
// Prototipos de funciones privadas //
// ============================== //
static bool MotorControl_IsMeltReady(float temp_c, bool sensor_ok);

// -- Inicialización del controlador de motor
void MotorControl_Init(void)
{
    EDU301D_Init();
    EDU301D_Stop();
}

// -- Actualización del controlador de motor
void MotorControl_Update(void)
{
    ProcessData_t data;

    ProcessData_GetSnapshot(&data);

    if (data.rpm_int_setpoint > 0.0f)
    {
        EDU301D_SetRpm(data.rpm_int_setpoint);
    }
    else
    {
        EDU301D_Stop();
    }
}

// -- Verifica si el motor está listo para fundir
// - Esta funcion no se está utilizando actualmente, pero se deja para futuras implementaciones.
static bool MotorControl_IsMeltReady(float temp_c, bool sensor_ok)
{
    if (sensor_ok == false)
    {
        melt_ready_latched = false;
        return false;
    }

    /* Habilita al superar el umbral de temperatura */
    if (melt_ready_latched == false)
    {
        if (temp_c >= MOTOR_CONTROL_MIN_MELT_TEMP_C)
        {
            melt_ready_latched = true;
        }
    }
    /* Deshabilita por debajo del umbral menos histéresis. */
    else
    {
        if (temp_c <= (MOTOR_CONTROL_MIN_MELT_TEMP_C - MOTOR_CONTROL_TEMP_HYST_C))
        {
            melt_ready_latched = false;
        }
    }

    return melt_ready_latched;
}
