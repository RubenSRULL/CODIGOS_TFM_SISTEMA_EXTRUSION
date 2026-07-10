/*
 * pid_controller.h
 *
 *  Created on: 24 jun 2026
 *      Author: rsahu
 */

#ifndef INC_PID_CONTROLLER_H_
#define INC_PID_CONTROLLER_H_

/* ========================================================= */
/* Librerías                                                 */
/* ========================================================= */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ========================================================= */
/* Estructura para el controlador PID                        */
/* ========================================================= */
typedef struct
{
    /* Entradas y salidas */
    float measurement;
    float setpoint;
    float output;

    /* Errores */
    float error;
    float previous_error;

    /* Terminos PID*/
    float proportional_term;
    float integral_term;
    float derivative_term;

    /* Límites de salida */
    float output_min;
    float output_max;

    /* Ganancias PID */
    float kp;
    float ki;
    float kd;

    /* Tiempo de muestreo para el controlador PID */
    float sample_time_s;

    /* Deadband */
    float deadband;

    /* Límite velocidad salida */
    float output_rate_limit;

    /* Filtro derivativo */
    float derivative_filter_N;

    /* Anti-windup */
    bool anti_windup_enabled;

} PIDController_t;


/* Inicialización PID */
void PID_Init(PIDController_t *pid);

/* Configuracion básica PID*/
void PID_ConfigureBasic(
    PIDController_t *pid,
    float kp,
    float ki,
    float kd,
    float output_min,
    float output_max,
    float sample_time_s
);

/* Configuracion avanzada PID */
void PID_ConfigureAdvanced(
    PIDController_t *pid,
    float deadband,
    float output_rate_limit,
    float derivative_filter_N,
    bool anti_windup_enabled
);

/* Actualizacion PID */
float PID_Update(
    PIDController_t *pid,
    float setpoint,
    float measurement,
    float dt_s
);

/* Reseteo PID */
void PID_Reset(PIDController_t *pid);

#endif /* INC_PID_CONTROLLER_H_ */
