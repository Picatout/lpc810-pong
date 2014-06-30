 /*
* Copyright 2014, Jacques Deschênes
* This file is part of LPC810-pong.
*
*     LPC810-pong is free software: you can redistribute it and/or modify
*     it under the terms of the GNU General Public License as published by
*     the Free Software Foundation, either version 3 of the License, or
*     (at your option) any later version.
*
*     VPC-32 is distributed in the hope that it will be useful,
*     but WITHOUT ANY WARRANTY; without even the implied warranty of
*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*     GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public License
*     along with LPC810-pong.  If not, see <http://www.gnu.org/licenses/>.
*/


/*
 * hardware.h
 *
 *  Created on: 2014-06-18
 *      Author: Jacques
 */

#ifndef LPC810_H_
#define LPC810_H_

#include <LPC8xx.h>

// assignation des broches
#define SYNC_OUT  (4) // NTSC sync signal output PIO0_4
#define VIDEO_OUT (3) // NTSC video signal output PIO0_3
#define AUDIO_OUT (2) // audio output  PIO0_2
#define PADDLE_1  (0) // paddle 1 analog input PIO0_0
#define PADDLE_2  (1) // paddle 2 analog input PIO0_1

// special function registers used by tvout
#define PULSE_WIDTH_REG  LPC_SCT->MATCHREL[1].L

//-------------------------
#define LEFT_POT (0)
#define RIGHT_POT (1)

//---------------------------

#define SPI_RXRDY (1)
#define SPI_TXRDY (2)
#define SPI_MSTIDLE (1u<<8)
#define SPI_ENABLE (1)
#define SPI_MASTER (4)
#define SPI_EOT (20)
#define SPI_EOF (21)
#define SPI_IGNRX  (22)
#define SPI_DATLEN  (24)
#define SPI_TXSSEL_N (16)

#define m_set_output_pin(bit) LPC_GPIO_PORT->DIR0|=(bit)

#define SYS_CLOCK (30000000UL)  // fréquence SYS_CLOCK 30Mhz
#define MSEC (SYS_CLOCK/1000)   // Tcy/milliseconde
#define USEC (SYS_CLOCK/1000000) // Tcy/µsec

// bits dans PINENABLE0
#define  DIS_ACMP_I1 (1<<0)  // analog compare input 1   on PIO0_0
#define  DIS_ACMP_I2 (1<<1)  // analog compare input 2   on PIO0_1
#define  DIS_SWCLK  (1<<2)   // single wire debug clock  on PIO0_3
#define  DIS_SWDIO   (1<<3)  // single wire debug data i/o  on PIO0_2
#define  DIS_XTALIN  (1<<4)  // disable crystal in on PIO0_8
#define  DIS_XTALOUT (1<<5)  // disable crystal out on PIO0_9
#define  DIS_RESET (1<<6)    // disable reset on PIO0_5
#define  DIS_CLKIN (1<<7)    // disable clkin on PIO0_1
#define  DIS_VDDCMP (1<<8)   // disable Vdd compare on PIO0_6

#define m_disable_pin(pins) LPC_SWM->PINENABLE0 |= (pins)

// bits dans SYSAHBCLKCTRL
#define  CLK_SYS (1<<0)
#define  CLK_ROM (1<<1)
#define  CLK_RAM (1<<2)
#define  CLCK_FLASHREG (1<<3)
#define  CLK_FLASH (1<<4)
#define  CLCK_I2C  (1<<5)
#define  CLK_GPIO (1<<6) // bit activation signal clock sur GPIO L
#define  CLK_SWM  (1<<7)
#define  CLK_SCT  (1<<8)  // bit activation signal clocks ur SCT
#define  CLK_WKT  (1<<9)
#define  CLK_MRT (1<<10)  // bit activation MRT
#define  CLK_SPI0 (1<<11)
#define  CLK_SPI1 (1<<12)
#define  CLK_CRC  (1<<13)
#define  CLK_UART0 (1<<14)
#define  CLK_UART1 (1<<15)
#define  CLK_UART2 (1<<16)
#define  CLK_WWDT (1<<17)
#define  CLK_IOCON (1<<18)
#define  CLK_ACMP (1<<19)

// initialisation du signal clock pour les périphériques utilisés
#define  m_periph_clock_enable(peripherals)  LPC_SYSCON->SYSAHBCLKCTRL |= (peripherals)


// bits dans PRESETCTRL
#define  RESET_SPI0 (1<<0)
#define  RESET_SPI1 (1<<1)
#define  RESET_UARTFRG (1<<2)
#define  RESET_UART0  (1<<3)
#define  RESET_UART1 (1<<4)
#define  RESET_UART2 (1<<5)
#define  RESET_I2C (1<<6)
#define  RESET_MRT (1<<7)
#define  RESET_SCT (1<<8)
#define  RESET_WKT (1<<9)
#define  RESET_GPIO (1<<10)
#define  RESET_FLASH (1<<11)
#define  RESET_ACMP (1<<12)


// reset les périphériques
#define m_periph_reset(peripheral)	{LPC_SYSCON->PRESETCTRL &=~(peripheral);LPC_SYSCON->PRESETCTRL |= (peripheral);}


#define SCT_IRQ_EVENT_0 (0)
#define SCT_IRQ_EVENT_1 (1)


void initialize_hardware();

void delay_ms(uint32_t n);
void delay_us(uint32_t n);
void tone(uint32_t freq, uint32_t msec);
void play_tune(const int *tune, int pause);
int read_pot(int pot);

#endif /* LPC810_H_ */
