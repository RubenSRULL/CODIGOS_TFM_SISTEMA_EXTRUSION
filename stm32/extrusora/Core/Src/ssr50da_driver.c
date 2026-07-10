/*
 * ssr50da_driver.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

 /* ==================================== */
 /* Librerias                            */
 /* ==================================== */
#include "ssr50da_driver.h"
#include "cmsis_os.h"

// =================================== //
// Definiciones
// =================================== //
#define SSR50DA_WINDOW_MS    1000U      // Ventana de tiempo en milisegundos para el control del ciclo de trabajo

/* ==================================== */
/* Funciones auxiliares                 */
/* ==================================== */


/* Limita un valor flotante entre un mínimo y un máximo */
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

/* Convierte milisegundos a ticks */
static uint32_t ticks_from_ms(uint32_t ms)
{
    uint32_t tick_freq;

    tick_freq = osKernelGetTickFreq();

    return (ms * tick_freq) / 1000U;
}

/* Escribir en el pin GPIO si enable = true */
static void SSR50DA_Write(SSR50DA_t *ssr, bool enabled)
{
    if ((ssr == NULL) || (!ssr->initialized))
    {
        return;
    }

    HAL_GPIO_WritePin(
        ssr->gpio_port,
        ssr->gpio_pin,
        enabled ? GPIO_PIN_SET : GPIO_PIN_RESET
    );
}

/* Inicializar el driver SSR50DA */
void SSR50DA_Init(
    SSR50DA_t *ssr,
    GPIO_TypeDef *gpio_port,
    uint16_t gpio_pin
)
{
    if (ssr == NULL)
    {
        return;
    }

    ssr->gpio_port = gpio_port;
    ssr->gpio_pin = gpio_pin;

    ssr->duty = 0.0f;
    ssr->window_start_tick = osKernelGetTickCount();

    ssr->initialized = true;

    SSR50DA_Write(ssr, false);
}

/* Establecer el ciclo de trabajo del driver SSR50DA */
void SSR50DA_SetDuty(
    SSR50DA_t *ssr,
    float duty
)
{
    if ((ssr == NULL) || (!ssr->initialized))
    {
        return;
    }

    ssr->duty = clamp_float(duty, 0.0f, 1.0f);
}

/* Actualizar el estado del driver SSR50DA */
void SSR50DA_Update(SSR50DA_t *ssr)
{
    uint32_t now;
    uint32_t window_ticks;
    uint32_t elapsed_ticks;
    uint32_t on_ticks;

    if ((ssr == NULL) || (!ssr->initialized))
    {
        return;
    }

    now = osKernelGetTickCount();

    window_ticks = ticks_from_ms(SSR50DA_WINDOW_MS);

    /* Verificar si el tiempo de ventana es válido */
    if (window_ticks == 0U)
    {
        SSR50DA_Write(ssr, false);
        return;
    }

    /* Mientras el tiempo transcurrido sea mayor o igual al tiempo de ventana */
    while ((now - ssr->window_start_tick) >= window_ticks)
    {
        /* Actualizar el inicio de la ventana */
        ssr->window_start_tick += window_ticks;
    }

    /* Calcular el tiempo transcurrido dentro de la ventana */
    elapsed_ticks = now - ssr->window_start_tick;

    /* Calcular el tiempo en el que el pin debe estar activo */
    on_ticks = (uint32_t)(ssr->duty * (float)window_ticks);

    /* Verificar si el pin debe estar activo comparando el tiempo transcurrido con el tiempo en el que debe estar activo */
    if ((ssr->duty > 0.0f) &&
        (elapsed_ticks < on_ticks))
    {
        SSR50DA_Write(ssr, true);
    }
    else
    {
        SSR50DA_Write(ssr, false);
    }
}

/* Apagar el driver SSR50DA */
void SSR50DA_Off(SSR50DA_t *ssr)
{
    if ((ssr == NULL) || (!ssr->initialized))
    {
        return;
    }

    ssr->duty = 0.0f;
    ssr->window_start_tick = osKernelGetTickCount();

    SSR50DA_Write(ssr, false);
}

/* Obtener el ciclo de trabajo del driver SSR50DA */
float SSR50DA_GetDuty(SSR50DA_t *ssr)
{
    if ((ssr == NULL) || (!ssr->initialized))
    {
        return 0.0f;
    }

    return ssr->duty;
}
