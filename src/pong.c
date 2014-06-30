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
===============================================================================
Name        : pong.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : jeux pong
               LPC_SCT utilisé pour générer synchro vidéo et sortie audio en PWM
               LPC_SPI0 utilisé pour sortie les bits vidéo
               LPC_ACMP0 utilisé pour lire les 2 potentiomètres

           assignations des broches
               - sortie synchronisation NTSC sur PIO0_4
               - sortie bits vidéo sur PIO0_3
               - sortie PWM audio sur PIO0_2
               - entrée potentiomètre 1 sur PIO0_0
               - entrée potentiomètre 2 sur PIO0_1
               - fonction RESET sur PIO0_5
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC8xx.h"
#endif

#include <cr_section_macros.h>

#include "hardware.h"
#include "game.h"




int main(void) {
	initialize_hardware();
	while (1){
		initialize_pong_game();
		play_pong();
	}
	return 0 ;
}
