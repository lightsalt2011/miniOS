#include "s3c2440.h"

int init_buzzer(void){
	SFR_VA(GPBCON) = SFR_VA(GPBCON) & (~0x3) | 0x1;
	return 0;
}

int buzzer_on(void){
	SFR_VA(GPBDAT) = SFR_VA(GPBDAT) & (~0x1) | 0x1;
	return 0;
}

int buzzer_off(void){
	SFR_VA(GPBDAT) = SFR_VA(GPBDAT) & (~0x1);
	return 0;
}

int beep(long time){
 	buzzer_on();
	wait(time);
	buzzer_off();
	return 0;
}	