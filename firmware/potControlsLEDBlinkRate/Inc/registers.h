#ifndef REGISTER_HEADER
#define REGISTER_HEADER
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>



typedef struct {
	uint8_t id;
	float val;
	uint8_t hold;
	
} stage;

typedef struct {
	uint8_t id;
	uint8_t loc;
	float val;
	uint8_t hold;
	float gateThresh;
	uint8_t clockDivider;
	float holdProbability;	
} tap;


typedef struct {
	tap * taps;
	stage * stages;
	uint8_t numOfStages;
	uint8_t numOfTaps;
	uint8_t span;
	uint8_t spanMode;
	uint8_t holdMode;
	uint8_t feedInID;
	uint8_t feedOutID;
	float feedMix;
	uint16_t clockCount;
	
} shiftRegister;


void initRegister(shiftRegister * newRegister, int _numOfTaps, int _numOfStages);

void sampleValue(shiftRegister * registerToUpdate, float newVal);

void updateTapVals(shiftRegister * registerToUpdate);

void updateTapLocs(shiftRegister * registerToUpdate);

void updateTapHolds(shiftRegister * registerToUpdate);

void updateClockDividerHolds(shiftRegister * registerToUpdate);

void updateStochasticHolds(shiftRegister * registerToUpdate);

int rand_lim(int limit);


#endif
