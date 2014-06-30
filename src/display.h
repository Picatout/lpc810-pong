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
 * display.h
 *
 *  Created on: 2014-06-24
 *      Author: Jacques Deschênes
 *      Description: interfaces function to tvout display
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "tvout.h"

void write_score(int player, int score); // affiche le pointage du joueur.
void plot(int x, int y, int color); // dessine un point.
void clear_screen(); // efface l'écran


#endif /* DISPLAY_H_ */
