/*
 * tmc2208_driver.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

/* ============================== */
/* Librerias                      */
/* ============================== */
#include "tmc2208_driver.h"

/* ============================== */
/* Funciones auxiliares           */
/* ============================== */
static float TMC2208_Abs(float x);
static float TMC2208_LimitRpm(TMC2208_t *drv, float rpm);
static uint32_t TMC2208_RpmToArr(TMC2208_t *drv, float rpm_abs);
static void TMC2208_SetDirection(TMC2208_t *drv, bool reverse);
static void TMC2208_Enable(TMC2208_t *drv, bool enable);

/* Inicialización de driver TMC2208 */
void TMC2208_Init(
    TMC2208_t *drv,
    TIM_HandleTypeDef *htim,
    uint32_t channel,
    GPIO_TypeDef *dir_port,
    uint16_t dir_pin,
    GPIO_TypeDef *en_port,
    uint16_t en_pin,
    float timer_clk_hz,
    float steps_per_rev,
    float rpm_max,
    uint32_t arr_max,
    bool invert_dir
)
{
    if (drv == NULL)
    {
        return;
    }

    drv->htim = htim;
    drv->channel = channel;

    drv->dir_port = dir_port;
    drv->dir_pin = dir_pin;

    drv->en_port = en_port;
    drv->en_pin = en_pin;

    drv->timer_clk_hz = timer_clk_hz;
    drv->steps_per_rev = steps_per_rev;
    drv->rpm_max = rpm_max;
    drv->arr_max = arr_max;

    drv->invert_dir = invert_dir;
    drv->initialized = true;
    drv->running = false;
    drv->last_arr = 0U;

    /* Arrancar el PWM */
    HAL_TIM_PWM_Start(drv->htim, drv->channel);

    /* Configurar el valor inicial del comparador y el contador a 0 */
    __HAL_TIM_SET_COMPARE(drv->htim, drv->channel, 0U);
    __HAL_TIM_SET_COUNTER(drv->htim, 0U);

    /* Deshabilitar el driver */
    TMC2208_Enable(drv, false);
}

/* Detener el motor TMC2208 */
void TMC2208_Stop(TMC2208_t *drv)
{
    if ((drv == NULL) || (drv->initialized == false))
    {
        return;
    }

    /* Configurar el valor del comparador y el contador a 0 */
    __HAL_TIM_SET_COMPARE(drv->htim, drv->channel, 0U);
    __HAL_TIM_SET_COUNTER(drv->htim, 0U);

    TMC2208_Enable(drv, false);

    drv->running = false;
    drv->last_arr = 0U;
}

/* Configurar la velocidad del motor TMC2208 */
void TMC2208_SetRpm(TMC2208_t *drv, float rpm)
{
    float rpm_abs;
    uint32_t arr;
    uint32_t ccr;
    bool reverse;

    if ((drv == NULL) || (drv->initialized == false))
    {
        return;
    }

    rpm = TMC2208_LimitRpm(drv, rpm);

    if (rpm == 0.0f)
    {
        TMC2208_Stop(drv);
        return;
    }

    reverse = (rpm < 0.0f);
    rpm_abs = TMC2208_Abs(rpm);

    arr = TMC2208_RpmToArr(drv, rpm_abs);
    ccr = arr / 2U;

    TMC2208_SetDirection(drv, reverse);
    TMC2208_Enable(drv, true);

    if ((drv->running == true) && (arr == drv->last_arr))
    {
        return;
    }

    __HAL_TIM_DISABLE(drv->htim);

    __HAL_TIM_SET_AUTORELOAD(drv->htim, arr);
    __HAL_TIM_SET_COMPARE(drv->htim, drv->channel, ccr);
    __HAL_TIM_SET_COUNTER(drv->htim, 0U);

    HAL_TIM_GenerateEvent(drv->htim, TIM_EVENTSOURCE_UPDATE);

    __HAL_TIM_ENABLE(drv->htim);

    drv->running = true;
    drv->last_arr = arr;
}

/* Verificar si el motor TMC2208 está en marcha */
bool TMC2208_IsRunning(TMC2208_t *drv)
{
    if (drv == NULL)
    {
        return false;
    }

    return drv->running;
}

/* Calcular el valor absoluto de un número flotante */
static float TMC2208_Abs(float x)
{
    return (x < 0.0f) ? -x : x;
}

/* Limitar la velocidad del motor TMC2208 */
static float TMC2208_LimitRpm(TMC2208_t *drv, float rpm)
{
    if (drv == NULL)
    {
        return 0.0f;
    }

    if (rpm > drv->rpm_max)
    {
        return drv->rpm_max;
    }

    if (rpm < -drv->rpm_max)
    {
        return -drv->rpm_max;
    }

    return rpm;
}

/* Convertir la velocidad en RPM a un valor para el registro de autoreload */
static uint32_t TMC2208_RpmToArr(TMC2208_t *drv, float rpm_abs)
{
    float pulse_hz;
    float arr_f;

    if (drv == NULL)
    {
        return 0U;
    }

    pulse_hz = (rpm_abs * drv->steps_per_rev) / 60.0f;

    if (pulse_hz < 1.0f)
    {
        pulse_hz = 1.0f;
    }

    arr_f = (drv->timer_clk_hz / pulse_hz) - 1.0f;

    if (arr_f < 20.0f)
    {
        arr_f = 20.0f;
    }

    if (arr_f > (float)drv->arr_max)
    {
        arr_f = (float)drv->arr_max;
    }

    return (uint32_t)arr_f;
}

/* Configurar la dirección del motor TMC2208 */
static void TMC2208_SetDirection(TMC2208_t *drv, bool reverse)
{
    GPIO_PinState pin_state;

    if ((drv == NULL) || (drv->dir_port == NULL))
    {
        return;
    }

    if (drv->invert_dir)
    {
        reverse = !reverse;
    }

    pin_state = reverse ? GPIO_PIN_RESET : GPIO_PIN_SET;

    HAL_GPIO_WritePin(drv->dir_port, drv->dir_pin, pin_state);
}

/* Configurar el estado de habilitación del motor TMC2208 */
static void TMC2208_Enable(TMC2208_t *drv, bool enable)
{
    if ((drv == NULL) || (drv->en_port == NULL))
    {
        return;
    }

    HAL_GPIO_WritePin(
        drv->en_port,
        drv->en_pin,
        enable ? GPIO_PIN_RESET : GPIO_PIN_SET
    );
}
