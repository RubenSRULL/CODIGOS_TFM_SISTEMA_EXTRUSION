/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: EXTRUDER_FSM.c
 *
 * Code generated for Simulink model 'EXTRUDER_FSM'.
 *
 * Model version                  : 1.68
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Sat Jun 20 22:48:02 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "EXTRUDER_FSM.h"
#include "rtwtypes.h"

/* Named constants for Chart: '<Root>/FSM EXTRUDER' */
#define EXTRUDER_FSM_IN_ALARM          ((uint8_T)1U)
#define EXTRUDER_FSM_IN_AUTO           ((uint8_T)2U)
#define EXTRUDER_FSM_IN_BOOT           ((uint8_T)3U)
#define EXTRUDER_FSM_IN_EXTRUDING      ((uint8_T)1U)
#define EXTRUDER_FSM_IN_IDLE_AUTO      ((uint8_T)2U)
#define EXTRUDER_FSM_IN_MANUAL         ((uint8_T)4U)
#define EXTRUDER_FSM_IN_NO_ACTIVE_CHILD ((uint8_T)0U)
#define EXTRUDER_FSM_IN_PREHEAT        ((uint8_T)3U)
#define EXTRUDER_FSM_IN_STOP           ((uint8_T)5U)

/* Block states (default storage) */
DW_EXTRUDER_FSM_T EXTRUDER_FSM_DW;

/* External inputs (root inport signals with default storage) */
ExtU_EXTRUDER_FSM_T EXTRUDER_FSM_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_EXTRUDER_FSM_T EXTRUDER_FSM_Y;

/* Real-time model */
static RT_MODEL_EXTRUDER_FSM_T EXTRUDER_FSM_M_;
RT_MODEL_EXTRUDER_FSM_T *const EXTRUDER_FSM_M = &EXTRUDER_FSM_M_;

/* Forward declaration for local functions */
static void EXTRUDER_FSM_AUTO(void);

