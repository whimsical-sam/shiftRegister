#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "registers.h"


void delay(int number_of_seconds) 
{ 
    // Converting time into milli_seconds 
    int milli_seconds = 1000 * number_of_seconds; 
  
    // Stroing start time 
    clock_t start_time = clock(); 
  
    // looping till required time is not acheived 
    while (clock() < start_time + milli_seconds) 
        ; 
} 
  


void run() {
	shiftRegister mainRegister;
	initRegister(&mainRegister,6,64);
	
	for (int i = 0; i<mainRegister.numOfStages; i++) {
		
		printf("Clock Count: %d\n",mainRegister.clockCount);
		
		sampleValue(&mainRegister,i);
		updateTapHolds(&mainRegister);
		updateTapLocs(&mainRegister);
		updateTapVals(&mainRegister);
		
		for (int i =0; i<mainRegister.numOfTaps; i++) {
			printf("Tap %d: %.1f\n",mainRegister.taps[i].id,mainRegister.taps[i].val);
		}
		printf("\n");
		delay(1);
		
	}	
}






void main() {
	run();
}