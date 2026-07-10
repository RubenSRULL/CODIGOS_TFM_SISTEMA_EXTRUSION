/*
 * motor_guide_control.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

// ================================== //
// Librerias
// ================================== //
#include "motor_guide_control.h"
#include "tmc2208_driver.h"
#include "process_data.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdbool.h>

// ================================== //
// Variables externas
// ================================== //
extern TIM_HandleTypeDef htim2;         // Handle del timer para el motor de guía

// ================================== //
// Enumeracion de dirección de guía
// ================================== //
typedef enum
{
    GUIDE_DIR_TO_LIMIT_1 = 0,
    GUIDE_DIR_TO_LIMIT_2 = 1

} GuideDirection_t;

// ================================== //
// Estructura de límite deboounced
// ================================== //
typedef struct
{
    bool raw_active;            // Estado raw del final de carrera (true = activo, false = inactivo)
    bool stable_active;         // Estado estable del final de carrera (true = activo, false = inactivo)
    bool event_used;            // Indicador de que el evento del límite ya fue utilizado
    uint32_t last_change_ms;    // Tiempo del último cambio de estado

} DebouncedLimit_t;

/* Instancia de motor de guía */
static TMC2208_t guide_motor;

/* Instancias de límites deboounced */
static DebouncedLimit_t limit_1;
static DebouncedLimit_t limit_2;

/* Instancia de dirección de guía */
static GuideDirection_t guide_dir = GUIDE_DIR_TO_LIMIT_2;


/* Actualización de límites deboounced */
static void GuideLimit_Update(
    DebouncedLimit_t *limit,
    bool raw_active,
    uint32_t now_ms
);

// ================================ //
// Prototipos de funciones privadas
// ================================ //
static bool Limit1_ReadRaw(void);
static bool Limit2_ReadRaw(void);
static float GuideDirectionToRpm(GuideDirection_t dir);


/* Inicialización del control del motor de guía */
void MotorGuideControl_Init(void)
{
    limit_1.raw_active = false;
    limit_1.stable_active = false;
    limit_1.event_used = false;
    limit_1.last_change_ms = 0U;

    limit_2.raw_active = false;
    limit_2.stable_active = false;
    limit_2.event_used = false;
    limit_2.last_change_ms = 0U;

    guide_dir = GUIDE_DIR_TO_LIMIT_2;

    TMC2208_Init(
        &guide_motor,
        &htim2,
        TIM_CHANNEL_1,
        M2_DIR_GPIO_Port,
        M2_DIR_Pin,
        M2_EN_GPIO_Port,
        M2_EN_Pin,
        GUIDE_TIMER_CLK_HZ,
        GUIDE_STEPS_PER_REV,
        GUIDE_RPM_MAX,
        GUIDE_TIMER_ARR_MAX,
        false
    );

    TMC2208_Stop(&guide_motor);
}

/* Actualización del control del motor de guía */
void MotorGuideControl_Update(void)
{
    ProcessData_t data;
    uint32_t now_ms;
    float rpm_cmd;

    now_ms = osKernelGetTickCount();

    /* Actualización de límites deboounced */
    GuideLimit_Update(&limit_1, Limit1_ReadRaw(), now_ms);
    GuideLimit_Update(&limit_2, Limit2_ReadRaw(), now_ms);

    /* Al tocar final 1, cambia hacia final 2 */
    if ((limit_1.stable_active == true) &&
        (limit_1.event_used == false))
    {
        guide_dir = GUIDE_DIR_TO_LIMIT_2;
        limit_1.event_used = true;
    }

    /* Al tocar final 2, cambia hacia final 1 */
    if ((limit_2.stable_active == true) &&
        (limit_2.event_used == false))
    {
        guide_dir = GUIDE_DIR_TO_LIMIT_1;
        limit_2.event_used = true;
    }

    ProcessData_GetSnapshot(&data);

    /* Si el control de guía está deshabilitado o hay un alarma activa */
    if ((data.guide_enable == false) ||
        (data.alarm_active == true))
    {
        TMC2208_Stop(&guide_motor);
        return;
    }

    /* Si los dos finales están activos, parada */
    if ((limit_1.stable_active == true) &&
        (limit_2.stable_active == true))
    {
        TMC2208_Stop(&guide_motor);
        return;
    }

    /* Conversión de dirección a velocidad */
    rpm_cmd = GuideDirectionToRpm(guide_dir);
    /* Configuración de la velocidad del motor */
    TMC2208_SetRpm(&guide_motor, rpm_cmd);
}

/* Actualización del estado de un límite deboounced */
static void GuideLimit_Update(
    DebouncedLimit_t *limit,
    bool raw_active,
    uint32_t now_ms
)
{
    if (limit == NULL)
    {
        return;
    }

    /* Si el estado raw ha cambiado */
    if (raw_active != limit->raw_active)
    {
        limit->raw_active = raw_active;
        limit->last_change_ms = now_ms;
        return;
    }

    /* Si ha pasado el tiempo de debounce */
    if ((now_ms - limit->last_change_ms) >= GUIDE_LIMIT_DEBOUNCE_MS)
    {
        /* Si el estado estable es diferente al estado raw */
        if (limit->stable_active != limit->raw_active)
        {
            limit->stable_active = limit->raw_active;

            if (limit->stable_active == false)
            {
                limit->event_used = false;
            }
        }
    }
}

/* Lectura del estado raw de un límite */
static bool Limit1_ReadRaw(void)
{
    return HAL_GPIO_ReadPin(
        FINAL_CARRERA_1_GPIO_Port,
        FINAL_CARRERA_1_Pin
    ) == GPIO_PIN_RESET;
}

/* Lectura del estado raw de un límite */
static bool Limit2_ReadRaw(void)
{
    return HAL_GPIO_ReadPin(
        FINAL_CARRERA_2_GPIO_Port,
        FINAL_CARRERA_2_Pin
    ) == GPIO_PIN_RESET;
}

/* Conversión de dirección a velocidad */
static float GuideDirectionToRpm(GuideDirection_t dir)
{
    /* Si la dirección es hacia el límite 2, retorna rpm positiva */
    if (dir == GUIDE_DIR_TO_LIMIT_2)
    {
        return GUIDE_RPM;
    }

    return -GUIDE_RPM;
}
