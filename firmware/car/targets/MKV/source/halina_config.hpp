/**
 * Copyright (c) Kolo Naukowe Elektronikow, Akademia Gorniczo-Hutnicza im. Stanislawa Staszica w Krakowie 2020
 * Authors: Arkadiusz Balys, Kamil Kasperczyk, Witold Lukasik
 *
 * NXP HALina configuration file
 *
 */

#pragma once

#include "MKV58F24.h"

/////////////////////// UART ///////////////////////
#define UART					UART0
#define UART_PORT				PORTA
#define UART_RX_PIN				14U
#define UART_TX_PIN				15U
#define UART_BAUDRATE			115200

#define DMA						DMA0

/////////////////////// LOG ///////////////////////
#define ENABLE_LOG				1


/////////////////////// DISPLAY ///////////////////////
#define DISPLAY_GPIO			GPIOC
#define DISPLAY_PORT			PORTC

// set refresh rate when using in systic
#define REFRESH_RATE_HZ			10
#define SYSTICK_RATE_HZ			1000

constexpr uint32_t DISPLAY_A1 = ((uint32_t)(1u<<0u));
constexpr uint32_t DISPLAY_A2 = ((uint32_t)(1u<<1u));
constexpr uint32_t DISPLAY_A3 = ((uint32_t)(1u<<2u));
constexpr uint32_t DISPLAY_A4 = ((uint32_t)(1u<<5u));

constexpr uint32_t DISPLAY_A = ((uint32_t)(1u<<8u));
constexpr uint32_t  DISPLAY_B =	((uint32_t)(1u<<9u));
constexpr uint32_t  DISPLAY_C =	((uint32_t)(1u<<11u));
constexpr uint32_t  DISPLAY_D =	((uint32_t)(1u<<14u));
constexpr uint32_t  DISPLAY_E =	((uint32_t)(1u<<13u));
constexpr uint32_t  DISPLAY_F =	((uint32_t)(1u<<10u));
constexpr uint32_t  DISPLAY_G =	((uint32_t)(1u<<15u));
constexpr uint32_t  DISPLAY_DP = ((uint32_t)(1u<<12u));

/////////////////////// MOTORS ///////////////////////
#define MOTOR_CLOCK_PRESCALER 		1
#define MOTOR_CLOCK_INPUT			240000000
#define MOTOR_CLOCK_OUTPUT			10000
#define MOTOR_CLOCK_MOD				MOTOR_CLOCK_INPUT/MOTOR_CLOCK_PRESCALER/MOTOR_CLOCK_OUTPUT
