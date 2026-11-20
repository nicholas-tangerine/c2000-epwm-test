//#############################################################################
//
// FILE:   epwm_ex2_updown_aq.c
//
// TITLE:  ePWM Action Qualifier Module - Using up/down count without ISR.
//
//! \addtogroup driver_example_list
//! <h1> ePWM Up Down Count Action Qualifier</h1>
//!
//! This example configures ePWM1, ePWM2, ePWM4, ePWM5, ePWM6, and ePWM8 to produce
//! square wave PWM signals on board power up with independent duty cycles
//! on ePWMxA (33%) and ePWMxB (67%).
//!
//! No ISR or dynamic comparator modification is used. PWM signals run
//! continuously in hardware once clocks are enabled.
//!
//! PWM channels configured:
//!   - ePWM1A / ePWM1B: GPIO0 / GPIO1
//!   - ePWM2A / ePWM2B: GPIO2 / GPIO3
//!   - ePWM4A / ePWM4B: GPIO6 / GPIO7
//!   - ePWM5A / ePWM5B: GPIO8 / GPIO9
//!   - ePWM6A / ePWM6B: GPIO10 / GPIO11
//!   - ePWM8A / ePWM8B: GPIO99 / GPIO75
//!
//! Note: ePWM3 (GPIO4 & GPIO5) is explicitly excluded.
//!
//! View the ePWM waveforms on an oscilloscope at 50 kHz.
//
//#############################################################################
//
//
// 
// C2000Ware v26.01.00.00
//
// Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

//
// Defines
//
// 50 kHz PWM frequency calculation:
// SYSCLK = 200 MHz, EPWMCLK = SYSCLK / 2 = 100 MHz (default)
// Clock prescalers: CLKDIV = 1, HSPCLKDIV = 1 => TBCLK = 100 MHz
// Up-Down count mode: Period = 2 * TBPRD / TBCLK
// For 50 kHz: TBPRD = 100 MHz / (2 * 50 kHz) = 1000 ticks.
//
#define PWM_TBPRD           1000U

// Duty cycles:
// Channel A: 33% duty cycle -> CMPA = 670 (active high between CMPA up & down = 660 / 2000 ticks)
// Channel B: 67% duty cycle -> CMPB = 330 (active high between CMPB up & down = 1340 / 2000 ticks)
#define PWM_CMPA_33_DUTY     670U
#define PWM_CMPB_67_DUTY     330U

//
// Function Prototypes
//
void initEPWMGPIO(void);
void initEPWM(uint32_t base, uint16_t period, uint16_t cmpA, uint16_t cmpB);

//
// Main
//
void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pull ups.
    //
    Device_initGPIO();

    //
    // Set EPWM clock divider (EPWMCLK = SYSCLK / 2 = 100 MHz)
    //
    SysCtl_setEPWMClockDivider(SYSCTL_EPWMCLK_DIV_2);

    //
    // Enable peripheral clocks for EPWM1, EPWM2, EPWM4, EPWM5, EPWM6, and EPWM8
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM1);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM2);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM4);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM5);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM6);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM8);

    //
    // Configure GPIO0-3, GPIO6-11, GPIO99, and GPIO75 to ePWM pin-mux functions and disable pull-ups
    // (GPIO4 and GPIO5 for PWM3 are excluded)
    //
    initEPWMGPIO();

    //
    // Disable sync (freeze clock to PWM modules while configuring)
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //
    // Configure ePWM1, ePWM2, ePWM4, ePWM5, ePWM6, and ePWM8 modules
    // Each module runs at 50 kHz: Channel A = 33% duty, Channel B = 67% duty
    //
    initEPWM(EPWM1_BASE, PWM_TBPRD, PWM_CMPA_33_DUTY, PWM_CMPB_67_DUTY);
    initEPWM(EPWM2_BASE, PWM_TBPRD, PWM_CMPA_33_DUTY, PWM_CMPB_67_DUTY);
    initEPWM(EPWM4_BASE, PWM_TBPRD, PWM_CMPA_33_DUTY, PWM_CMPB_67_DUTY);
    initEPWM(EPWM5_BASE, PWM_TBPRD, PWM_CMPA_33_DUTY, PWM_CMPB_67_DUTY);
    initEPWM(EPWM6_BASE, PWM_TBPRD, PWM_CMPA_33_DUTY, PWM_CMPB_67_DUTY);
    initEPWM(EPWM8_BASE, PWM_TBPRD, PWM_CMPA_33_DUTY, PWM_CMPB_67_DUTY);

    //
    // Enable sync and clock to PWM modules (starts PWM outputs synchronously)
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //
    // IDLE loop. PWM signals continue driving out automatically from hardware.
    //
    for(;;)
    {
        NOP;
    }
}

