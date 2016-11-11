#include "predictor.h"
#include <stdio.h>
#include <math.h>

const char *predictorName[5] = { "Static", "Gshare", "Local",
"Alpha21264", "Perceptron" };

int globalhistBits;	// Number of bits for Global History
int localhistBits;	// Number of bits for Local History
int pcBits;			// Number of bits for PC index
int predictorType;  // Branch Predictor Type


//intialize predictors
void init_predictor_gshare();
void init_predictor_local();
void init_predictor_alpha21264();
void init_predictor_perceptron();


//training
void train_predictor_gshare(unsigned int pc, bool outcome);
void train_predictor_local(unsigned int pc, bool outcome);
void train_predictor_alpha21264(unsigned int pc, bool outcome);
void train_predictor_perceptron(unsigned int pc, bool outcome);


//make predictions
bool make_prediction_gshare(unsigned int pc);
bool make_prediction_local(unsigned int pc);
bool make_prediction_alpha21264(unsigned int pc);
bool make_prediction_perceptron(unsigned int pc);
int MAX(int a, int b);
int MIN(int a, int b);

// Predictor Data Structures
int GHR = 0;

int* BHT;
int* PHT;

int bht_mask = 0;
int pht_mask = 0;
int bht_length = 0;
int pht_length = 0;

void init_predictor ()
{
	switch (predictorType) {
		case DEFAULT:
			break;
		case GSHARE:
			init_predictor_gshare();
			break;
		case LOCAL:
			init_predictor_local();
			break;
		case ALPHA21264:
			init_predictor_alpha21264();
			break;
		case PERCEPTRON:
			init_predictor_perceptron();
			break;
		default:
			break;
		}
}

void init_predictor_local() {
	pht_length = 1 << pcBits;
	pht_mask = pht_length - 1;
	PHT = (int*)malloc(pht_length * sizeof(int));
	for (int i = 0; i< pht_length; i++) {
		PHT[i] = 0;
	}

	bht_length = 1 << localhistBits;
	bht_mask = bht_length - 1;
	BHT = (int*)malloc(bht_length * sizeof(int));
	for (int i = 0; i< bht_length; i++) {
		BHT[i] = 1;
	}
}

void init_predictor_alpha21264(){}
void init_predictor_perceptron(){}
void init_predictor_gshare(){}

bool make_prediction (unsigned int pc)
{
	switch (predictorType) {
		case DEFAULT:
			return TAKEN;
		case GSHARE:
			return make_prediction_gshare(pc);
		case LOCAL:
			return make_prediction_local(pc);
		case ALPHA21264:
			return make_prediction_alpha21264(pc);
		case PERCEPTRON:
			return make_prediction_perceptron(pc);
		default:
			break;
		}

	return NOTTAKEN;
}

bool make_prediction_local(unsigned int pc) {
	int iPHT = pc & pht_mask;
	int iBHT = PHT[iPHT] & bht_mask;

	if (BHT[iBHT] > 1) {
		return TAKEN;
	}
	return NOTTAKEN;
}

bool make_prediction_gshare(unsigned int pc){}
bool make_prediction_alpha21264(unsigned int pc){}
bool make_prediction_perceptron(unsigned int pc){}

void train_predictor (unsigned int pc, bool outcome)
{
	switch (predictorType) {
		case DEFAULT:
			return;
		case GSHARE:
			train_predictor_gshare(pc, outcome);
			return;
		case LOCAL:
			train_predictor_local(pc, outcome);
			return;
		case ALPHA21264:
			train_predictor_alpha21264(pc, outcome);
			return;
		case PERCEPTRON:
			train_predictor_perceptron(pc, outcome);
			return;
		default:
			return;
		}
}

void train_predictor_local(unsigned int pc, bool outcome) {
	int iPHT = pc & pht_mask;
	int iBHT = PHT[iPHT] & bht_mask;

	if (outcome == TAKEN) {
		BHT[iBHT]++;
	}
	else {
		BHT[iBHT]--;
	}

	if (BHT[iBHT] > 3) {
		BHT[iBHT] = 3;
	}
	else if (BHT[iBHT] < 0) {
		BHT[iBHT] = 0;
	}

	PHT[iPHT] <<= 1;
	PHT[iPHT] &= bht_mask;
	if (outcome == TAKEN) {
		PHT[iPHT] += 1;
	}
}

void train_predictor_gshare(unsigned int pc, bool outcome){}
void train_predictor_alpha21264(unsigned int pc, bool outcome){}
void train_predictor_perceptron(unsigned int pc, bool outcome){}


int MAX(int x, int y)
{
	if (x > y)
		return x;
	return y;
}

int MIN(int x, int y)
{
	if (x < y)
		return x;
	return y;
}
