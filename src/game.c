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

#include <stdlib.h>
#include "game.h"
#include "display.h"

// constantes reliées au jeux
#define LEFT_PLAYER (LEFT_POT)
#define RIGHT_PLAYER (RIGHT_POT)
#define NO_WINNER (3)
#define PADDLE_HEIGHT (3)
#define PADDLE_VMIN (7)
#define PADDLE_VMAX (VRES-PADDLE_HEIGHT-1)
#define TOP_BORDER (12)
#define BOTTOM_BORDER (TOP_BORDER+33)
#define LEFT_BORDER (0)
#define RIGHT_BORDER (HRES-1)
#define SHOW (1)
#define HIDE (0)
#define CENTER (((VRES-TOP_BORDER)>>1)+TOP_BORDER-1)
#define WIN_SCORE (21)

typedef struct ball_struct{
	int x;
	int y;
	int dx;
	int dy;
	int skip;
}ball_t;


unsigned char score[2]; // pointage joueurs
unsigned char paddle[2]; // position raquettes
ball_t ball; //position de la balle
unsigned char winner=LEFT_PLAYER;
unsigned char server;
unsigned char running;



// affiche ou cache la raquette
void draw_paddle(int player,int show){
	char mask;
	int i,offset;
	if (!player){
		mask=128;
		offset=0;
	}else{
		mask=1;
		offset=7;
	}//if
	for (i=0;i<PADDLE_HEIGHT;i++){
		if (show){
			video_buffer[(paddle[player]+i)*ROW_SIZE+offset]|=mask;
		}else{
			video_buffer[(paddle[player]+i)*ROW_SIZE+offset]&=~mask;
		}
	}//for
}//f()


// déplace la raquette du joueur
void move_paddle(int player){
	int new_y;
	new_y=read_pot(player)+TOP_BORDER;
	if (new_y!=paddle[player]){
		draw_paddle(player, HIDE);
		paddle[player]=new_y;
		draw_paddle(player,SHOW);
	}
}//f()


// fait entendre un son lorsqu'un joueur manque la balle
void sound_miss(){
	int f,s;
	f=1024;
	while ((s=f>>4)){
		tone(f,2);
		f -= s;
	}
}//f()

// fait entendre un son lorsque la balle rebondie
void sound_rebound(){
    tone(100,30);
}//f()

const char WINNER[32][4]={
	{0x80,0x1f,0x00,0x20},
	{0x80,0x20,0x80,0x20},
	{0x40,0x2a,0x80,0x40},
	{0x40,0x20,0x80,0x40},
	{0x60,0x31,0x80,0xc0},
	{0x20,0x1b,0x00,0x80},
	{0x30,0x0e,0x01,0x80},
	{0x18,0x0a,0x03,0x00},
	{0x0f,0xfb,0xfe,0x00},
	{0x00,0x40,0x40,0x00},
	{0x00,0x51,0x40,0x00},
	{0x00,0x55,0x40,0x00},
	{0x00,0x55,0x40,0x00},
	{0x00,0x55,0x40,0x00},
	{0x00,0x5b,0x40,0x00},
	{0x00,0x40,0x40,0x00},
	{0x00,0x40,0x40,0x00},
	{0x00,0x40,0x40,0x00},
	{0x00,0x60,0xc0,0x00},
	{0x00,0x3f,0x80,0x00},
	{0x00,0x3b,0x80,0x00},
	{0x00,0x31,0x80,0x00},
	{0x00,0x71,0xc0,0x00},
	{0x00,0x60,0xc0,0x00},
	{0x00,0x60,0xc0,0x00},
	{0x00,0x60,0xc0,0x00},
	{0x00,0x60,0xc0,0x00},
	{0x00,0xc0,0x60,0x00},
	{0x00,0xc0,0x60,0x00},
	{0x00,0xc0,0x60,0x00},
	{0x01,0x80,0x30,0x00},
	{0x03,0x80,0x38,0x00}
};

const int gonna_fly_now[]={392,250,392,125,392,125,392,250,392,250,392,125,
		                   392,125,494,250,392,125,392,125,392,250,494,250,
		                   494,250,494,125,494,125,494,250,494,125,494,125,
		                   587,250,494,125,494,125,494,500,0,0};

void game_over(){
	int i,j,offset;
	if (score[LEFT_PLAYER]==21){
		winner=LEFT_PLAYER;
		offset=0;
	}else{
		winner=RIGHT_PLAYER;
		offset=4;
	}
	for (i=0;i<32;i++){
		for (j=0;j<4;j++){
			video_buffer[(i+TOP_BORDER+1)*ROW_SIZE+j+offset]|=WINNER[i][j];
		}
	}
	play_tune(gonna_fly_now,60);
	running=0;
}//f()

// attend que le joueur déplace sa raquette
void wait_service(){
	int y;
	paddle[server]=read_pot(server)+TOP_BORDER;
	while (1){
		y=read_pot(server)+TOP_BORDER;
		if (y<paddle[server]-1 || y>paddle[server]+1) break;
		move_paddle(1-server);// l'autre joueur peut se déplacer en attendant le service.
	}
	ball.dy=rand()%3 - 1;
	if (server==LEFT_PLAYER){
		ball.dx=1;
	}else{
		ball.dx=-1;
	}
	ball.skip=rand()%2;
}//f()

