/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: WINDER_FSM.h
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

#ifndef WINDER_FSM_h_
#define WINDER_FSM_h_
#ifndef WINDER_FSM_COMMON_INCLUDES_
#define WINDER_FSM_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* WINDER_FSM_COMMON_INCLUDES_ */

#include "WINDER_FSM_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  uint8_T is_active_c3_WINDER_FSM;     /* '<Root>/FSM EXTRUDER' */
  uint8_T is_c3_WINDER_FSM;            /* '<Root>/FSM EXTRUDER' */
} DW_WINDER_FSM_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  boolean_T init_ok;                   /* '<Root>/init_ok' */
  boolean_T stop_cmd;                  /* '<Root>/stop_cmd' */
  boolean_T fault_cmd;                 /* '<Root>/fault_cmd' */
  boolean_T reset_ok;                  /* '<Root>/reset_ok' */
  boolean_T run_cmd;                   /* '<Root>/run_cmd' */
  boolean_T home_cmd;                  /* '<Root>/home_cmd' */
  real_T home_ok;                      /* '<Root>/home_ok' */
} ExtU_WINDER_FSM_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  boolean_T alarm_active;              /* '<Root>/alarm_active' */
  uint8_T state_id;                    /* '<Root>/state_id' */
  boolean_T guide_enable;              /* '<Root>/guide_enable' */
  boolean_T winder_enable;             /* '<Root>/winder_enable' */
} ExtY_WINDER_FSM_T;

/* Real-time Model Data Structure */
struct tag_RTM_WINDER_FSM_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_WINDER_FSM_T WINDER_FSM_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_WINDER_FSM_T WINDER_FSM_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_WINDER_FSM_T WINDER_FSM_Y;

/* Model entry point functions */
extern void WINDER_FSM_initialize(void);
extern void WINDER_FSM_step(void);
extern void WINDER_FSM_terminate(void);

/* Real-time Model object */
extern RT_MODEL_WINDER_FSM_T *const WINDER_FSM_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'WINDER_FSM'
 * '<S1>'   : 'WINDER_FSM/FSM EXTRUDER'
 */
#endif                                 /* WINDER_FSM_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
