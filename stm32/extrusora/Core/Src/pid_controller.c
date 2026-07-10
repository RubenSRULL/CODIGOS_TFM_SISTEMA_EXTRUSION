/*
 * pid_controller.c
 *
 *  Created on: 24 jun 2026
 *      Author: rsahu
 */

/* ========================================================= */
/* Librerías                                                 */ 
/* ========================================================= */
#include "pid_controller.h"

/* Función para calcular el valor absoluto de un número flotante */
static float abs_float(float x)
{
    return (x < 0.0f) ? -x : x;
}

/* Función para limitar un valor flotante entre dos límites */
static float clamp_float(float value, float min_value, float max_value)
{
    if (value > max_value)
    {
        return max_value;
    }

    if (value < min_value)
    {
        return min_value;
    }

    return value;
}

/* Funciones públicas */

/* Inicialización PID.
   - Asigna valores por defecto a los parámetros del controlador */
void PID_Init(PIDController_t *pid)
{
	if (pid == NULL)
    {
        return;
    }

    pid->measurement = 0.0f;
    pid->setpoint = 0.0f;
    pid->output = 0.0f;

    pid->error = 0.0f;
    pid->previous_error = 0.0f;

    pid->proportional_term = 0.0f;
    pid->integral_term = 0.0f;
    pid->derivative_term = 0.0f;

    pid->output_min = 0.0f;
    pid->output_max = 1.0f;

    pid->kp = 0.0f;
    pid->ki = 0.0f;
    pid->kd = 0.0f;

    pid->sample_time_s = 0.1f;

    pid->deadband = 0.0f;
    pid->output_rate_limit = 0.0f;
    pid->derivative_filter_N = 0.0f;

    pid->anti_windup_enabled = true;
}

/* Configuración básica del PID
   - Asigna valores básicos a los parámetros del controlador */
void PID_ConfigureBasic(
    PIDController_t *pid,
    float kp,
    float ki,
    float kd,
    float output_min,
    float output_max,
    float sample_time_s
)
{
	if (pid == NULL)
    {
        return;
    }

    if (output_max < output_min)
    {
        return;
    }

    if (sample_time_s <= 0.0f)
    {
        return;
    }

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->output_min = output_min;
    pid->output_max = output_max;

    pid->sample_time_s = sample_time_s;

    pid->output = clamp_float(
        pid->output,
        pid->output_min,
        pid->output_max
    );
}

/* Configuración avanzada del PID
   - Asigna valores avanzados a los parámetros del controlador */
void PID_ConfigureAdvanced(
    PIDController_t *pid,
    float deadband,
    float output_rate_limit,
    float derivative_filter_N,
    bool anti_windup_enabled
)
{
	if (pid == NULL)
    {
        return;
    }

    if (deadband < 0.0f)
    {
        deadband = 0.0f;
    }

    if (output_rate_limit < 0.0f)
    {
        output_rate_limit = 0.0f;
    }

    if (derivative_filter_N < 0.0f)
    {
        derivative_filter_N = 0.0f;
    }

    pid->deadband = deadband;
    pid->output_rate_limit = output_rate_limit;
    pid->derivative_filter_N = derivative_filter_N;
    pid->anti_windup_enabled = anti_windup_enabled;
}

/* Actualización del PID
   - Calcula la nueva salida del controlador */
float PID_Update(
    PIDController_t *pid,
    float setpoint,
    float measurement,
    float dt_s
)
{
    float error;
    float derivative;
    float integral_aux;
    float output_aux;
    float output;
    float max_output_delta;
    float output_delta;
    bool allow_integral = true;

    if (pid == NULL)
    {
        return 0.0f;
    }

    /* Verificar tiempo de muestreo introducido */
    if (dt_s <= 0.0f)
    {
        if (pid->sample_time_s <= 0.0f)
        {
            return pid->output;
        }
        dt_s = pid->sample_time_s;
    }

    /* Actualizar valores PID*/
    pid->setpoint = setpoint;
    pid->measurement = measurement;
    error = pid->setpoint - pid->measurement;

    /* Calcular error */
    if (abs_float(error) <= pid->deadband)
    {
        pid->error = 0.0f;
    }
    else
    {
        pid->error = error;
    }

    /* Termino proporcional */
    pid->proportional_term = pid->kp * pid->error;
    /* Término integral */
    integral_aux =
        pid->integral_term +
        (pid->ki * pid->error * dt_s);
    /* Termino derivativo */
    derivative = (pid->error - pid->previous_error) / dt_s;


    /* Si el filtro derivativo está habilitado, aplicarlo */
    if (pid->derivative_filter_N > 0.0f)
    {
        float alpha;

        alpha =
            (pid->derivative_filter_N * dt_s) /
            (1.0f + (pid->derivative_filter_N * dt_s));

        pid->derivative_term =
            pid->derivative_term +
            alpha * (derivative - pid->derivative_term);
    }
    else
    {
        pid->derivative_term = derivative;
    }

    /* Calcular Salida */
    output_aux =
        pid->proportional_term +
        integral_aux +
        (pid->kd * pid->derivative_term);


    /* Si antiwindup activo */
    if (pid->anti_windup_enabled)
    {
        /* Si la salida auxiliar excede los límites y el error es positivo, deshabilitar la integral */
        if ((output_aux > pid->output_max) &&
            (pid->error > 0.0f))
        {
            allow_integral = false;
        }

        /* Si la salida auxiliar es menor que el mínimo y el error es negativo, deshabilitar la integral */
        if ((output_aux < pid->output_min) &&
            (pid->error < 0.0f))
        {
            allow_integral = false;
        }
    }

    /* Si se permite la integral, calcular término integral */
    if (allow_integral)
    {
        pid->integral_term = integral_aux;
    }

    /* Calcular salida final a partir de los términos PID */
    output_aux =
        pid->proportional_term +
        pid->integral_term +
        (pid->kd * pid->derivative_term);

    /* Limitar la salida final */
    output = clamp_float(
        output_aux,
        pid->output_min,
        pid->output_max
    );

    /* Limitador velocidad de salida si está habilitado */
    if (pid->output_rate_limit > 0.0f)
    {   
        /* Calcular el cambio máximo permitido en la salida */
        max_output_delta = pid->output_rate_limit * dt_s;
        output_delta = output - pid->output;

        if (output_delta > max_output_delta)
        {
        	output = pid->output + max_output_delta;
        }
        else if (output_delta < -max_output_delta)
        {
        	output = pid->output - max_output_delta;
        }
    }

    /* Limitar la salida final */
    pid->output = clamp_float(
    	output,
        pid->output_min,
        pid->output_max
    );

    pid->previous_error = pid->error;

    return pid->output;
}

/* Reiniciar el controlador PID */
void PID_Reset(PIDController_t *pid)
{
	if (pid == NULL)
    {
        return;
    }

    pid->measurement = 0.0f;
    pid->setpoint = 0.0f;

    pid->output = clamp_float(
        0.0f,
        pid->output_min,
        pid->output_max
    );

    pid->error = 0.0f;
    pid->previous_error = 0.0f;

    pid->proportional_term = 0.0f;
    pid->integral_term = 0.0f;
    pid->derivative_term = 0.0f;
}