/* Function for Chart: '<Root>/FSM EXTRUDER' */
static void EXTRUDER_FSM_AUTO(void)
{
  /* Inport: '<Root>/fault' incorporates:
   *  Inport: '<Root>/stop_cmd'
   *  Inport: '<Root>/stop_ok'
   */
  if (EXTRUDER_FSM_U.fault) {
    EXTRUDER_FSM_DW.is_AUTO = EXTRUDER_FSM_IN_NO_ACTIVE_CHILD;
    EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_ALARM;

    /* Outport: '<Root>/state_id' */
    EXTRUDER_FSM_Y.state_id = 6U;

    /* Outport: '<Root>/heater_enable' */
    EXTRUDER_FSM_Y.heater_enable = false;

    /* Outport: '<Root>/extruder_enable' */
    EXTRUDER_FSM_Y.extruder_enable = false;

    /* Outport: '<Root>/winder_enable' */
    EXTRUDER_FSM_Y.winder_enable = false;

    /* Outport: '<Root>/manual_enable' */
    EXTRUDER_FSM_Y.manual_enable = false;

    /* Outport: '<Root>/alarm_active' */
    EXTRUDER_FSM_Y.alarm_active = true;
  } else if (EXTRUDER_FSM_U.stop_cmd || EXTRUDER_FSM_U.stop_ok) {
    EXTRUDER_FSM_DW.is_AUTO = EXTRUDER_FSM_IN_NO_ACTIVE_CHILD;
    EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_STOP;

    /* Outport: '<Root>/state_id' */
    EXTRUDER_FSM_Y.state_id = 1U;

    /* Outport: '<Root>/heater_enable' */
    EXTRUDER_FSM_Y.heater_enable = false;

    /* Outport: '<Root>/extruder_enable' */
    EXTRUDER_FSM_Y.extruder_enable = false;

    /* Outport: '<Root>/winder_enable' */
    EXTRUDER_FSM_Y.winder_enable = false;

    /* Outport: '<Root>/manual_enable' */
    EXTRUDER_FSM_Y.manual_enable = false;

    /* Outport: '<Root>/alarm_active' */
    EXTRUDER_FSM_Y.alarm_active = false;
  } else {
    switch (EXTRUDER_FSM_DW.is_AUTO) {
     case EXTRUDER_FSM_IN_EXTRUDING:
      /* Outport: '<Root>/state_id' */
      EXTRUDER_FSM_Y.state_id = 4U;

      /* Outport: '<Root>/heater_enable' */
      EXTRUDER_FSM_Y.heater_enable = true;

      /* Outport: '<Root>/extruder_enable' */
      EXTRUDER_FSM_Y.extruder_enable = true;

      /* Outport: '<Root>/winder_enable' */
      EXTRUDER_FSM_Y.winder_enable = true;

      /* Outport: '<Root>/manual_enable' */
      EXTRUDER_FSM_Y.manual_enable = false;

      /* Outport: '<Root>/alarm_active' */
      EXTRUDER_FSM_Y.alarm_active = false;

      /* Inport: '<Root>/finish_ok' */
      if (EXTRUDER_FSM_U.finish_ok) {
        EXTRUDER_FSM_DW.is_AUTO = EXTRUDER_FSM_IN_IDLE_AUTO;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 2U;

        /* Outport: '<Root>/heater_enable' */
        EXTRUDER_FSM_Y.heater_enable = false;

        /* Outport: '<Root>/extruder_enable' */
        EXTRUDER_FSM_Y.extruder_enable = false;

        /* Outport: '<Root>/winder_enable' */
        EXTRUDER_FSM_Y.winder_enable = false;
      }

      /* End of Inport: '<Root>/finish_ok' */
      break;

     case EXTRUDER_FSM_IN_IDLE_AUTO:
      /* Outport: '<Root>/state_id' */
      EXTRUDER_FSM_Y.state_id = 2U;

      /* Outport: '<Root>/heater_enable' */
      EXTRUDER_FSM_Y.heater_enable = false;

      /* Outport: '<Root>/extruder_enable' */
      EXTRUDER_FSM_Y.extruder_enable = false;

      /* Outport: '<Root>/winder_enable' */
      EXTRUDER_FSM_Y.winder_enable = false;

      /* Outport: '<Root>/manual_enable' */
      EXTRUDER_FSM_Y.manual_enable = false;

      /* Outport: '<Root>/alarm_active' */
      EXTRUDER_FSM_Y.alarm_active = false;

      /* Inport: '<Root>/preheat_cmd' */
      if (EXTRUDER_FSM_U.preheat_cmd) {
        EXTRUDER_FSM_DW.is_AUTO = EXTRUDER_FSM_IN_PREHEAT;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 3U;

        /* Outport: '<Root>/heater_enable' */
        EXTRUDER_FSM_Y.heater_enable = true;
      }

      /* End of Inport: '<Root>/preheat_cmd' */
      break;

     default:
      /* Outport: '<Root>/state_id' */
      /* case IN_PREHEAT: */
      EXTRUDER_FSM_Y.state_id = 3U;

      /* Outport: '<Root>/heater_enable' */
      EXTRUDER_FSM_Y.heater_enable = true;

      /* Outport: '<Root>/extruder_enable' */
      EXTRUDER_FSM_Y.extruder_enable = false;

      /* Outport: '<Root>/winder_enable' */
      EXTRUDER_FSM_Y.winder_enable = false;

      /* Outport: '<Root>/manual_enable' */
      EXTRUDER_FSM_Y.manual_enable = false;

      /* Outport: '<Root>/alarm_active' */
      EXTRUDER_FSM_Y.alarm_active = false;

      /* Inport: '<Root>/extrude_cmd' incorporates:
       *  Inport: '<Root>/extrude_ok'
       */
      if (EXTRUDER_FSM_U.extrude_cmd && EXTRUDER_FSM_U.extrude_ok) {
        EXTRUDER_FSM_DW.is_AUTO = EXTRUDER_FSM_IN_EXTRUDING;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 4U;

        /* Outport: '<Root>/extruder_enable' */
        EXTRUDER_FSM_Y.extruder_enable = true;

        /* Outport: '<Root>/winder_enable' */
        EXTRUDER_FSM_Y.winder_enable = true;
      }

      /* End of Inport: '<Root>/extrude_cmd' */
      break;
    }
  }

  /* End of Inport: '<Root>/fault' */
}

