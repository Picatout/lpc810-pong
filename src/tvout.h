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
 * tvout.h
 *
 *  Created on: 2014-06-18
 *      Author: Jacques Deschênes
 */

#ifndef TVOUT_H_
#define TVOUT_H_
#include "hardware.h"

// paramètres vidéo NTSC
#define HRES (64) // pixels horizontal
#define VRES (56) // pixels vertical
#define ROW_SIZE (HRES/(8*sizeof(char)))  //nombre d'entiers par ligne d'affichage
#define LINES_PER_FRAME  (262) // nombre de lignes vidéo par balayage vertical
#define NTSC_DISPLAY (230) // nombre de lignes visibles dans un balayage vertical
#define RESCAN (4) //((unsigned) NTSC_DISPLAY/VRES)  //nombre de lignes d'affichage par pixels
#define DISPLAY_LINES  (VRES*RESCAN) // nombre de lignes vidéo affichées
#define FIRST_VISIBLE  (30) //((NTSC_DISPLAY-DISPLAY_LINES)/2+22)
#define LAST_VISIBLE (FIRST_VISIBLE+DISPLAY_LINES-1)
#define BUFFER_SIZE (ROW_SIZE*sizeof(char)*VRES) // video buffer size
#define LINE_PERIOD  (1893) // ~Fsysclk/15734, ajuster pour obtenir 63,5µSec
#define H_PULSE  (139)  // largeur impulsion HSYNC   4,7µsec*Fsysclk, ajuster si nécessaire
#define V_PULSE  (LINE_PERIOD-H_PULSE) // largeur impulsion VSYNC
#define VIDEO_DELAY (2*H_PULSE-100) // délais en début de ligne avant envoie pixels
#define PIXCLK_DIV  (18)  // diviseur pixel clock
#define   BLACK (0)
#define   WHITE (1)
#define   INVERT (2)

extern volatile unsigned frame_cntr;

extern volatile unsigned char video_buffer[BUFFER_SIZE];

void frame_delay(int n); // délais en multiple de frame
void frame_sync(); // attend le prochain frame


#endif /* TVOUT_H_ */
