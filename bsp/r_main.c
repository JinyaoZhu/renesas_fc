/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V2.03.02.01 [15 May 2015]
* Device(s)    : R5F100LE
* Tool-Chain   : CA78K0R
* Description  : This file implements main function.
* Creation Date: 2015/8/15
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_intc.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
/* Start user code for include. Do not edit comment generated here */
#include "bsp.h"
#include "global.h"
#include "cordic.h"
#include "sensor_update.h"
#include "ekf_att.h"
#include "motor.h"
#include "control.h"
#include "rc.h"
#include "sonar.h"
#include "ccd_camera.h"
#include "slave.h"
#include "bmp180.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
#define ATTITUDE_FUSION_FACTOR_X (0.0020)
#define ATTITUDE_FUSION_FACTOR_Y (0.0020)
#define ATTITUDE_FUSION_FACTOR_Z (0.0020)

#define KALMAN_POS_K1_X (0.18)
#define KALMAN_POS_K2_X (0.15)

#define KALMAN_POS_K1_Y (0.18)
#define KALMAN_POS_K2_Y (0.15)

#define KALMAN_POS_K1_Z (0.1487) //(0.140)
#define KALMAN_POS_K2_Z (0.10)   //(0.1993)//(0.185)

float PID_POS_P_X = (2.20);
float PID_VEL_P_X = (8.50);
float PID_VEL_I_X = (3.50);
float PID_VEL_D_X = (0.80);

float PID_POS_P_Y = (2.00);
float PID_VEL_P_Y = (8.20);
float PID_VEL_I_Y = (3.20);
float PID_VEL_D_Y = (0.85);

float PID_POS_P_Z = 3.0;   //(5.0);//(1.730)
float PID_VEL_P_Z = 0.55;  //(0.50);//(0.55)
float PID_VEL_I_Z = 1.0;   //(1.0);//(2.500)
float PID_VEL_D_Z = 0.060; //(0.06);//(0.038)

#define STRESS_TEST (0)

#define RC_LOOP_PERIOD (0.020) /* 20ms */
#define SERIAL_PRINT_PERIOD (0.030)
#define NAV_PERIOD (0.050)
#define SLAVE_COM_PERIOD (0.030)
#define FAIL_SAFE_PERIOD (0.050)

void startup(void);
void loop(void);
void flight_control(void);

static SLAVE_DTAT_TYPE SlaveMCUData = {0};

static uint16_t Mode = 1;

/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
  R_MAIN_UserInit();
  /* Start user code. Do not edit comment generated here */
  startup();
  while (1U)
  {
    loop();
  }
  /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
  /* Start user code. Do not edit comment generated here */
  EI();
  /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */

void startup(void)
{
  uint16_t i = 0;

  R_TAU0_Channel0_Start(); /*PWM*/
  /* ESC calibrate */
  //Motor_MaxThrust(&g_Motor);
  //g_DelayMs(3000);

  Motor_MinThrust(&g_Motor);

  R_TAU0_Channel6_Start(); /* RC in */
  R_INTC6_Start();

  R_UART0_Start();
  R_TAU0_Channel7_Start(); /* timestamp timer */

  CORDIC_Init();
  IIC1_Init();     /* mpu6050 */
  R_CSI20_Start(); /* SPI */

  if (P14 .1 == HIGH)
  {
    Mode = 1;
  }

  if (P2 .1 == HIGH)
  {
    Mode = 2;
  }
  if (P2 .2 == HIGH)
  {
    Mode = 3;
  }
  if (P2 .3 == HIGH)
  {
    Mode = 4;
  }
  g_DelayMs(1000);
  for (i = 0; i < Mode; i++)
  {
    LED = HIGH;
    g_DelayMs(250);
    LED = LOW;
    g_DelayMs(250);
  }

  switch (Mode)
  {
  case 1:

    PID_POS_P_X = (2.55);
    PID_VEL_P_X = (30.0);
    PID_VEL_I_X = (4.00);
    PID_VEL_D_X = (2.20);

    PID_POS_P_Y = (2.50);
    PID_VEL_P_Y = (25.0);
    PID_VEL_I_Y = (3.20);
    PID_VEL_D_Y = (2.00);
    break;

  case 2:
    PID_POS_P_X = (2.20);
    PID_VEL_P_X = (8.60);
    PID_VEL_I_X = (3.50);
    PID_VEL_D_X = (0.80);

    PID_POS_P_Y = (2.00);
    PID_VEL_P_Y = (7.80);
    PID_VEL_I_Y = (3.50);
    PID_VEL_D_Y = (0.70);
    break;

  case 3:
    PID_POS_P_X = (2.55);
    PID_VEL_P_X = (30.0);
    PID_VEL_I_X = (4.00);
    PID_VEL_D_X = (2.20);

    PID_POS_P_Y = (2.50);
    PID_VEL_P_Y = (25.0);
    PID_VEL_I_Y = (3.20);
    PID_VEL_D_Y = (2.00);

    P2 .0 = HIGH;
    break;

  case 4:

    PID_POS_P_X = (2.20);
    PID_VEL_P_X = (8.60);
    PID_VEL_I_X = (3.50);
    PID_VEL_D_X = (0.80);

    PID_POS_P_Y = (2.00);
    PID_VEL_P_Y = (7.80);
    PID_VEL_I_Y = (3.50);
    PID_VEL_D_Y = (0.75);
    break;

  default:
    break;
  }

  SensorUpdate_Init();
  SensorUpdate_GetGyroAccRaw(&g_GyroRaw, &g_AccRaw);
  g_AccEstBody.x = g_AccRaw.x;
  g_AccEstBody.y = g_AccRaw.y;
  g_AccEstBody.z = g_AccRaw.z;
  g_FlagAttInit = 0;
  g_FlagArm = 1;
  do
  {
    Slave_GetData(&SlaveMCUData);
    BSP_Ser_Printf("Connect to slave MCU ...\r\n");
    g_DelayMs(200);
  } while ((SlaveMCUData.sonar == 0) && (SlaveMCUData.qual >= 10));
  BSP_Ser_Printf("System run...\r\n");
  R_INTC3_Start(); /* flight control loop */
}

