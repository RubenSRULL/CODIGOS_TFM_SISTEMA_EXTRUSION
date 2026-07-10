/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: WINDER_FSM.c
 *
 * Code generated for Simulink model 'WINDER_FSM'.
 *
 * Model version                  : 1.96
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Sat Jun 20 22:49:09 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "WINDER_FSM.h"
#include "rtwtypes.h"

/* Named constants for Chart: '<Root>/FSM EXTRUDER' */
#define WINDER_FSM_IN_ALARM            ((uint8_T)1U)
#define WINDER_FSM_IN_BOOT             ((uint8_T)2U)
#define WINDER_FSM_IN_HOME             ((uint8_T)3U)
#define WINDER_FSM_IN_RUNNING          ((uint8_T)4U)
#define WINDER_FSM_IN_STOP             ((uint8_T)5U)

/* Block states (default storage) */
DW_WINDER_FSM_T WINDER_FSM_DW;

/* External inputs (root inport signals with default storage) */
ExtU_WINDER_FSM_T WINDER_FSM_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_WINDER_FSM_T WINDER_FSM_Y;

/* Real-time model */
static RT_MODEL_WINDER_FSM_T WINDER_FSM_M_;
RT_MODEL_WINDER_FSM_T *const WINDER_FSM_M = &WINDER_FSM_M_;

