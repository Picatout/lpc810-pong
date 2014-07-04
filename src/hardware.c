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
 * hardware.c
 *
 *  Created on: 2014-06-18
 *      Author: Jacques
 */


#include "hardware.h"
#include "tvout.h"

inline void assign_pins(){ // selectionne les périphériques sur les broches
	// on désactive tous les périphériques et fonctions inutilisés pour avoir accès aux GPIO
	 m_disable_pin(DIS_SWCLK|DIS_SWDIO|DIS_CLKIN);
	 LPC_SWM->PINENABLE0 &= ~(DIS_ACMP_I1|DIS_ACMP_I2); // activation des entrées comparateur analogique
	// configuration switch matrix
	LPC_SYSCON->SYSAHBCLKCTRL |= CLK_SWM; // activation signal clock sur SWM
	LPC_SWM->PINASSIGN6 = 0x00ffffff|(SYNC_OUT<<24); // SYNC_OUT sur PIO0_4  (CTOUT_0)
	LPC_SWM->PINASSIGN4 = 0xffffff00|VIDEO_OUT; // VIDEO_OUT sur PIO0_3  (SPI0 MOSI)
	LPC_SWM->PINASSIGN7 = 0xffffff00|AUDIO_OUT; // AUDIO_OUT sur PIO0_2  (CTOUT_1)
	LPC_SYSCON->SYSAHBCLKCTRL &= ~CLK_SWM; 	// désactivation signal clock sur SWM
}//f()

void SPI0_init(){
	LPC_SPI0->DIV=PIXCLK_DIV; // pixel clock
	LPC_SPI0->CFG |= SPI_MASTER;
	LPC_SPI0->TXCTRL = ((7u<<SPI_DATLEN)|(1u<<SPI_IGNRX)|(1u<<SPI_TXSSEL_N));
}//f()

void PWM_init(){
	LPC_SCT->CONFIG=3u<<17; //2 compteurs 16 bits, clocké par BUS_CLOCK et autolimit
	// initialisation SCT pour synchronisation vidéo , utilise CTR_L
	LPC_SCT->STATE_L=0; // état 0, les états ne sont pas utilisés
	LPC_SCT->REGMODE_L=0; // mode match.
	LPC_SCT->MATCH[0].L=LINE_PERIOD; // période ligne vidéo
	LPC_SCT->MATCHREL[0].L=LINE_PERIOD; // valeur de rechargement de registre MATCH[0].L
	LPC_SCT->MATCH[1].L=H_PULSE; // largeur impulsion de synchronisation horizontale
	LPC_SCT->MATCHREL[1].L=H_PULSE; // valeur de rechargement
	LPC_SCT->MATCH[2].L=VIDEO_DELAY;
	LPC_SCT->MATCHREL[2].L=VIDEO_DELAY;
	// événement zéro met fin à l'impulsion de synchronisation horizontale
	LPC_SCT->EVENT[0].CTRL=1u|(1u<<5)|(1u<<12); //contrôle CTOUT_0, condition CTR_L==MATCH[1].L
	LPC_SCT->EVENT[0].STATE=1; // événement actif lorsque STATE_L==0
	LPC_SCT->OUT[0].SET=(1u<<0); // contrôlé par événement 0
	// événement 1 débute l'impulsion de synchronisation horizontale
	LPC_SCT->EVENT[1].CTRL=(1u<<5)|(1u<<12); // contrôle CTOUT_0, condition CTR_L==MATCH[0].L
	LPC_SCT->EVENT[1].STATE=1; // événement actif lorsque STATE_L==0
	LPC_SCT->OUT[0].CLR=(1u<<1); // contrôlé par événement 1
    // événement 2 débute l'envoie des bits vidéo
	LPC_SCT->EVENT[2].CTRL=2|(1u<<12);// CTR_L=MATCH[2]
	LPC_SCT->EVENT[2].STATE=1;

	// inialistaion SCT pour sortie audio, compteur H
	LPC_SCT->CTRL_H=(1u<<2); //HALT
	LPC_SCT->CTRL_H &=~(255<<5); // diviseur
	LPC_SCT->CTRL_H|=(29<<5); // sys_clock/30
	LPC_SCT->STATE_H=0; // état 0, les états ne sont pas utilisés
	LPC_SCT->REGMODE_H=0; // mode match
	LPC_SCT->EVENT[3].CTRL=(1u<<4)|(1u<<5)|(1u<<6)|(1u<<12); // contrôle CTOUT_1, condition CTR_H==MATCH[0].H
	LPC_SCT->EVENT[3].STATE=1; // événement actif lorsque STATE_H=0
	LPC_SCT->EVENT[4].CTRL=1u|(1u<<4)|(1u<<5)|(1u<<6)|(1u<<12); // contrôle CTOUT_1, condition CTR_H==MATCH[1].H
	LPC_SCT->EVENT[4].STATE=1;  // événement actif lorsque STATE_H=0
	LPC_SCT->OUT[1].CLR=(1u<<3);  // événement 3 met COUT_1 à 0
	LPC_SCT->OUT[1].SET=(1u<<4);  // événement 4 met CTOUT_1 à 1

	LPC_SCT->EVFLAG=0x3f; // met à zéro tous les indicateurs d'évémenements
	LPC_SCT->EVEN =(1u<<SCT_IRQ_EVENT_1); // interruption authorisé sur événement 1
	NVIC_EnableIRQ(SCT_IRQn); // active l'interruption SCT (9)

	LPC_SCT->CTRL_L = (LPC_SCT->CTRL_L & ~(3u << 1))|(1u<<3); // remet compteur à zéro et démarre le PWM

}//f()

