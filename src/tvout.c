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
 * tvout.c
 *
 *  Created on: 2014-06-18
 *      Author: Jacques Deschênes
 */

#include <stdint.h>
#include <LPC8xx.h>
#include "tvout.h"


volatile unsigned char video_buffer[BUFFER_SIZE];
volatile uint16_t ln_cntr=0;
volatile unsigned frame_cntr=0;

// céduleur de tâches et génération signal vidéo
void SCT_IRQHandler (void){
	uint16_t first,i;
	if (LPC_SCT->EVFLAG & 2){ // event 1
		ln_cntr++;
		switch (ln_cntr){
		case 3:
			PULSE_WIDTH_REG=H_PULSE;
			break;
		case FIRST_VISIBLE:
			LPC_SCT->EVEN |= 4u; // active interruption sur événement 2
			break;
		case LAST_VISIBLE+1:
			LPC_SCT->EVEN &= ~4u; // désactive interruption sur événement 2
		    break;
		case LINES_PER_FRAME-1:
			ln_cntr=0;
			PULSE_WIDTH_REG=V_PULSE;
			frame_cntr++;
			break;
		default:
			break;
		}//switch
		LPC_SCT->EVFLAG |= 2u; // remet à zéro l'indicateur énévenement 1
	}else {//événement 2
		first=((ln_cntr-FIRST_VISIBLE)>>2)*ROW_SIZE;
		LPC_SPI0->CFG|=SPI_ENABLE;
		LPC_SPI0->TXDAT=0;
		while (!(LPC_SPI0->STAT&SPI_TXRDY));
		for (i=ROW_SIZE;i;i--){
			LPC_SPI0->TXDAT=video_buffer[first++];
			while (!(LPC_SPI0->STAT&SPI_TXRDY));
		}
		LPC_SPI0->TXDAT=0;
		while (!(LPC_SPI0->STAT&SPI_TXRDY));
		LPC_SPI0->CFG&=~SPI_ENABLE;
		LPC_SCT->EVFLAG |= 4u; // remet à zéro l'indicateur événement 2
	}//if
}//SCT_IRQHandler()

void frame_delay(int n){
	int t0,l;
	l=ln_cntr;
	t0=frame_cntr+n;
	while ((frame_cntr<t0)||(l!=ln_cntr));
}//f()

void frame_sync(){// attend le prochain frame
	int i;
	i=frame_cntr;
	while (frame_cntr==i);
}//f()