/* Model step function */
void EXTRUDER_FSM_step(void)
{
  /* Chart: '<Root>/FSM EXTRUDER' incorporates:
   *  Inport: '<Root>/auto_cmd'
   *  Inport: '<Root>/fault'
   *  Inport: '<Root>/init_ok'
   *  Inport: '<Root>/manual_cmd'
   *  Inport: '<Root>/reset_ok'
   *  Inport: '<Root>/stop_cmd'
   *  Inport: '<Root>/stop_ok'
   */
  if (EXTRUDER_FSM_DW.is_active_c3_EXTRUDER_FSM == 0) {
    EXTRUDER_FSM_DW.is_active_c3_EXTRUDER_FSM = 1U;
    EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_BOOT;

    /* Outport: '<Root>/state_id' */
    EXTRUDER_FSM_Y.state_id = 0U;

    /* Outport: '<Root>/heater_enable' */
    EXTRUDER_FSM_Y.heater_enable = false;

    /* Outport: '<Root>/extruder_enable' */
    EXTRUDER_FSM_Y.extruder_enable = false;

    /* Outport: '<Root>/winder_enable' */
    EXTRUDER_FSM_Y.winder_enable = false;

    /* Outport: '<Root>/manual_enable' */
    EXTRUDER_FSM_Y.manual_enable = false;

    /* Outport: '<Root>/alarm_active' */
    EXTRUDER_FSM_Y.alarm_active = false;
  } else {
    switch (EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM) {
     case EXTRUDER_FSM_IN_ALARM:
      /* Outport: '<Root>/state_id' */
      EXTRUDER_FSM_Y.state_id = 6U;

      /* Outport: '<Root>/heater_enable' */
      EXTRUDER_FSM_Y.heater_enable = false;

      /* Outport: '<Root>/extruder_enable' */
      EXTRUDER_FSM_Y.extruder_enable = false;

      /* Outport: '<Root>/winder_enable' */
      EXTRUDER_FSM_Y.winder_enable = false;

      /* Outport: '<Root>/manual_enable' */
      EXTRUDER_FSM_Y.manual_enable = false;

      /* Outport: '<Root>/alarm_active' */
      EXTRUDER_FSM_Y.alarm_active = true;
      if (EXTRUDER_FSM_U.reset_ok && (!EXTRUDER_FSM_U.fault)) {
        EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_STOP;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 1U;

        /* Outport: '<Root>/alarm_active' */
        EXTRUDER_FSM_Y.alarm_active = false;
      }
      break;

     case EXTRUDER_FSM_IN_AUTO:
      EXTRUDER_FSM_AUTO();
      break;

     case EXTRUDER_FSM_IN_BOOT:
      /* Outport: '<Root>/state_id' */
      EXTRUDER_FSM_Y.state_id = 0U;

      /* Outport: '<Root>/heater_enable' */
      EXTRUDER_FSM_Y.heater_enable = false;

      /* Outport: '<Root>/extruder_enable' */
      EXTRUDER_FSM_Y.extruder_enable = false;

      /* Outport: '<Root>/winder_enable' */
      EXTRUDER_FSM_Y.winder_enable = false;

      /* Outport: '<Root>/manual_enable' */
      EXTRUDER_FSM_Y.manual_enable = false;

      /* Outport: '<Root>/alarm_active' */
      EXTRUDER_FSM_Y.alarm_active = false;
      if (EXTRUDER_FSM_U.init_ok && (!EXTRUDER_FSM_U.fault)) {
        EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_STOP;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 1U;
      } else if (EXTRUDER_FSM_U.fault) {
        EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_ALARM;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 6U;

        /* Outport: '<Root>/alarm_active' */
        EXTRUDER_FSM_Y.alarm_active = true;
      }
      break;

     case EXTRUDER_FSM_IN_MANUAL:
      /* Outport: '<Root>/state_id' */
      EXTRUDER_FSM_Y.state_id = 5U;

      /* Outport: '<Root>/heater_enable' */
      EXTRUDER_FSM_Y.heater_enable = false;

      /* Outport: '<Root>/extruder_enable' */
      EXTRUDER_FSM_Y.extruder_enable = false;

      /* Outport: '<Root>/winder_enable' */
      EXTRUDER_FSM_Y.winder_enable = false;

      /* Outport: '<Root>/manual_enable' */
      EXTRUDER_FSM_Y.manual_enable = true;

      /* Outport: '<Root>/alarm_active' */
      EXTRUDER_FSM_Y.alarm_active = false;
      if (EXTRUDER_FSM_U.stop_cmd || EXTRUDER_FSM_U.stop_ok) {
        EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_STOP;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 1U;

        /* Outport: '<Root>/manual_enable' */
        EXTRUDER_FSM_Y.manual_enable = false;
      } else if (EXTRUDER_FSM_U.fault) {
        EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_ALARM;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 6U;

        /* Outport: '<Root>/manual_enable' */
        EXTRUDER_FSM_Y.manual_enable = false;

        /* Outport: '<Root>/alarm_active' */
        EXTRUDER_FSM_Y.alarm_active = true;
      }
      break;

     default:
      /* Outport: '<Root>/state_id' */
      /* case IN_STOP: */
      EXTRUDER_FSM_Y.state_id = 1U;

      /* Outport: '<Root>/heater_enable' */
      EXTRUDER_FSM_Y.heater_enable = false;

      /* Outport: '<Root>/extruder_enable' */
      EXTRUDER_FSM_Y.extruder_enable = false;

      /* Outport: '<Root>/winder_enable' */
      EXTRUDER_FSM_Y.winder_enable = false;

      /* Outport: '<Root>/manual_enable' */
      EXTRUDER_FSM_Y.manual_enable = false;

      /* Outport: '<Root>/alarm_active' */
      EXTRUDER_FSM_Y.alarm_active = false;
      if (EXTRUDER_FSM_U.auto_cmd) {
        EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_AUTO;
        EXTRUDER_FSM_DW.is_AUTO = EXTRUDER_FSM_IN_IDLE_AUTO;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 2U;
      } else if (EXTRUDER_FSM_U.manual_cmd) {
        EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_MANUAL;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 5U;

        /* Outport: '<Root>/manual_enable' */
        EXTRUDER_FSM_Y.manual_enable = true;
      } else if (EXTRUDER_FSM_U.fault) {
        EXTRUDER_FSM_DW.is_c3_EXTRUDER_FSM = EXTRUDER_FSM_IN_ALARM;

        /* Outport: '<Root>/state_id' */
        EXTRUDER_FSM_Y.state_id = 6U;

        /* Outport: '<Root>/alarm_active' */
        EXTRUDER_FSM_Y.alarm_active = true;
      }
      break;
    }
  }

  /* End of Chart: '<Root>/FSM EXTRUDER' */
}

/* Model initialize function */
void EXTRUDER_FSM_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void EXTRUDER_FSM_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