/*
*********************************************************
*    Main loop
*********************************************************
*/
void loop(void)
{
  static float system_time = 0;
  static uint16_t timestamp;
  static float main_loop_dt = 0;

  /* loop period manage */
  static float serial_loop_time_cnt = 0.005;
  static float rc_loop_time_cnt = 0.010;
  static float nav_loop_time_cnt = 0.00;
  static float slave_com_loop_cnt = 0.01;
  static float sonar_loop_cnt = 0;
  static float fail_safe_loop_cnt = 0;

  static float rc_arm_wait_time = 0;
  static float set_height_wait_time = 0;
  static float last_thrust = 0;

  /* navigator loop */
  static float nav_timer = 0;
  static uint16_t nav_state = 0;

  /* Snoar loop */
  static float current_sonar = 0;
  static float last_sonar = 0;

  /* Slave MCU data */
  static uint32_t spi_sum = 0;
  static uint16_t spot_capture_cnt = 0;
  static uint16_t flag_spot_capture = RESET;
  static float euler_delay_pitch = 0;
  static float euler_delay_roll = 0;
  static float cam_raw_x = 0;
  static float cam_raw_y = 0;
  static float last_sonar_raw = 0;

  /* Fail safe */
  static float fail_safe_timeout = 0;

  main_loop_dt = g_Getdt(&timestamp);

  /* Update loop cnt */
  rc_loop_time_cnt += main_loop_dt;
  nav_loop_time_cnt += main_loop_dt;
  serial_loop_time_cnt += main_loop_dt;
  slave_com_loop_cnt += main_loop_dt;
  sonar_loop_cnt += main_loop_dt;
  fail_safe_loop_cnt += main_loop_dt;

  // if(g_FlagAttInit == SET)
  //   system_time += main_loop_dt;

  // if((system_time > 3.0f)&&(system_time < 4.0f)){
  //  g_FlagArm = RESET;
  //    g_RC.thrust = 0.2f;
  //  }

#if 1
  /* Navigate loop */
  switch (Mode)
  {
  case 1:
    if (nav_loop_time_cnt > NAV_PERIOD)
    {
      if ((g_FlagHovering == SET) && (g_FlagFailSafe == RESET))
      {
        nav_timer += nav_loop_time_cnt;

        switch (nav_state)
        {
        /* take off */
        case 0:
          if (g_TargetPos.z < 0.65f)
            g_TargetPos.z += 0.05;

          if (g_PosRef.z > 0.65)
          {
            g_TargetPos.z = 0.65;
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = SET;
            nav_state = 1;
            nav_timer = 0;
          }

          break;
        /* stablelize */
        case 1:
          if (nav_timer > 4.0f)
          {
            g_FlagEnableControlX = RESET;
            g_UserEuler.pitch = 2.0 * DEG2RAD;
            nav_state = 2;
            nav_timer = 0;
          }
          break;

        /* go ahead */
        case 2:
          if ((SlaveMCUData.bx == 0) && (SlaveMCUData.by == 0) && (SlaveMCUData.ly2 != 0))
          {
            g_UserEuler.pitch = 0.8 * DEG2RAD;
            nav_state = 3;
            nav_timer = 0;
          }
          break;

        /* stop accelerate */
        case 3:
          if ((SlaveMCUData.bx != 0) && (SlaveMCUData.by != 0))
          {
            g_UserEuler.pitch = -3.0f * DEG2RAD;
            g_PosRef.x = g_CamOffset_x = -0.20;
            g_VelRef.x = 0.2f;
            nav_state = 4;
            nav_timer = 0;
          }
          else if (nav_timer > 6.0f)
            g_UserEuler.pitch = 1.0 * DEG2RAD;
          break;

        /* catch ending point */
        case 4:
          if (nav_timer > 0.5f)
          {
            g_UserEuler.pitch = 0.0f * DEG2RAD;
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = SET;
            nav_state = 5;
            nav_timer = 0;
          }
          break;
        /* stabilize */
        case 5:

          if (nav_timer > 5.0f)
          {
            nav_state = 6;
            nav_timer = 0;
          }
          break;

        /* landing */
        case 6:
          if (g_TargetPos.z > 0.05)
            g_TargetPos.z -= 0.04;
          if (g_PosRef.z < 0.10)
          {
            g_TargetPos.z = 0;
            nav_state = 7;
            nav_timer = 0;
          }
          break;
        /* stop */
        case 7:
          g_FlagArm = SET;
          break;
        default:
          break;
        }
      }
      nav_loop_time_cnt = 0;
    }
    break;

  case 2:
    if (nav_loop_time_cnt > NAV_PERIOD)
    {
      if ((g_FlagHovering == SET) && (g_FlagFailSafe == RESET))
      {
        nav_timer += nav_loop_time_cnt;

        switch (nav_state)
        {
        /* take off */
        case 0:
          if (g_TargetPos.z < 0.60)
            g_TargetPos.z += 0.05;
          if (g_PosRef.z > 0.60)
          {
            g_TargetPos.z = 0.60;
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = SET;
            nav_state = 1;
            nav_timer = 0;
          }
          break;

        /* stabilize */
        case 1:
          if (nav_timer > 4.0f)
          {
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = RESET;
            g_UserEuler.roll = 3.2 * DEG2RAD;
            nav_state = 2;
            nav_timer = 0;
          }
          break;

        /* move right */
        case 2:
          if ((SlaveMCUData.bx == 0) && (SlaveMCUData.by == 0))
          {
            g_UserEuler.roll = 0.9 * DEG2RAD;
            nav_state = 3;
            nav_timer = 0;
          }
          break;

        /* get to conner 1 */
        case 3:
          if ((SlaveMCUData.lx == 0) && (SlaveMCUData.ly != 0))
          {
            g_UserEuler.roll = -2.0 * DEG2RAD;
            g_PosRef.y = g_CamOffset_y;
            g_VelRef.y = 0;
            nav_state = 4;
            nav_timer = 0;
          }
          break;

        /* stabilize */
        case 4:
          if (nav_timer > 0.5f)
          {
            g_UserEuler.roll = 0.0 * DEG2RAD;
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = SET;
          }
          if (nav_timer > 3.0f)
          {
            nav_state = 5;
            nav_timer = 0;
          }
          break;

        /* move forward */
        case 5:
          g_FlagEnableControlX = RESET;
          g_FlagEnableControlY = SET;
          if (SlaveMCUData.ly2 == 0)
            g_UserEuler.pitch = 3.2 * DEG2RAD;
          else
          {
            g_UserEuler.pitch = 1.0 * DEG2RAD;
            nav_state = 6;
            nav_timer = 0;
          }
          break;

        /* get to conner 2 */
        case 6:
          if (nav_timer > 5.0f)
            g_UserEuler.pitch = 1.1 * DEG2RAD;

          if ((SlaveMCUData.ly == 0) && (SlaveMCUData.lx2 != 0))
          {
            g_UserEuler.pitch = -2.0f * DEG2RAD;
            g_PosRef.x = g_CamOffset_x;
            g_VelRef.x = 0;
            nav_state = 7;
            nav_timer = 0;
          }
          break;

        /* stabilize */
        case 7:
          if (nav_timer > 0.5f)
          {
            g_UserEuler.pitch = 0 * DEG2RAD;
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = SET;
          }
          if (nav_timer > 3.0f)
          {
            nav_state = 8;
            nav_timer = 0;
          }
          break;

        /* move left */
        case 8:
          g_FlagEnableControlX = SET;
          g_FlagEnableControlY = RESET;
          if (SlaveMCUData.lx == 0)
          {
            if (nav_timer < 4.0f)
              g_UserEuler.roll = -2.2 * DEG2RAD;
            else
              g_UserEuler.roll = -2.3 * DEG2RAD;
          }
          else
          {
            g_UserEuler.roll = -0.8 * DEG2RAD;
            nav_state = 9;
            nav_timer = 0;
          }
          break;

        /* get to conner 3 */
        case 9:
          if ((SlaveMCUData.lx2 == 0) && (SlaveMCUData.ly2 != 0))
          {
            g_UserEuler.roll = 2.0f * DEG2RAD;
            g_PosRef.y = g_CamOffset_y;
            g_VelRef.y = 0;
            nav_state = 10;
            nav_timer = 0;
          }
          break;

        /* stabilize */
        case 10:
          if (nav_timer > 0.5f)
          {
            g_UserEuler.roll = 0.0f * DEG2RAD;
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = SET;
          }
          if (nav_timer > 3.0f)
          {
            nav_state = 11;
            nav_timer = 0;
          }
          break;

        /* move backward */
        case 11:

          break;

        default:
          break;
        }
      }
      nav_loop_time_cnt = 0;
    }
    break;

  case 3:
    if (nav_loop_time_cnt > NAV_PERIOD)
    {
      if ((g_FlagHovering == SET) && (g_FlagFailSafe == RESET))
      {
        nav_timer += nav_loop_time_cnt;

        switch (nav_state)
        {
        /* take off */
        case 0:
          if (g_TargetPos.z < 0.65f)
            g_TargetPos.z += 0.05;

          if (g_PosRef.z > 0.65)
          {
            g_TargetPos.z = 0.65;
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = SET;
            nav_state = 1;
            nav_timer = 0;
          }

          break;
        /* stablelize */
        case 1:
          if (nav_timer > 4.0f)
          {
            g_FlagEnableControlX = RESET;
            g_UserEuler.pitch = 2.0 * DEG2RAD;
            nav_state = 2;
            nav_timer = 0;
          }
          break;

        /* go ahead */
        case 2:
          if ((SlaveMCUData.bx == 0) && (SlaveMCUData.by == 0) && (SlaveMCUData.ly2 != 0))
          {
            g_UserEuler.pitch = 0.8 * DEG2RAD;
            nav_state = 3;
            nav_timer = 0;
          }
          break;

        /* stop accelerate */
        case 3:
          if ((SlaveMCUData.bx != 0) && (SlaveMCUData.by != 0))
          {
            g_UserEuler.pitch = -3.0f * DEG2RAD;
            g_PosRef.x = g_CamOffset_x = -0.20;
            g_VelRef.x = 0.2f;
            nav_state = 4;
            nav_timer = 0;
          }
          else if (nav_timer > 6.0f)
            g_UserEuler.pitch = 1.0 * DEG2RAD;
          break;

        /* catch ending point */
        case 4:
          if (nav_timer > 0.5f)
          {
            g_UserEuler.pitch = 0.0f * DEG2RAD;
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = SET;
            nav_state = 5;
            nav_timer = 0;
          }
          break;
        /* stabilize */
        case 5:
          if (nav_timer > 4.0f)
          {
            g_FlagEnableControlX = RESET;
            g_FlagEnableControlY = SET;
            g_UserEuler.pitch = -3.0 * DEG2RAD;
            nav_state = 6;
            nav_timer = 0;
          }
          break;

        /* come back */
        case 6:
          if ((SlaveMCUData.bx == 0) && (SlaveMCUData.by == 0) && (SlaveMCUData.ly != 0))
          {
            g_UserEuler.pitch = -0.3 * DEG2RAD;
            nav_state = 7;
            nav_timer = 0;
          }
          break;

        /* come back to the starting point */
        case 7:
          if ((SlaveMCUData.bx != 0) && (SlaveMCUData.by != 0))
          {
            g_UserEuler.pitch = 3.0f * DEG2RAD;
            g_PosRef.x = g_CamOffset_x = 0.2;
            g_VelRef.x = -0.2;
            nav_state = 8;
            nav_timer = 0;
          }
          else if (nav_timer > 6.0f)
            g_UserEuler.pitch = -0.5 * DEG2RAD;
          break;

        /* catching the starting point and stabilize */
        case 8:
          if (nav_timer > 0.5)
          {
            g_UserEuler.pitch = 0.0f * DEG2RAD;
            g_FlagEnableControlX = SET;
            g_FlagEnableControlY = SET;
          }
          if (nav_timer > 4.0f)
          {
            nav_state = 9;
            nav_timer = 0;
          }

          break;

        /* landing */
        case 9:
          if (g_TargetPos.z > 0.05)
            g_TargetPos.z -= 0.04;
          if (g_PosRef.z < 0.10)
          {
            g_TargetPos.z = 0;
            nav_state = 10;
            nav_timer = 0;
          }
          break;
        /* stop */
        case 10:
          g_FlagArm = SET;
          break;
        default:
          break;
        }
      }
      nav_loop_time_cnt = 0;
    }
    break;

  case 4:

    break;

  default:
    break;
  }
#endif

  /* rc data update 50Hz */
  if (rc_loop_time_cnt > RC_LOOP_PERIOD)
  {
    rc_loop_time_cnt = 0;
    rc_GetCommand(&g_RC);
    g_Thrust = g_RC.thrust;
    rc_GetTargetEuler(&g_RC, &g_RCEuler);
  }

  /* Tune */
  //g_Debug1 = PID_VEL_I_Z = 2.5 + 2.5*((g_RC.aux2 - 0.5)*2);

  /* arm or disarm */
  if ((g_FlagAttInit == SET) && (g_RC.ch3 < 0.10f) && (g_RC.ch4 < 0.10f) && (g_RC.ch2 < 0.10f) && (g_RC.ch1 > 0.80f))
  {
    /* wait 2 seconds */
    if (rc_arm_wait_time < 2.0f)
    {
      rc_arm_wait_time += main_loop_dt;
    }
    else
    {
      rc_arm_wait_time = 0;
      /* toggle g_ArmFlag */
      if (g_FlagArm == 0)
        g_FlagArm = SET;
      else
      {
        g_RCEuler.yaw = g_Euler.yaw;
        g_FlagArm = RESET;
      }
    }
  }
  else
    rc_arm_wait_time = 0;

  /* Slave communication loop */
  if (slave_com_loop_cnt > SLAVE_COM_PERIOD)
  {
    if (Slave_GetData(&SlaveMCUData) == DEF_OK)
    { /* Get data from slave MUC */
      /* delay to match the latency */
      euler_delay_pitch = DLPF(g_Euler.pitch, euler_delay_pitch, 1.5f * HZ2RAD, slave_com_loop_cnt);
      euler_delay_roll = DLPF(g_Euler.roll, euler_delay_roll, 1.5f * HZ2RAD, slave_com_loop_cnt);

      /* capture spot */
      if ((SlaveMCUData.bx != 0) && (SlaveMCUData.by != 0))
      {
        if (spot_capture_cnt < 10)
          spot_capture_cnt++;
        else
          flag_spot_capture = SET;
      }
      else
      {
        spot_capture_cnt = 0;
        flag_spot_capture = RESET;
      }

      /* X update */
      if ((SlaveMCUData.lx != 0) || (SlaveMCUData.lx2 != 0))
      {
        if (SlaveMCUData.lx == 0) /* lx is not avaliable */
          cam_raw_x = SlaveMCUData.lx2;
        else if (SlaveMCUData.lx2 == 0) /* lx2 is not avaliabel */
          cam_raw_x = SlaveMCUData.lx;
        else /* both ly and ly2 is avaliable */
          cam_raw_x = (SlaveMCUData.lx + SlaveMCUData.lx2) / 2.0f;

        if (flag_spot_capture == SET)
          cam_raw_x = (cam_raw_x + SlaveMCUData.bx) / 2.0f;

        g_CamOffset_x = DLPF(g_PosRef.z * (-(cam_raw_x - 14) / 38.5 + 1.42857f * sinf(euler_delay_pitch)), g_CamOffset_x, 2 * HZ2RAD, slave_com_loop_cnt);

        g_FlagCamUpdate_X = SET;
      }

      /* Y update */
      if ((SlaveMCUData.ly != 0) || (SlaveMCUData.ly2 != 0))
      {
        if (SlaveMCUData.ly == 0) /* ly is not avaliable */
          cam_raw_y = SlaveMCUData.ly2;

        else if (SlaveMCUData.ly2 == 0) /* ly2 is not avaliabel */
          cam_raw_y = SlaveMCUData.ly;

        else /* both ly and ly2 is avaliable */
          cam_raw_y = (SlaveMCUData.ly + SlaveMCUData.ly2) / 2.0f;

        if (flag_spot_capture == SET)
          cam_raw_y = (cam_raw_y + SlaveMCUData.by) / 2.0f;

        g_CamOffset_y = DLPF(g_PosRef.z * (-(cam_raw_y - 14) / 38.5 - 1.42857f * sinf(euler_delay_roll)), g_CamOffset_y, 2 * HZ2RAD, slave_com_loop_cnt);

        g_FlagCamUpdate_Y = SET;
      }
    }
    else
      g_Slave_SPI_ErrorCnt++;

    spi_sum++;
    slave_com_loop_cnt = 0;
  }

  /* Sonar Loop */
  if (last_sonar_raw != SlaveMCUData.sonar)
  {
    last_sonar_raw = SlaveMCUData.sonar;
    current_sonar = DLPF(SlaveMCUData.sonar * 1.04, current_sonar, 2 * HZ2RAD, sonar_loop_cnt);
    if ((current_sonar > 0.01f) && (current_sonar < 2.00f) && (max(fabs(g_Euler.pitch), fabs(g_Euler.roll)) < 15 * DEG2RAD) && (fabs(current_sonar - last_sonar) < 0.05))
    {
      g_SonarAlt = current_sonar * cosf(g_Euler.pitch) * cosf(g_Euler.roll);
      g_FlagSonarUpdate = SET;
    }
    last_sonar = current_sonar;
    sonar_loop_cnt = 0;
  }

  /* Set Height Mode */
  if ((fabs(g_Thrust - last_thrust) < 0.005f) && (g_FlagAttInit == SET) && (g_FlagTakeOff == SET) && (g_RC.aux1 < 0.8))
  {
    if (set_height_wait_time < 2)
    {
      set_height_wait_time += main_loop_dt;
    }
    else if (g_FlagSetHeight == RESET)
    {
      g_TargetPos.x = 0;
      g_TargetPos.y = -0.05;
      g_TargetPos.z = g_PosRef.z;

      g_FlagEnableControlX = RESET;
      g_FlagEnableControlY = RESET;

      g_FlagSetHeight = SET;
      if (g_RC.aux1 < 0.2)
        g_FlagHovering = SET;
    }
  }
  else
  {
    g_FlagSetHeight = RESET;
    g_FlagHovering = RESET;
    set_height_wait_time = 0;
  }
  last_thrust = g_Thrust;

  /* Fail safe loop */
  if (fail_safe_loop_cnt > FAIL_SAFE_PERIOD)
  {
    /* fail safe */
    if (g_FlagFailSafe == RESET)
    {
      if ((g_FlagHovering == SET) && (SlaveMCUData.ly == 0) && (SlaveMCUData.ly2 == 0) && (SlaveMCUData.lx == 0) && (SlaveMCUData.lx2 == 0) &&
          (SlaveMCUData.bx == 0) && (SlaveMCUData.by == 0) && (g_PosRef.z > 0.3f))
      {
        if (fail_safe_timeout < 3.0f)
          fail_safe_timeout += fail_safe_loop_cnt;
        else
        {
          g_FlagEnableControlX = RESET;
          g_FlagEnableControlY = RESET;
          g_FlagFailSafe = SET;
        }
      }
      else
      {
        fail_safe_timeout = 0;
        g_FlagFailSafe = RESET;
      }
    }
    else
    { /* Auto landing */
      if (g_TargetPos.z > 0.05)
        g_TargetPos.z -= 0.04;
      if (g_PosRef.z < 0.10)
      {
        g_TargetPos.z = 0;
        g_FlagArm = SET;
      }
    }
    fail_safe_loop_cnt = 0;
  }

  /* Serial Print Loop */
  if (serial_loop_time_cnt > SERIAL_PRINT_PERIOD)
  {
    serial_loop_time_cnt = 0;
    //BSP_Ser_Printf("%d,%d,%d,%d,\r\n",SlaveMCUData.bx, SlaveMCUData.by,SlaveMCUData.ly, SlaveMCUData.ly2);
    //BSP_Ser_Printf("%d,\r\n",nav_state);
    //BSP_Ser_Printf("%f,%f,\r\n",g_CamOffset_x,-g_PosRef.z*sinf(euler_delay_pitch));
    //BSP_Ser_Printf("%f,%f,\r\n",g_CamOffset_x, g_CamOffset_y);
    //BSP_Ser_Printf("Spot Capture:%s\r\n",flag_spot_capture?"SET":"RESET");
    //BSP_Ser_Printf("%f,%f,%f,%d,\r\n", g_TargetPos.z, g_TargetEuler.pitch*RAD2DEG, g_TargetEuler.roll*RAD2DEG,nav_state);
    //BSP_Ser_Printf("%f,%f,\r\n", g_PosRef.x, g_PosRef.y);
    //BSP_Ser_Printf("%f,%f,%f,%f,\r\n", g_CamOffset_y, g_PosRef.y, g_VelRef.y, PID_VEL_P_Y);
    //BSP_Ser_Printf("%f,%f,%f,%f,\r\n", g_CamOffset_x, g_PosRef.x, g_VelRef.x, PID_VEL_P_X);
    //BSP_Ser_Printf("%f,%f,\r\n",g_BaroVel,g_BaroAlt);
    //BSP_Ser_Printf("error:%f%%,\r\n",100*(float)g_Slave_SPI_ErrorCnt/spi_sum);
    //BSP_Ser_Printf("%d,%d,%d,%5.4f%%,\r\n", SlaveMCUData.bx, SlaveMCUData.by, SlaveMCUData.qual, 100 * (float)g_Slave_SPI_ErrorCnt / spi_sum);
    //BSP_Ser_Printf("%f,%f,\r\n", g_IR_Roll*RAD2DEG,g_IR_Picth*RAD2DEG);
    //    {
    //      extern uint8_t PixelData[128];
    //      extern uint8_t PixelDataFiltered[124];
    //      static int16_t i_tmp = -1;
    //
    //      if(i_tmp == -1)
    //        BSP_Ser_Printf("*LD00000000");
    //      else if(i_tmp <= 123)
    //        BSP_Ser_Printf("%X",PixelDataFiltered[i_tmp]);
    //      else{
    //  i_tmp = -2;
    //        BSP_Ser_Printf("#");
    //      }
    //      i_tmp ++;
    //    }
    //BSP_Ser_Printf("%f,%f,\r\n",RAD2DEG*fast_atan2(g_AccRaw.z,g_AccRaw.x),RAD2DEG*atan2(g_AccRaw.z,g_AccRaw.x));
    //BSP_Ser_Printf("%8.3f,\r\n",g_AccCurrentNorm);
    //BSP_Ser_Printf("%f,%f,\r\n",g_CCDPos_y,g_PosRef.y);
    //BSP_Ser_Printf("%f,%f,\r\n",g_IRDistance,g_PosRef.x);
    //BSP_Ser_Printf("%d,%d,%d,%d,%d,\r\n",temp[0],temp[1],temp[2],temp[3],temp[4]);
    //BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,\r\n",g_Debug1,g_Debug2,g_Debug3);
    //BSP_Ser_Printf("%8.3f,%8.3f,CMP\r\n",g_TargetPos.y,g_VelRef.y);
    //BSP_Ser_Printf("%8.3f,%8.3f,CMP\r\n",g_TargetPos.x,g_PosRef.x);
    //BSP_Ser_Printf("%8.3f,%8.3f,CMP\r\n",g_AccEstBody.x,g_AccRaw.x);
    //BSP_Ser_Printf("%8.3f,%8.3f,CMP\r\n",g_AccRaw.x,g_AccFiltered.x);
    //BSP_Ser_Printf("%8.3f,%8.3f,pitch\r\n",RAD2DEG *g_Euler.pitch,RAD2DEG *atan2f(g_AccRaw.x, 1.0f / InvSqrt(g_AccRaw.y * g_AccRaw.y + g_AccRaw.z * g_AccRaw.z)));
    //BSP_Ser_Printf("%8.3f,%8.3f,roll\r\n",RAD2DEG *g_Euler.roll,RAD2DEG *atan2f(-g_AccRaw.y, -g_AccRaw.z));
    //BSP_Ser_Printf("%8.3f,%8.3f, CMP\r\n", RAD2DEG * g_Euler.roll, RAD2DEG * g_TargetEuler.roll);
    //BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,ATT_E\r\n", RAD2DEG * g_Euler.yaw, RAD2DEG * g_Euler.pitch, RAD2DEG * g_Euler.roll);
    //BSP_Ser_Printf("%6.3f,%6.3f,%6.3f,%6.3f,\r\n",g_Quaternion.w,g_Quaternion.x,g_Quaternion.y,g_Quaternion.z);
    //BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,%8.3f,ACC\r\n",g_AccRaw.x,g_AccRaw.y,g_AccRaw.z,g_AccCurrentNorm);
    //BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,A\r\n",g_GyroRaw.x,g_GyroRaw.y,g_GyroRaw.z);
    //BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,A\r\n",g_AccRef.x,g_AccRef.y,g_AccRef.z);
    //BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,A\r\n",g_VelRef.x,g_VelRef.y,g_VelRef.z);
    //BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,A\r\n",g_PosRef.x,g_PosRef.y,g_PosRef.z);
    //BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,%8.3f,T\r\n", g_SonarAlt, g_PosRef.z, g_VelRef.z, g_AccRef.z);
    BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,%8.3f,%d,\r\n", g_CamOffset_x, g_PosRef.x, g_VelRef.x, g_AccRef.x, nav_state);
    //BSP_Ser_Printf("%8.3f,%8.3f,\r\n",g_VelRef.z,g_PosRef.z);
    //BSP_Ser_Printf("%f\r\n", serial_loop_time_cnt);
    //BSP_Ser_Printf("%d,%d,\r\n",px4flow.pixel_flow_x_sum,px4flow.pixel_flow_y_sum);
    //BSP_Ser_Printf("%8.3f,%8.3f,\r\n",optflow_px,optflow_py);
    //BSP_Ser_Printf("%f\r\n", g_AttLoopDt);
    //BSP_Ser_Printf("%d,%f,\r\n",g_FlagSetHeight,g_TargetPos.z);
    //BSP_Ser_Printf("%f,%f,\r\n",g_AccEstBody.z,g_AccRaw.z);
    //BSP_Ser_Printf("%f,%f,OPTFLOW\r\n",0.1*px4flow.gyro_x_rate,g_GyroFiltered.x);
    //BSP_Ser_Printf("iic error counter:%d\r\n",g_IMU_IIC_ErrorCnt);
    //BSP_Ser_Printf("%f,%f,OPTFLOW Q\r\n",px4flow.ground_distance/1000.0f,g_PosRef.z);
    //BSP_Ser_Printf("%8.3f,%8.3f,%3d,OPTFLOW Vel\r\n", g_OptFlow_x, g_OptFlow_y,px4flow.qual);
    //BSP_Ser_Printf("iic error counter:%d\r\n",g_OPTFLOW_IIC_ErrorCnt);
    //BSP_Ser_Printf("%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,\r\n",g_RC.ch1,g_RC.ch2,g_RC.ch3,g_RC.ch4,g_RC.ch5,g_RC.ch6);
    //BSP_Ser_Printf("%f,SONAR\r\n",g_SonarAlt);
    //BSP_Ser_Printf("%8.3f,%8.3f,%8.3f,TRG\r\n",RAD2DEG*g_TargetEuler.yaw,RAD2DEG*g_TargetEuler.pitch,RAD2DEG*g_TargetEuler.roll);
  }

  if ((fabs(g_Euler.pitch) > 50 * DEG2RAD) || (fabs(g_Euler.roll) > 50 * DEG2RAD))
    g_FlagArm = SET;

  /* Take off indicator */
  if ((g_Thrust > 0.15) && (g_FlagArm == RESET))
    g_FlagTakeOff = SET;
  else
    g_FlagTakeOff = RESET;

  /* show arm or disarm */
  if ((g_FlagArm == SET) && (g_FlagAttInit == SET))
    LED = HIGH; /* LED on */
  else
    LED = LOW; /* LED off */
}