// le joueur a manquer la balle
// mettre à jour le pointage
// faire entendre le son associé
// player est le joueur qui a manqué la balle.
void ball_missed(int player){
	plot(ball.x,ball.y,INVERT);
	sound_miss();
	plot(ball.x,ball.y,INVERT);
	ball.dx=0;
	ball.dy=0;
	if (player==LEFT_PLAYER){
		score[RIGHT_PLAYER]++;
		write_score(RIGHT_PLAYER,score[RIGHT_PLAYER]);
		if (score[RIGHT_PLAYER]==WIN_SCORE){
			game_over();
			return;
		}
		ball.x=RIGHT_BORDER-1;
		ball.y=paddle[RIGHT_PLAYER]+1;
		server=RIGHT_PLAYER;
	}else{
		score[LEFT_PLAYER]++;
		write_score(LEFT_PLAYER,score[LEFT_PLAYER]);
		if (score[LEFT_PLAYER]==WIN_SCORE){
			game_over();
			return;
		}
		ball.x=LEFT_BORDER+1;
		ball.y=paddle[LEFT_PLAYER]+1;
		server=LEFT_PLAYER;
	}
	plot(ball.x,ball.y,WHITE);
	wait_service();
}//f()

//déplace la balle et vérifie les collisions
void move_ball(){
    int new_x, new_y,bounce;
	if (!(ball.dx || ball.dy)) return;
	plot(ball.x,ball.y,INVERT);
    bounce=0;

	if (ball.skip & ball.x){
    	new_y=ball.y;
    }else{
    	new_y=ball.y+ball.dy;
    }
//  new_y=ball.y+ball.dy;
	new_x=ball.x+ball.dx;
    if ((new_x==LEFT_BORDER)||(new_x==RIGHT_BORDER)){
    	if (ball.dx<0){
    		ball.x=new_x;
    		ball.y=new_y;
    		ball_missed(LEFT_PLAYER);
    	}else{
    		ball.x=new_x;
    		ball.y=new_y;
    		ball_missed(RIGHT_PLAYER);
    	}
    	return;
    }
    if (ball.dy && ((new_y==(TOP_BORDER+1))||(new_y==(BOTTOM_BORDER-1)))){
    	bounce=1; // rebond sur mur latéral.
    }//if
    if ((ball.dx<0) && (new_x==LEFT_BORDER+1)){
    		if ((new_y>=paddle[LEFT_PLAYER])&&(new_y<=(paddle[LEFT_PLAYER]+2))){
    			bounce=1; // rebond sur raquette joueur de gauche.
    			ball.dx=-ball.dx;
    			ball.dy=rand()%3 - 1;
    			ball.skip=rand()%2;
    		}
    }else if (new_x==RIGHT_BORDER-1){
    		if ((new_y>=paddle[RIGHT_PLAYER])&&(new_y<=(paddle[RIGHT_PLAYER]+2))){
    			bounce=1; // rebond sur raquette joueur de droite.
    			ball.dx=-ball.dx;
    			ball.dy=rand()%3 - 1;
    			ball.skip=rand()%2;
    		}
   	}
    if (bounce){
    	if ((new_x==31) && ball.dy){
    		ball.skip=rand()%2;
    	}
		if ((new_y==BOTTOM_BORDER-1) && (ball.dy==1)){
			ball.dy=-1;
		}else if ((new_y==TOP_BORDER+1) && (ball.dy==-1)){
			ball.dy=1;
		}
    	sound_rebound();
    }//if
    ball.x=new_x;
    ball.y=new_y;
    plot(new_x,new_y,INVERT);
}//f()

void initialize_pong_game(){
	unsigned i;
	clear_screen();
	for (i=0;i<ROW_SIZE;i++){
		 video_buffer[i+TOP_BORDER*ROW_SIZE]=0xff;
		 video_buffer[i+ROW_SIZE*BOTTOM_BORDER]=0xff;
	}
	for (i=TOP_BORDER+1;i<(BOTTOM_BORDER-1);i++){
		 if (i&1) video_buffer[ROW_SIZE*i+3]=1u;
	}
	running=0;
	score[LEFT_PLAYER]=0;
	score[RIGHT_PLAYER]=0;
	server=winner;
	winner=NO_WINNER;
	paddle[LEFT_PLAYER]=read_pot(LEFT_PLAYER)+TOP_BORDER;
	paddle[RIGHT_PLAYER]=read_pot(RIGHT_PLAYER)+TOP_BORDER;
	if (server==LEFT_PLAYER){
		ball.x=LEFT_BORDER+1;
		ball.y=paddle[LEFT_PLAYER]+1;
	}else{
		ball.x=RIGHT_BORDER-1;
		ball.y=paddle[RIGHT_PLAYER]+1;
	}
	ball.dx=0;
	ball.dy=0;
	write_score(LEFT_PLAYER,score[LEFT_PLAYER]);
	write_score(RIGHT_PLAYER,score[RIGHT_PLAYER]);
	draw_paddle(LEFT_PLAYER,WHITE);
	draw_paddle(RIGHT_PLAYER,WHITE);
	plot(ball.x,ball.y,WHITE);
	srand(frame_cntr);
}//f()

// au départ c'est le joueur de gauche qui a le service
// il doit déplacer sa raquette pour lancer la balle.
// la balle part avec un angle pointant vers le haut
// ou le bas en fonction de la direction du
// déplacement de la raquette.
void play_pong(){
	wait_service();
	running=1;
	while (running){
		move_paddle(LEFT_PLAYER);
		move_paddle(RIGHT_PLAYER);
		move_ball();
		frame_sync();
		frame_delay(1);
	}
}//f()




