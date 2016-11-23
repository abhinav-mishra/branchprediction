#include "predictor.h"
#include <stdio.h>
#include <math.h>

int globalhistBits;	// Number of bits for Global History
int localhistBits;	// Number of bits for Local History
int pcBits;			// Number of bits for PC index
int predictorType;  // Branch Predictor Type
int budgetType;     // Budget Limit in K

//intialize predictors
extern void initializeBits_local();
extern void initializeBits_gshare();
extern void initializeBits_alpha21264();
extern void initializeBits_perceptron();
extern int init_predictor_gshare();
extern int init_predictor_local();
extern int init_predictor_alpha21264();
extern int init_predictor_perceptron();


//training
extern void train_predictor_gshare(unsigned int pc, bool outcome);
extern void train_predictor_local(unsigned int pc, bool outcome);
extern void train_predictor_alpha21264(unsigned int pc, bool outcome);
extern void train_predictor_perceptron(unsigned int pc, bool outcome);

//make predictions
extern bool make_prediction_gshare(unsigned int pc);
extern bool make_prediction_local(unsigned int pc);
extern bool make_prediction_alpha21264(unsigned int pc);
extern bool make_prediction_perceptron(unsigned int pc);

// Predictor Data Structures
int GHR = 0;

int* BHT;   // Branch History Table
int* PHT;   // Pattern History Table
int* gPred; // Global predictor
int* choice; // For making choice between global and local in Alpha
int** percep_weights; //For perceptron
int* history; //For perceptron history bits (-1 or 1)
float y_perceptron = 0.0;
int percep_length = 0;
int percep_mask = 0;
float theta = 0.0;

int bht_mask = 0;
int pht_mask = 0;
int bht_length = 0;
int pht_length = 0;
int gPred_length = 0;
int choice_len = 0;

// Initializing the right predictor
int init_predictor ()
{
	switch (predictorType) {
		case DEFAULT:
			break;
		case LOCAL:
            initializeBits_local();
			return init_predictor_local();
			break;
		case GSHARE:
            initializeBits_gshare();
			return init_predictor_gshare();
			break;
		case ALPHA21264:
            initializeBits_alpha21264();
			return init_predictor_alpha21264();
			break;
		case PERCEPTRON:
            initializeBits_perceptron();
			return init_predictor_perceptron();
			break;
		default:
			return -1;
			break;
		}
	return -1;
}

// Choosing the predictor for making predictions
bool make_prediction (unsigned int pc)
{
	switch (predictorType) {
		case DEFAULT:
			return TAKEN;
		case LOCAL:
			return make_prediction_local(pc);
		case GSHARE:
			return make_prediction_gshare(pc);
		case ALPHA21264:
			return make_prediction_alpha21264(pc);
		case PERCEPTRON:
			return make_prediction_perceptron(pc);
		default:
			break;
		}

	return NOTTAKEN;
}

// Training the right predictor 
void train_predictor (unsigned int pc, bool outcome)
{
	switch (predictorType) {
		case DEFAULT:
			return;
		case LOCAL:
			train_predictor_local(pc, outcome);
			return;
		case GSHARE:
			train_predictor_gshare(pc, outcome);
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

