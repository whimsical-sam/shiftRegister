#include "registers.h"
#define TRUE 1
#define FALSE 0

void initRegister(shiftRegister * newRegister, int _numOfTaps, int _numOfStages) {
	
	// Initialize register default settings.
	newRegister->clockCount = 0;
	newRegister->span = 1;
	newRegister->spanMode = 0; // mode 0 makes each tap located at id*span, while mode 1 makes each tap located at id+span -- change to enum??
	newRegister->holdMode = 0; // mode 0 uses clock dividers to determine holds, while mode 1 uses hold probabilities
	newRegister->feedInID = 0;
	newRegister->feedOutID = _numOfTaps-1;
	newRegister->feedMix = 0;
	newRegister->numOfStages = _numOfStages;
	newRegister->numOfTaps = _numOfTaps;
	
	// Assign memory for tap and stage arrays
	newRegister->taps = (tap *) malloc(_numOfTaps * sizeof(tap));
	newRegister->stages = (stage *) malloc(_numOfStages * sizeof(stage));
	
	
	// Initialize stages of the register
	for (int i = 0; i<_numOfStages; i++) {
		stage * newStage = &(newRegister->stages[i]);
		newStage->id = i;
		newStage->val = 0;
		newStage->hold = FALSE;
	}
	
	//Initialize taps of the register
	for (int i = 0; i<_numOfTaps; i++) {
		tap * newTap = &(newRegister->taps[i]);
		newTap->id = i;
		newTap->loc = (newRegister->span)*i;
		newTap->val = 0;
		newTap->hold = FALSE;
		newTap->gateThresh = 0.5;
		newTap->clockDivider = 1;
		newTap->holdProbability = 0;
	}
}

void sampleValue(shiftRegister * registerToUpdate, float newVal) {
	
	
	// get the feedback stages current value before it gets overwritten.
	float feedbackVal = registerToUpdate->stages[registerToUpdate->feedOutID].val;
	
	// Get the last stage
	stage * currentStage = &(registerToUpdate->stages[registerToUpdate->numOfStages-1]);
	
	// Starting with the last stage as the current stage, transfer the previous stage's value into the current Stage
	while (currentStage->id > 0) {
		stage * previousStage = &(registerToUpdate->stages[currentStage->id-1]);
		currentStage->val = previousStage->val;
		currentStage = previousStage;
	}
	
	// Sample the new value into the head of the chain
	currentStage->val = newVal;
	
	// Process feedback
	stage * feedbackStage = &(registerToUpdate->stages[registerToUpdate->feedInID]);
	feedbackStage->val = feedbackStage->val*(1-registerToUpdate->feedMix)+(registerToUpdate->feedMix)*feedbackVal;
	
	
	
}

void updateTapVals(shiftRegister * registerToUpdate) {
	
	for (int i = 0; i<registerToUpdate->numOfTaps; i++) {
		
		// get Tap
		tap * tapToUpdate = &(registerToUpdate->taps[i]);
		
		// Check if tap is held
		if (tapToUpdate->hold != TRUE) {
			
			// if not held, then get value from correct location and update tap
			float valToUse = registerToUpdate->stages[tapToUpdate->loc].val;
			tapToUpdate->val = valToUse;
		}
		
	}
	
	//Increment Clock
	registerToUpdate->clockCount++;
}

void updateTapLocs(shiftRegister * registerToUpdate) {
	uint8_t span = registerToUpdate->span;
	uint8_t mode = registerToUpdate->spanMode;
	uint8_t numOfStages = registerToUpdate->numOfStages;
	for (int i = 0; i<registerToUpdate->numOfTaps; i++) {
		tap * tapToUpdate = &(registerToUpdate->taps[i]);
		switch (mode) {
			case 0:
				tapToUpdate->loc = (tapToUpdate->id*span)%numOfStages;
				break;
			
			case 1:
				tapToUpdate->loc = (tapToUpdate->id+(span-1))%numOfStages;
				break;
		}	
	}
}

void updateTapHolds(shiftRegister * registerToUpdate) {
	switch (registerToUpdate->holdMode) {
		case 0:
			updateClockDividerHolds(registerToUpdate);
			break;
			
		case 1:
			updateStochasticHolds(registerToUpdate);
			break;
	}
}

void updateClockDividerHolds(shiftRegister * registerToUpdate) {
	
	uint16_t clockCount = registerToUpdate->clockCount;
	
	for (int i = 0; i<registerToUpdate->numOfTaps; i++) {
		tap * tapToUpdate = &(registerToUpdate->taps[i]);
		if (clockCount % (tapToUpdate->clockDivider) == 0) {
			tapToUpdate->hold = FALSE;
		} else {
			tapToUpdate->hold = TRUE;
		}
	}
}

void updateStochasticHolds(shiftRegister * registerToUpdate) {
	for (int i = 0; i<registerToUpdate->numOfTaps; i++) {
		tap * tapToUpdate = &(registerToUpdate->taps[i]);
		if (rand_lim(99)<tapToUpdate->holdProbability) {
			tapToUpdate->hold=TRUE;
		} else {
			tapToUpdate->hold = FALSE;
		}
	}
}

int rand_lim(int limit) {
/* return a random number between 0 and limit inclusive.
 */

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}