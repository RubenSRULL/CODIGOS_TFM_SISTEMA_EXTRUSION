/*
 * edu301d_driver.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */


/*
Cálculo de la frecuencia de pulsos para el motor paso a paso del EDU301D:
    - ARR: Valor del registro de auto-recarga del timer (Registro de periodo).
    - CCR: Valor del registro de comparación del timer (Registro de ancho de pulso).

-> Ejemplo: rpm = 10.0, gear ratio = 2.25, steps per rev = 200, timer clk = 170 MHz
    -> rpm_motor = 10.0 / 2.25 = 22.22
    -> pulse_hz = 22.22 * 200 / 60 = 148.15
    -> arr = 170000000 / 148.15 - 1 = 114799 (esta division significa que el timer cuenta hasta 114799 tics antes de reiniciarse, lo que da un periodo de 1/148.15 s)
    -> ccr = arr / 2 = 57399 (para un ciclo de trabajo del 50%, ya que queremos pulsos cuadrados para el motor paso a paso, pues el motor necesita pulsos de ancho constante para funcionar correctamente)
*/

 /* =================================== */
 /* Librerías                           */
 /* =================================== */
#include "edu301d_driver.h"
#include "main.h"


/* Timer  para la señal de pulsos al EDU301D. */
extern TIM_HandleTypeDef htim2;

/* Estado interno del driver. */
static bool edu301d_running = false;
static uint32_t edu301d_last_arr = 0U;


static float EDU301D_LimitRpm(float rpm);
static uint32_t EDU301D_RpmToArr(float rpm);


/* Inicialización del driver EDU301D. */
void EDU301D_Init(void)
{
    /* Arranca PWM; con CCR=0 no hay pulsos. */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0U);
    __HAL_TIM_SET_COUNTER(&htim2, 0U);

    edu301d_running = false;
    edu301d_last_arr = 0U;
}

/* Detiene el driver EDU301D.
    - Detiene la generación de pulsos. */
void EDU301D_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0U);
    __HAL_TIM_SET_COUNTER(&htim2, 0U);

    edu301d_running = false;
    edu301d_last_arr = 0U;
}

/* Configura la velocidad del motor EDU301D.
    - rpm: Velocidad en revoluciones por minuto. */
void EDU301D_SetRpm(float rpm)
{
    uint32_t arr;
    uint32_t ccr;

    rpm = EDU301D_LimitRpm(rpm);

    if (rpm <= 0.0f)
    {
        EDU301D_Stop();
        return;
    }

    /* Calcula ARR y CCR para la velocidad deseada */
    arr = EDU301D_RpmToArr(rpm);
    ccr = arr / 2U;


    /* Si ya está girando a la misma frecuencia, no reiniciamos el timer para evitar pequeños cortes */
    /* Mejora propuesta por IA */
    if ((edu301d_running == true) && (arr == edu301d_last_arr))
    {
        return;
    }

    /* Desactiva el timer antes de cambiar ARR y CCR */
    __HAL_TIM_DISABLE(&htim2);
    /* Carga el nuevo valor de ARR */
    __HAL_TIM_SET_AUTORELOAD(&htim2, arr);
    /* Carga el nuevo valor de CCR */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ccr);
    /* Reinicia el contador del timer */
    __HAL_TIM_SET_COUNTER(&htim2, 0U);
    /* Genera un evento de actualización que se activara cuando se complete el contador */
    HAL_TIM_GenerateEvent(&htim2, TIM_EVENTSOURCE_UPDATE);
    /* Activa el timer */
    __HAL_TIM_ENABLE(&htim2);
    /* Actualiza el estado interno */
    edu301d_running = true;
    edu301d_last_arr = arr;
}

/* Devuelve true si el driver EDU301D está generando pulsos */
bool EDU301D_IsRunning(void)
{
    return edu301d_running;
}

/* Limita la velocidad del motor EDU301D */
static float EDU301D_LimitRpm(float rpm)
{
    if (rpm < 0.0f)
    {
        return 0.0f;
    }

    if (rpm > EDU301D_RPM_MAX)
    {
        return EDU301D_RPM_MAX;
    }

    return rpm;
}

/* Convierte RPM a valor de ARR para el timer */
static uint32_t EDU301D_RpmToArr(float rpm)
{
    float rpm_motor;
    float pulse_hz;
    float arr_f;

    /* Calcula la velocidad del motor considerando la reductora */
    rpm_motor = rpm * EDU301D_GEAR_RATIO;

    /* Pulsos/s = rpm_motor * pulsos_por_vuelta / 60 */
    pulse_hz = (rpm_motor * EDU301D_STEPS_PER_REV) / 60.0f;

    /* Calcula el valor de ARR para el timer */
    arr_f = (EDU301D_TIMER_CLK_HZ / pulse_hz) - 1.0f;

    /* Limita el valor de ARR */
    if (arr_f < 20.0f)
    {
        arr_f = 20.0f;
    }

    if (arr_f > 4294967295.0f)
    {
        arr_f = 4294967295.0f;
    }

    return (uint32_t)arr_f;
}
