/*
 * temperature_control.c
 *
 *  Created on: 22 jun 2026
 *      Author: rsahu
 */

/* ========================================================= */
/* Librerías                                                 */
/* ========================================================= */
#include "temperature_control.h"
#include "process_data.h"
#include "pid_controller.h"
#include "max31856_driver.h"
#include "ssr50da_driver.h"
#include "cmsis_os.h"
#include <math.h>
#include <stdbool.h>

/* ========================================================= */
/* Configuracion                                             */
/* ========================================================= */
#define TEMP_CONTROL_PERIOD_MS          250U                                        // Periodo de control en milisegundos
#define TEMP_MAX_SAFE_C                 300.0f                                      // Temperatura máxima segura en grados Celsius
#define TEMP_PID_KP                     0.02f                                       // Ganancia proporcional del controlador PID
#define TEMP_PID_KI                     0.000075f                                   // Ganancia integral del controlador PID
#define TEMP_PID_KD                     0.0f                                        // Ganancia derivativa del controlador PID
#define TEMP_PID_DEADBAND_C             0.5f                                        // Banda muerta del controlador PID en grados Celsius
#define TEMP_PID_OUTPUT_RATE_LIMIT      0.0f                                        // Límite de la tasa de cambio de salida del controlador PID

/* ========================================================= */
/* Variables privadas                                        */
/* ========================================================= */
static PIDController_t g_temperature_pid;       // Controlador PID para el control de temperatura
static MAX31856_t g_thermocouple;               // Dispositivo MAX31856 para la lectura del termopar
static SSR50DA_t g_ssr;                         // Dispositivo SSR50DA para el control del calentador
static bool g_initialized = false;              // Indica si el sistema de control de temperatura ha sido inicializado
static bool g_sensor_ok = false;                // Indica si el sensor de temperatura está funcionando correctamente
static uint32_t g_last_control_tick = 0;        // Último tick de control de temperatura

/* Convierte milisegundos a ticks */
static uint32_t ticks_from_ms(uint32_t ms)
{
    uint32_t tick_freq;
    tick_freq = osKernelGetTickFreq();
    return (ms * tick_freq) / 1000U;
}

/* Configura el controlador PID para el control de temperatura */
static void configure_temperature_pid(void)
{
    /* Inicializa el controlador PID */
    PID_Init(&g_temperature_pid);

    /* Configura los parámetros básicos del PID */
    PID_ConfigureBasic(
        &g_temperature_pid,
        TEMP_PID_KP,
        TEMP_PID_KI,
        TEMP_PID_KD,
        0.0f,
        1.0f,
        (float)TEMP_CONTROL_PERIOD_MS / 1000.0f
    );

    /* Configura los parámetros avanzados del PID */
    PID_ConfigureAdvanced(
        &g_temperature_pid,
        TEMP_PID_DEADBAND_C,
        TEMP_PID_OUTPUT_RATE_LIMIT,
        0.0f,
        true
    );
}

/* Configura el sensor de temperatura MAX31856 */
static bool configure_temperature_sensor(SPI_HandleTypeDef *hspi)
{
    bool ok;

    /* Inicializa el dispositivo MAX31856 */
    MAX31856_InitDevice(
        &g_thermocouple,
        hspi,
        CS_GPIO_Port,
        CS_Pin
    );

    /* Inicia el sensor y verifica si está funcionando correctamente */
    ok = MAX31856_Begin(&g_thermocouple);

    if (!ok)
    {
        return false;
    }

    /* Configura el filtro de ruido en 50Hz */
    MAX31856_SetNoiseFilter(
        &g_thermocouple,
        MAX31856_NOISE_FILTER_50HZ
    );

    /* Configura el modo de conversión continua */
    MAX31856_SetConversionMode(
        &g_thermocouple,
        MAX31856_CONTINUOUS
    );

    return true;
}


/* Funciones públicas */

/* Inicializa el control de temperatura */
void TemperatureControl_Init(SPI_HandleTypeDef *hspi)
{
    /* Configura el controlador PID */
    configure_temperature_pid();

    /* Inicializa el dispositivo SSR50DA */
    SSR50DA_Init(
        &g_ssr,
        SSR_GPIO_Port,
        SSR_Pin
    );

    /* Configura el sensor de temperatura */
    g_sensor_ok = configure_temperature_sensor(hspi);

    /* Inicializa el tiempo del último control */
    g_last_control_tick = osKernelGetTickCount();
    g_initialized = true;

    /* Apaga el sistema de control de temperatura inicialmente */
    TemperatureControl_Off();
}

/* Apaga el sistema de control de temperatura */
void TemperatureControl_Off(void)
{
    /* Apaga el sistema de control de temperatura */
    SSR50DA_Off(&g_ssr);
    PID_Reset(&g_temperature_pid);
    ProcessData_SetHeaterPercent(0.0f);
    g_last_control_tick = osKernelGetTickCount();
}

/* Indica si el sensor de temperatura está funcionando correctamente */
bool TemperatureControl_IsSensorOk(void)
{
    return g_sensor_ok;
}

/* Actualiza el control de temperatura */
void TemperatureControl_Update(void)
{
    /* Obtiene los datos del proceso */
    ProcessData_t data;

    uint32_t now;
    uint32_t elapsed_ticks;
    uint32_t period_ticks;

    float temp_c;
    float heater_duty;

    if (!g_initialized)
    {
        return;
    }

    /* Actualiza el estado del dispositivo SSR */
    SSR50DA_Update(&g_ssr);
    /* Obtiene el tiempo actual */
    now = osKernelGetTickCount();
    /* Calcula los ticks del período de control */
    period_ticks = ticks_from_ms(TEMP_CONTROL_PERIOD_MS);
    elapsed_ticks = now - g_last_control_tick;

    if (elapsed_ticks < period_ticks)
    {
        return;
    }

    g_last_control_tick = now;

    if (!g_sensor_ok)
    {
        TemperatureControl_Off();
        return;
    }

    if (MAX31856_ReadFault(&g_thermocouple) != 0U)
    {
        MAX31856_ClearFault(&g_thermocouple);
        TemperatureControl_Off();
        return;
    }

    /* Obtiene la temperatura del termopar */
    temp_c = MAX31856_ReadThermocoupleTemperature(&g_thermocouple);

    if (isnan(temp_c))
    {
        TemperatureControl_Off();
        return;
    }

    /* Guarda la temperatura medida en los datos del proceso */
    ProcessData_SetTemperature(temp_c);
    /* Obtiene una instantánea de los datos del proceso */
    ProcessData_GetSnapshot(&data);

    bool heater_allowed;

    heater_allowed = data.heater_enable || data.manual_enable;

    /* Condiciones para apagar el sistema de control de temperatura */
    if ((!heater_allowed) ||
        (data.alarm_active) ||
        (data.target_temp_c <= 0.0f) ||
        (temp_c >= TEMP_MAX_SAFE_C))
    {
        TemperatureControl_Off();
        return;
    }

    /* Calcula la salida del controlador PID */
    heater_duty = PID_Update(
        &g_temperature_pid,
        data.target_temp_c,
        temp_c,
        0.0f
    );

    /* Establece el ciclo de trabajo del dispositivo SSR */
    SSR50DA_SetDuty(&g_ssr, heater_duty);
    /* Guarda el ciclo de trabajo del calentador en los datos del proceso */
    ProcessData_SetHeaterPercent(heater_duty * 100.0f);
}
