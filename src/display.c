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
 * display.c
 *
 *  Created on: 2014-06-24
 *      Author: Jacques Deschênes
 *      Description: interfaces function to tvout display
 */

#include "display.h"

// police 3x5 pixels, chiffres 0-9
const char  digits[10][5]={
		{14,10,10,10,14},//0
		{4,12,4,4,14},//1
		{14,2,14,8,14},//2
		{14,2,14,2,14},//3
		{10,10,14,2,2},//4
		{14,8,14,2,14},//5
		{8,8,14,10,14},//6
		{14,2,2,2,2},//7
		{14,10,14,10,14},//8
		{14,10,14,2,2} //9
};


// affiche le pointage
void write_score(int player, int score){
	unsigned char offset;
	int i,j;
	offset=player*(ROW_SIZE>>1)+1;
	i=(score/10);
//	put_char_at(offset,0,i,WHITE);

	for (j=0;j<5;j++){
		video_buffer[j*ROW_SIZE+offset]=digits[i][j]<<4;
	}

	i=(score%10);
//	put_char_at(offset+4,0,i,WHITE);

	for (j=0;j<5;j++){
		video_buffer[j*ROW_SIZE+offset]|=digits[i][j];
	}

}//f()

/*
 * void plot(int x, int y, int operation
 * description: dessine un pixel
 * paramètres:
 * 		x: coordonnée X
 * 		y: coordonnée Y
 * 		color: BLACK, WHITE et INVERT
 */
void plot(int x, int y, int color){
	int byte, bit;
	byte = x/8;
	bit= 1<<(7-(x%8));
	switch(color){
	case BLACK:
		video_buffer[y*ROW_SIZE+byte]&=~bit;
		break;
	case WHITE:
		video_buffer[y*ROW_SIZE+byte]|=bit;
		break;
	case INVERT:
		video_buffer[y*ROW_SIZE+byte]^=bit;
		break;
	}//switch
}//f()




void clear_screen(){
	int i;
	for (i=BUFFER_SIZE;i;i--) video_buffer[i]=0;
}//f()