void sysclock_init(){
	LPC_SYSCON->PDRUNCFG &=~((1u<<7u)|(1u<<15u)); // activation du PLL et ACMP
	LPC_SYSCON->SYSPLLCLKSEL=0; // sélection source du PLL, alimenté par IRC (12Mhz)
	LPC_SYSCON->SYSPLLCLKUEN=0; // mise à jour
	LPC_SYSCON->SYSPLLCLKUEN=1; // source PLL
	LPC_SYSCON->SYSPLLCTRL=4|(1<<5); // feedback div =5, post-diviseur=4 pour FCLKOUT=60Mhz
	while (!(LPC_SYSCON->SYSPLLSTAT&1));// attend que le PLL sois stable
	LPC_SYSCON->SYSAHBCLKDIV=2; // FSYSCLK=FCLKOUT/2
	LPC_SYSCON->MAINCLKSEL=3;  // sélection main clock= sortie PLL
	LPC_SYSCON->MAINCLKUEN=0;  // mise à jour
	LPC_SYSCON->MAINCLKUEN=1;  // source main clock
	m_periph_clock_enable(CLK_GPIO|CLK_SCT|CLK_MRT|CLK_SPI0|CLK_ACMP); // active signaux clock pour les périphérique utilisés
	m_periph_reset(RESET_MRT|RESET_GPIO|RESET_SCT|RESET_SPI0|RESET_ACMP); // réinitialise les périphériques
	LPC_CMP->CTRL=(3u<<25); //hystérésie 20mV.
	LPC_CMP->LAD=1;
}//f()


void initialize_hardware(){
	 sysclock_init();
	 LPC_MRT->Channel[0].CTRL=(1u<<1);  //multi-rate timer canal 0 en mode 'one-shot interrupt'
	 assign_pins(); // assignation des périphériques aux broches
	 //LPC_SYSCON->IRQLATENCY=40;
	 NVIC_SetPriority(SCT_IRQn,0); // plus haute priorité d'interruption
	 NVIC_SetPriority(MRT_IRQn,3); // plus basse priorité
	 PWM_init();
     SPI0_init();
}//f()

void delay_ms(uint32_t n){
	LPC_MRT->Channel[0].STAT|=1;
	LPC_MRT->Channel[0].INTVAL=(n*MSEC)&0x7fffffff;
	while (!(LPC_MRT->Channel[0].STAT&1u));
}//f()

inline void delay_us(uint32_t n){
	LPC_MRT->Channel[0].STAT|=1;
	LPC_MRT->Channel[0].INTVAL=(n*USEC)&0x7fffffff;
	while (!(LPC_MRT->Channel[0].STAT&1u));
}//f()


volatile unsigned tone_on=0;

#define AUDIO_CLOCK (SYS_CLOCK/30)

void tone(uint32_t freq, uint32_t msec){
	while (tone_on); // attend la fin de la note précédente
	LPC_MRT->Channel[1].STAT|=1;
	NVIC_EnableIRQ(MRT_IRQn);
	LPC_MRT->Channel[1].CTRL=3u; //single shot interrupt mode
	LPC_MRT->Channel[1].INTVAL=(msec*MSEC)&0x7fffffff;
	LPC_SCT->MATCH[0].H=((AUDIO_CLOCK/freq))-1;   // période
	LPC_SCT->MATCHREL[0].H=((AUDIO_CLOCK/freq))-1;
	LPC_SCT->MATCH[1].H=((AUDIO_CLOCK/freq)>>1)-1;   // rapport cyclique 50%
	LPC_SCT->MATCHREL[1].H=((AUDIO_CLOCK/freq)>>1)-1;
	LPC_SCT->CTRL_H = (LPC_SCT->CTRL_H & ~(3u << 1))|(1u<<3); //démarre le PWM
	tone_on=1;
}//f()

void play_tune(const int *tune, int pause){
	int freq, duration;
	freq=*tune++;
	duration=*tune++;
	while (duration){
		if (freq){
			tone(freq,duration);
			while (tone_on);
			delay_ms(pause);
		}else{
			delay_ms(duration+pause);
		}
		freq=*tune++;
		duration=*tune++;
	}
}//f()




void MRT_IRQHandler(void){
	if (LPC_MRT->Channel[1].STAT&1){
		tone_on=0;
		LPC_SCT->CTRL_H=(1u<<2);// arrêt du PWM
		LPC_MRT->Channel[1].STAT|=1;
		NVIC_DisableIRQ(MRT_IRQn);
	}
}//f()


/*
 *  utilisation du comparateur analogique avec la référence de
 *  voltage à 32 niveaux pour créer un convertisseur A/N à 5 bits
 *  la lecture se fait par approximation successive en divisant
 *  l'intervalle en 2 à chaque test.
 */
int read_pot(int pot){
	int min,max,test,count,last,current;
	min=0;
	max=31;
	test=15;
	LPC_CMP->CTRL &= ~(3u<<8);
	if (pot==LEFT_POT){
		LPC_CMP->CTRL|=(2u<<8); // entrée positive du comparateur sur ACMP0_I2
	}else{
		LPC_CMP->CTRL|=(1u<<8); // entrée positive du comparateur sur ACMP0_I1
	}
	while ((test<max) && (test>min)){
		LPC_CMP->LAD&=~(1|(31<<1u));
		LPC_CMP->LAD|=1|(test<<1u);
		delay_us(10);
		count=0;
		last=1;
		while (count<5){
			current=LPC_CMP->CTRL&(1u<<21u);
			if (current==last){
				count++;
			}else{
				last=current;
				count=0;
			}
			delay_us(1);
		}
		if (last){
			min=test;
			test=test+((max-test)>>1);
		}else{
			max=test;
			test=test-((test-min)>>1);
		}
	}
	return	test;
}//f()