/* Model step function */
void WINDER_FSM_step(void)
{
  /* Chart: '<Root>/FSM EXTRUDER' incorporates:
   *  Inport: '<Root>/fault_cmd'
   *  Inport: '<Root>/home_cmd'
   *  Inport: '<Root>/home_ok'
   *  Inport: '<Root>/init_ok'
   *  Inport: '<Root>/reset_ok'
   *  Inport: '<Root>/run_cmd'
   *  Inport: '<Root>/stop_cmd'
   */
  if (WINDER_FSM_DW.is_active_c3_WINDER_FSM == 0) {
    WINDER_FSM_DW.is_active_c3_WINDER_FSM = 1U;
    WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_BOOT;

    /* Outport: '<Root>/state_id' */
    WINDER_FSM_Y.state_id = 0U;

    /* Outport: '<Root>/guide_enable' */
    WINDER_FSM_Y.guide_enable = false;

    /* Outport: '<Root>/winder_enable' */
    WINDER_FSM_Y.winder_enable = false;

    /* Outport: '<Root>/alarm_active' */
    WINDER_FSM_Y.alarm_active = false;
  } else {
    switch (WINDER_FSM_DW.is_c3_WINDER_FSM) {
     case WINDER_FSM_IN_ALARM:
      /* Outport: '<Root>/state_id' */
      WINDER_FSM_Y.state_id = 4U;

      /* Outport: '<Root>/guide_enable' */
      WINDER_FSM_Y.guide_enable = false;

      /* Outport: '<Root>/winder_enable' */
      WINDER_FSM_Y.winder_enable = false;

      /* Outport: '<Root>/alarm_active' */
      WINDER_FSM_Y.alarm_active = true;
      if (WINDER_FSM_U.reset_ok && (!WINDER_FSM_U.fault_cmd)) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_BOOT;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 0U;

        /* Outport: '<Root>/alarm_active' */
        WINDER_FSM_Y.alarm_active = false;
      }
      break;

     case WINDER_FSM_IN_BOOT:
      /* Outport: '<Root>/state_id' */
      WINDER_FSM_Y.state_id = 0U;

      /* Outport: '<Root>/guide_enable' */
      WINDER_FSM_Y.guide_enable = false;

      /* Outport: '<Root>/winder_enable' */
      WINDER_FSM_Y.winder_enable = false;

      /* Outport: '<Root>/alarm_active' */
      WINDER_FSM_Y.alarm_active = false;
      if (WINDER_FSM_U.init_ok && (!WINDER_FSM_U.fault_cmd)) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_STOP;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 1U;
      } else if (WINDER_FSM_U.fault_cmd) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_ALARM;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 4U;

        /* Outport: '<Root>/alarm_active' */
        WINDER_FSM_Y.alarm_active = true;
      }
      break;

     case WINDER_FSM_IN_HOME:
      /* Outport: '<Root>/state_id' */
      WINDER_FSM_Y.state_id = 2U;

      /* Outport: '<Root>/guide_enable' */
      WINDER_FSM_Y.guide_enable = true;

      /* Outport: '<Root>/winder_enable' */
      WINDER_FSM_Y.winder_enable = true;

      /* Outport: '<Root>/alarm_active' */
      WINDER_FSM_Y.alarm_active = false;
      if (WINDER_FSM_U.stop_cmd) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_STOP;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 1U;

        /* Outport: '<Root>/guide_enable' */
        WINDER_FSM_Y.guide_enable = false;

        /* Outport: '<Root>/winder_enable' */
        WINDER_FSM_Y.winder_enable = false;
      } else if (WINDER_FSM_U.fault_cmd) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_ALARM;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 4U;

        /* Outport: '<Root>/guide_enable' */
        WINDER_FSM_Y.guide_enable = false;

        /* Outport: '<Root>/winder_enable' */
        WINDER_FSM_Y.winder_enable = false;

        /* Outport: '<Root>/alarm_active' */
        WINDER_FSM_Y.alarm_active = true;
      } else if (WINDER_FSM_U.run_cmd) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_RUNNING;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 3U;
      }
      break;

     case WINDER_FSM_IN_RUNNING:
      /* Outport: '<Root>/state_id' */
      WINDER_FSM_Y.state_id = 3U;

      /* Outport: '<Root>/guide_enable' */
      WINDER_FSM_Y.guide_enable = true;

      /* Outport: '<Root>/winder_enable' */
      WINDER_FSM_Y.winder_enable = true;

      /* Outport: '<Root>/alarm_active' */
      WINDER_FSM_Y.alarm_active = false;
      if (WINDER_FSM_U.fault_cmd) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_ALARM;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 4U;

        /* Outport: '<Root>/guide_enable' */
        WINDER_FSM_Y.guide_enable = false;

        /* Outport: '<Root>/winder_enable' */
        WINDER_FSM_Y.winder_enable = false;

        /* Outport: '<Root>/alarm_active' */
        WINDER_FSM_Y.alarm_active = true;
      } else if (WINDER_FSM_U.stop_cmd) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_STOP;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 1U;

        /* Outport: '<Root>/guide_enable' */
        WINDER_FSM_Y.guide_enable = false;

        /* Outport: '<Root>/winder_enable' */
        WINDER_FSM_Y.winder_enable = false;
      }
      break;

     default:
      /* Outport: '<Root>/state_id' */
      /* case IN_STOP: */
      WINDER_FSM_Y.state_id = 1U;

      /* Outport: '<Root>/guide_enable' */
      WINDER_FSM_Y.guide_enable = false;

      /* Outport: '<Root>/winder_enable' */
      WINDER_FSM_Y.winder_enable = false;

      /* Outport: '<Root>/alarm_active' */
      WINDER_FSM_Y.alarm_active = false;
      if (WINDER_FSM_U.fault_cmd) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_ALARM;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 4U;

        /* Outport: '<Root>/alarm_active' */
        WINDER_FSM_Y.alarm_active = true;
      } else if (WINDER_FSM_U.home_cmd && (WINDER_FSM_U.home_ok != 0.0)) {
        WINDER_FSM_DW.is_c3_WINDER_FSM = WINDER_FSM_IN_HOME;

        /* Outport: '<Root>/state_id' */
        WINDER_FSM_Y.state_id = 2U;

        /* Outport: '<Root>/guide_enable' */
        WINDER_FSM_Y.guide_enable = true;

        /* Outport: '<Root>/winder_enable' */
        WINDER_FSM_Y.winder_enable = true;
      }
      break;
    }
  }

  /* End of Chart: '<Root>/FSM EXTRUDER' */
}

/* Model initialize function */
void WINDER_FSM_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void WINDER_FSM_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