//
// initEPWMGPIO - Configure GPIO0-3 and GPIO6-11 as ePWM pin-mux with pull-ups disabled
//
void initEPWMGPIO(void)
{
    //
    // Configure GPIO0 through GPIO3 as ePWM1A/B and ePWM2A/B
    //
    GPIO_setPinConfig(GPIO_0_EPWM1_A);
    GPIO_setPinConfig(GPIO_1_EPWM1_B);
    GPIO_setPinConfig(GPIO_2_EPWM2_A);
    GPIO_setPinConfig(GPIO_3_EPWM2_B);

    //
    // Configure GPIO6 through GPIO11 as ePWM4A/B, ePWM5A/B, and ePWM6A/B
    // (Skipping GPIO4 and GPIO5 which correspond to ePWM3)
    //
    GPIO_setPinConfig(GPIO_6_EPWM4_A);
    GPIO_setPinConfig(GPIO_7_EPWM4_B);
    GPIO_setPinConfig(GPIO_8_EPWM5_A);
    GPIO_setPinConfig(GPIO_9_EPWM5_B);
    GPIO_setPinConfig(GPIO_10_EPWM6_A);
    GPIO_setPinConfig(GPIO_11_EPWM6_B);

    //
    // Configure GPIO99 as ePWM8A and GPIO75 as ePWM8B
    //
    GPIO_setPinConfig(GPIO_99_EPWM8_A);
    GPIO_setPinConfig(GPIO_75_EPWM8_B);

    //
    // Disable pull-ups on all active ePWM GPIO pins (set as standard push-pull output)
    //
    GPIO_setPadConfig(0, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(1, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(2, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(3, GPIO_PIN_TYPE_STD);

    GPIO_setPadConfig(6, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(7, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(8, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(9, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(10, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(11, GPIO_PIN_TYPE_STD);

    GPIO_setPadConfig(99, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(75, GPIO_PIN_TYPE_STD);
}

//
// initEPWM - Configure Time-Base, Counter-Compare, and Action-Qualifier for an ePWM module
//
void initEPWM(uint32_t base, uint16_t period, uint16_t cmpA, uint16_t cmpB)
{
    //
    // Set up Time-Base Period, Counter, and Phase Shift
    //
    EPWM_setTimeBasePeriod(base, period);
    EPWM_setPhaseShift(base, 0U);
    EPWM_setTimeBaseCounter(base, 0U);

    //
    // Up-Down count mode, disable phase load
    //
    EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_disablePhaseShiftLoad(base);

    //
    // TBCLK prescaler = /1, High-Speed TBCLK prescaler = /1
    // TBCLK = EPWMCLK / (1 * 1) = 100 MHz
    //
    EPWM_setClockPrescaler(base, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);

    //
    // Set Compare values
    //
    EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, cmpA);
    EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, cmpB);

    //
    // Load Compare registers on counter zero (shadow mode)
    //
    EPWM_setCounterCompareShadowLoadMode(base,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(base,
                                         EPWM_COUNTER_COMPARE_B,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    //
    // Action Qualifier configuration for Channel A:
    // - Force LOW on counter zero
    // - Drive HIGH when counting UP reaches CMPA
    // - Drive LOW when counting DOWN reaches CMPA
    // Result: Active HIGH centered pulse (duty cycle = 2 * (period - cmpA) / (2 * period))
    //
    EPWM_setActionQualifierAction(base,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(base,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(base,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

    //
    // Action Qualifier configuration for Channel B:
    // - Force LOW on counter zero
    // - Drive HIGH when counting UP reaches CMPB
    // - Drive LOW when counting DOWN reaches CMPB
    // Result: Active HIGH centered pulse (duty cycle = 2 * (period - cmpB) / (2 * period))
    //
    EPWM_setActionQualifierAction(base,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(base,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(base,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);

    //
    // Set emulation mode so PWM continues to run if debugger is connected/halted
    //
    EPWM_setEmulationMode(base, EPWM_EMULATION_FREE_RUN);
}