/*
*********************************************************************
*     Attitude and Control loop period = 5ms
*********************************************************************
*/
void flight_control(void)
{
  static MPU6050_TYPE mpu6050;
  static uint16_t timestamp;
  static long att_loop_cnt = 0;
  static float pid_thrust = 0;
  static VECTOR error_pos_p = {0, 0, 0};
  static VECTOR error_vel_p = {0, 0, 0};
  static VECTOR error_vel_i = {0, 0, 0};
  static VECTOR error_vel_d = {0, 0, 0};
  static VECTOR vel_control_b = {0, 0, 0};
  static VECTOR vel_control_r = {0, 0, 0};
  static VECTOR acc_ref;
  static EULER euler_delta;
  static float dt, half_dt_square;
  static float sin_yaw, cos_yaw;
  float tmp;

  //DBG_PIN = HIGH;
  dt = g_AttLoopDt = g_Getdt(&timestamp);

  /* Get IMU data */
  if (SensorUpdate_GetGyroAccRaw(&g_GyroRaw, &g_AccRaw) == DEF_FAIL)
    g_IMU_IIC_ErrorCnt++;

  /* Start attitude estimate */
  g_AccFiltered.x = DLPF(g_AccRaw.x, g_AccFiltered.x, 3 * HZ2RAD, dt);
  g_AccFiltered.y = DLPF(g_AccRaw.y, g_AccFiltered.y, 3 * HZ2RAD, dt);
  g_AccFiltered.z = DLPF(g_AccRaw.z, g_AccFiltered.z, 3 * HZ2RAD, dt);

  //euler_delta.yaw   = applyDeadband(g_GyroRaw.z * dt, 0.00015f);
  euler_delta.yaw = g_GyroRaw.z * dt;
  euler_delta.pitch = g_GyroRaw.y * dt;
  euler_delta.roll = g_GyroRaw.x * dt;
  EulerRoateVectSmall(&g_AccEstBody, &euler_delta);

  //g_AccCurrentNorm = 1.0f / InvSqrt(g_AccRaw.x * g_AccRaw.x + g_AccRaw.y * g_AccRaw.y + g_AccRaw.z * g_AccRaw.z);

  /* fusion */
  g_AccEstBody.x = g_AccEstBody.x + ATTITUDE_FUSION_FACTOR_X * (g_AccFiltered.x - g_AccEstBody.x);
  g_AccEstBody.y = g_AccEstBody.y + ATTITUDE_FUSION_FACTOR_Y * (g_AccFiltered.y - g_AccEstBody.y);
  g_AccEstBody.z = g_AccEstBody.z + ATTITUDE_FUSION_FACTOR_Z * (g_AccFiltered.z - g_AccEstBody.z);

  g_Euler.pitch = atan2f(g_AccEstBody.x, 1.0f / InvSqrt(g_AccEstBody.y * g_AccEstBody.y + g_AccEstBody.z * g_AccEstBody.z));
  g_Euler.roll = atan2f(-g_AccEstBody.y, -g_AccEstBody.z);

  /* calculate yaw */
  {
    float cosRoll;
    float sinRoll;
    float cosPitch;
    float sinPitch;
    float Xh, Yh;
    static uint8_t normalize_trig = 0;

    EulerRoateVectSmall(&g_MagEstBody, &euler_delta);
    fast_SinCos(g_Euler.roll, &sinRoll, &cosRoll);
    fast_SinCos(g_Euler.pitch, &sinPitch, &cosPitch);
    Xh = g_MagEstBody.x * cosPitch + g_MagEstBody.y * sinRoll * sinPitch + g_MagEstBody.z * sinPitch * cosRoll;
    Yh = g_MagEstBody.y * cosRoll - g_MagEstBody.z * sinRoll;

    /* trigger to normalize mag vector */
    if (normalize_trig >= 5)
    {
      normalize_trig = 0;
      VectNormalize(&g_MagEstBody);
    }
    else
      normalize_trig++;

    g_Euler.yaw = atan2f(Xh, Yh);
  }
  /* End attitude estimate */

  /* Get reference acceleration */
  EulerRoateVectXY(&acc_ref, &g_AccRaw, &g_Euler);

  if (att_loop_cnt < 1000)
  {
    if (att_loop_cnt >= 500)
      VectAdd(&g_AccRefOffset, &g_AccRefOffset, &acc_ref);
    VectScale(&g_AccRef, 0);
    att_loop_cnt++;
  }
  else if (att_loop_cnt == 1000)
  {
    VectScale(&g_AccRefOffset, 1.0f / 500);
    VectScale(&g_AccRef, 0);
    att_loop_cnt++;
    g_FlagAttInit = SET; /* Indicate attitude init ok */
  }
  else
  {
    VectSub(&acc_ref, &acc_ref, &g_AccRefOffset);
    VectScale(&acc_ref, -1);
    g_AccRef.x = DLPF(acc_ref.x, g_AccRef.x, 10 * HZ2RAD, dt);
    g_AccRef.y = DLPF(acc_ref.y, g_AccRef.y, 10 * HZ2RAD, dt);
    g_AccRef.z = DLPF(acc_ref.z, g_AccRef.z, 10 * HZ2RAD, dt);
    //VectCopy(&acc_ref, &g_AccRef);
  }

  /* Start position estimate */

  half_dt_square = 0.5 * dt * dt;

  if (g_FlagAttInit == SET)
  {

    /* posteriori estimate of acc reference frame */
    //if (g_FlagOptflowUpdate == SET) {
    //  g_AccRef.x = g_AccRef.x + 0.001f * (g_OptflowQuality / 255.0f) * (g_OptFlow_x - g_VelRef.x);
    //  g_AccRef.y = g_AccRef.y + 0.001f * (g_OptflowQuality / 255.0f) * (g_OptFlow_y - g_VelRef.y);
    //}
    if ((g_FlagSonarUpdate == SET) || STRESS_TEST)
    {
      g_AccRef.z = g_AccRef.z + 0.015 * (g_SonarAlt - g_PosRef.z);
    }

    /* a prior estimate */
    if ((g_PosRef.z > 0.30) || STRESS_TEST)
    {
      g_VelRef.x = g_VelRef.x + g_AccRef.x * dt;
      g_PosRef.x = Constraint_f(g_PosRef.x + g_VelRef.x * dt + g_AccRef.x * half_dt_square, -0.5, 0.5);
    }
    if ((g_PosRef.z > 0.30) || STRESS_TEST)
    {
      g_VelRef.y = g_VelRef.y + g_AccRef.y * dt;
      g_PosRef.y = Constraint_f(g_PosRef.y + g_VelRef.y * dt + g_AccRef.y * half_dt_square, -0.5, 0.5);
    }

    g_VelRef.z = g_VelRef.z + g_AccRef.z * dt;
    g_PosRef.z = Constraint_f(g_PosRef.z + g_VelRef.z * dt + g_AccRef.z * half_dt_square, 0, 3);

    /* a posteriori estimate */

    /* Sonar */
    if ((g_FlagSonarUpdate == SET) || STRESS_TEST)
    {
      g_FlagSonarUpdate = RESET;
      tmp = g_SonarAlt - g_PosRef.z; /* position error */
      g_PosRef.z = g_PosRef.z + KALMAN_POS_K1_Z * tmp;
      g_VelRef.z = g_VelRef.z + KALMAN_POS_K2_Z * tmp;
    }

    /* Camera stablelized */
    if ((g_FlagCamUpdate_Y == SET) || STRESS_TEST)
    {
      g_FlagCamUpdate_Y = RESET;
      tmp = (g_CamOffset_y - g_PosRef.y); /* position error */
      g_PosRef.y = g_PosRef.y + KALMAN_POS_K1_Y * tmp;
      g_VelRef.y = g_VelRef.y + KALMAN_POS_K2_Y * tmp;
    }
    if ((g_FlagCamUpdate_X == SET) || STRESS_TEST)
    {
      g_FlagCamUpdate_X = RESET;
      tmp = (g_CamOffset_x - g_PosRef.x); /* position error */
      g_PosRef.x = g_PosRef.x + KALMAN_POS_K1_X * tmp;
      g_VelRef.x = g_VelRef.x + KALMAN_POS_K2_X * tmp;
    }
  }
  /* End position estimate */

  /* Position Control */

  /* Position Z control */
  if ((g_FlagSetHeight == SET) || STRESS_TEST)
  {
    error_pos_p.z = DLPF(PID_POS_P_Z * Constraint_f(g_TargetPos.z - g_PosRef.z, -1, 1), error_pos_p.z, 6 * HZ2RAD, dt);
    error_vel_p.z = DLPF(error_pos_p.z - g_VelRef.z, error_vel_p.z, 5 * HZ2RAD, dt);
    error_vel_i.z = Constraint_f(error_vel_i.z + error_vel_p.z * dt, -0.2, 0.2);
    error_vel_d.z = DLPF(-g_AccRef.z, error_vel_d.z, 10 * HZ2RAD, dt);
    pid_thrust = PID_VEL_P_Z * error_vel_p.z + PID_VEL_I_Z * error_vel_i.z + PID_VEL_D_Z * error_vel_d.z;
  }
  else
  {
    error_vel_i.z = 0;
    pid_thrust = -0.02f * g_AccRef.z;
  }
  /* End Positon Z PID */

  /* Start Position XY control */
  if ((g_FlagHovering == SET) || STRESS_TEST)
  {
    fast_SinCos(g_Euler.yaw, &sin_yaw, &cos_yaw);

    error_pos_p.x = PID_POS_P_X * (g_TargetPos.x - g_PosRef.x);
    error_vel_p.x = error_pos_p.x - g_VelRef.x;
    error_vel_i.x = Constraint_f(error_vel_i.x + error_vel_p.x * dt, -20, 20);
    error_vel_d.x = -g_AccRef.x;

    error_pos_p.y = PID_POS_P_Y * (g_TargetPos.y - g_PosRef.y);
    error_vel_p.y = error_pos_p.y - g_VelRef.y;
    error_vel_i.y = Constraint_f(error_vel_i.y + error_vel_p.y * dt, -20, 20);
    error_vel_d.y = -g_AccRef.y;

    vel_control_r.x = PID_VEL_P_X * error_vel_p.x + PID_VEL_I_X * error_vel_i.x + PID_VEL_D_X * error_vel_d.x;
    vel_control_r.y = PID_VEL_P_Y * error_vel_p.y + PID_VEL_I_Y * error_vel_i.y + PID_VEL_D_Y * error_vel_d.y;

    vel_control_b.x = cos_yaw * vel_control_r.x + sin_yaw * vel_control_r.y;
    vel_control_b.y = -sin_yaw * vel_control_r.x + cos_yaw * vel_control_r.y;

    if (g_PosRef.z > 0.30f)
    {

      if (g_FlagEnableControlX == SET)
        g_NavEuler.pitch = Constraint_f(vel_control_b.x, -8.0f, 8.0f) * DEG2RAD;
      else
      {
        error_vel_i.x = 0;
        g_NavEuler.pitch = 0;
      }

      if (g_FlagEnableControlY == SET)
        g_NavEuler.roll = -Constraint_f(vel_control_r.y, -8.0f, 8.0f) * DEG2RAD;
      else
      {
        error_vel_i.y = 0;
        g_NavEuler.roll = 0;
      }
    }
    else
    { /* if optical flow is not available */
      //flow_euler.pitch = 0;
      //flow_euler.roll = 0;
      g_NavEuler.pitch = Constraint_f(PID_VEL_D_X * error_vel_d.x, -8.0f, 8.0f) * DEG2RAD;
      g_NavEuler.roll = -Constraint_f(PID_VEL_D_Y * error_vel_d.y, -8.0f, 8.0f) * DEG2RAD;
    }
  }
  else
  {
    g_NavEuler.pitch = 0;
    g_NavEuler.roll = 0;
    error_vel_d.x = error_vel_d.y = 0;
    error_vel_i.x = error_vel_i.y = 0;
  }
  g_NavEuler.yaw = 0;

  /* End Position XY PID */

  /* Get target euler */
  g_TargetEuler.yaw = g_RCEuler.yaw;
  g_TargetEuler.pitch = g_RCEuler.pitch + g_NavEuler.pitch + Constraint_f(g_UserEuler.pitch, -10 * DEG2RAD, 10 * DEG2RAD);
  g_TargetEuler.roll = g_RCEuler.roll + g_NavEuler.roll + Constraint_f(g_UserEuler.roll, -10 * DEG2RAD, 10 * DEG2RAD) - 0 * DEG2RAD;

  /* Start contorl */
  if ((g_FlagArm == RESET) || STRESS_TEST)
  {
    if (g_FlagTakeOff == SET)
      CTRL_PIDRegulator(g_Euler, g_TargetEuler, g_GyroRaw, g_Thrust + pid_thrust, &g_Control, dt, 1);
    else
      CTRL_PIDRegulator(g_Euler, g_TargetEuler, g_GyroRaw, g_Thrust, &g_Control, dt, 0);

    Motor_SetPWM(&g_Control, &g_Motor);
  }
  else
    Motor_MinThrust(&g_Motor);
  /* End control */

  //g_GyroFiltered.x = DLPF(g_GyroRaw.x, g_GyroFiltered.x, 5 * HZ2RAD, dt);
  //g_GyroFiltered.y = DLPF(g_GyroRaw.y, g_GyroFiltered.y, 5 * HZ2RAD, dt);
  //g_GyroFiltered.z = DLPF(g_GyroRaw.z, g_GyroFiltered.z, 5 * HZ2RAD, dt);

  //DBG_PIN = LOW;
}
/* End user code. Do not edit comment generated here */
