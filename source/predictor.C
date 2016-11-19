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

void perceptron(int);

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
int* gPred;
int* choice;
int** percep_weights; //For perceptron
int* history; //For perceptron
float y_perceptron = 0.0;
int percep_length = 0;
int percep_mask = 0;
int theta = 0;

int bht_mask = 0;
int pht_mask = 0;
int bht_length = 0;
int pht_length = 0;
int gPred_length = 0;
int choice_len = 0;

void init_predictor ()
{
	switch (predictorType) {
		case DEFAULT:
			break;
		case LOCAL:
			init_predictor_local();
			break;
		case GSHARE:
			init_predictor_gshare();
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

void init_predictor_local() 
{
    int sizeinbits = 0;
    pht_length = 1 << pcBits;
	pht_mask = pht_length - 1;
	int localPHT[pht_length];
    //PHT = &localPHT[0];//(int*)malloc(pht_length * sizeof(int));
	for (int i = 0; i< pht_length; i++) {
		localPHT[i] = 0;
	}
    PHT = &localPHT[0];
	bht_length = 1 << localhistBits;
	bht_mask = bht_length - 1;
	BHT = (int*)malloc(bht_length * sizeof(int));
	for (int i = 0; i< bht_length; i++) {
		BHT[i] = 0;
	}
    sizeinbits = (pht_length*localhistBits)+(bht_length * 2);
    printf("sizeinbits = %d+%d = %d \n", (pht_length * localhistBits), (bht_length * 2), sizeinbits);
}

void init_predictor_gshare()
{
	int sizeinbits = 0;
    GHR = 0;
    bht_length = 1 << globalhistBits;
	bht_mask = (1 << globalhistBits) - 1;
	BHT = (int*)malloc(bht_length * sizeof(int));
	for (int i = 0; i< bht_length; i++) {
		BHT[i] = 0;
	}
    sizeinbits = (globalhistBits)+(bht_length * 2);
    printf("sizeinbits = %d+%d = %d \n", (globalhistBits), (bht_length * 2), sizeinbits);
}

void init_predictor_alpha21264()
{
	int sizeinbits = 0;
    GHR = 0;
	gPred_length = 1 << globalhistBits;
	gPred = (int*)malloc(gPred_length * sizeof(int));
	for (int i = 0; i< gPred_length; i++) {
		gPred[i] = 1;
	}

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
		BHT[i] = 0;
	}

	choice_len = 1 << globalhistBits;
	choice = (int*)malloc(choice_len * sizeof(int));
	for (int i = 0; i< choice_len; i++) {
		choice[i] = 0;
	}
    
    sizeinbits = (pht_length*localhistBits)+(bht_length * 2)+(choice_len * 2 * 2)+(globalhistBits);
    printf("sizeinbits = %d+%d = %d \n", (pht_length * localhistBits)+(bht_length * 2), (choice_len * 2 * 2), sizeinbits);

}

void init_predictor_perceptron()
{
    int sizeinbits = 0;
    percep_length = 1 << pcBits;
    theta = (1.93 * (globalhistBits)) + 14;
    percep_mask = percep_length - 1;
    percep_weights = (int **)malloc(percep_length * sizeof(int*));
    for (int i = 0; i< percep_length; i++) {
        percep_weights[i] = (int*)malloc((globalhistBits+1) * sizeof(int));
        for (int j = 0; j <= globalhistBits; j++) {
            percep_weights[i][j] = 0;
        }
    }

    history = (int*)malloc(globalhistBits * sizeof(int));
    for (int i = 0; i< globalhistBits; i++) {
        history[i] = -1;
    }

    sizeinbits = ((globalhistBits+1) * percep_length * BW) + globalhistBits;
    printf("sizeinbits = %d\n", sizeinbits);
}

int arrToint() {
    int res = 0;
    for (int i=0; i<globalhistBits; i++) {
        if (history[i] == 1)
            res += (1<<i);
    }
    return res;
}

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

bool make_prediction_local(unsigned int pc) {
	int iPHT = pc & pht_mask;
	int iBHT = PHT[iPHT] & bht_mask;

	if (BHT[iBHT] > 1) {
		return TAKEN;
	}
	return NOTTAKEN;
}

bool make_prediction_gshare(unsigned int pc)
{
	int xr = (GHR^pc) & bht_mask;
	if (BHT[xr] > 1) {
		return TAKEN;
	}
	return NOTTAKEN;
}

bool make_prediction_alpha21264(unsigned int pc)
{
	int lResult = NOTTAKEN;
	int iPHT = pc & pht_mask;
	int iBHT = PHT[iPHT] & bht_mask;
	if (BHT[iBHT] > 1) {
		lResult = TAKEN;
	}

	int gResult = NOTTAKEN;
	int index = GHR & (choice_len - 1);

	if (gPred[index] > 1) {
		gResult = TAKEN;
	}

	if (choice[index] > 1) {
		return gResult;
	}
	else {
		return lResult;
	}
}

bool make_prediction_perceptron(unsigned int pc)
{
    int ghr = arrToint();
    int iPercep = (pc^ghr) & percep_mask;
    y_perceptron = percep_weights[iPercep][globalhistBits];
    for (int i = 0; i< globalhistBits; i++) { 
        y_perceptron += percep_weights[iPercep][i] * history[i];
    }
    
    if (y_perceptron < 0)
        return NOTTAKEN;
    return TAKEN;
}

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

void train_predictor_gshare(unsigned int pc, bool outcome)
{
	int xr = (GHR^pc) & bht_mask;
	if (outcome == TAKEN) {
		BHT[xr]++;
	}
	else {
		BHT[xr]--;
	}

	if (BHT[xr] > 3) {
		BHT[xr] = 3;
	}
	else if (BHT[xr] < 0) {
		BHT[xr] = 0;
	}

	GHR <<= 1;
	GHR &= bht_mask;
	if (outcome == TAKEN) {
		GHR += 1;
	}
}

void train_predictor_alpha21264(unsigned int pc, bool outcome)
{
	int lResult = NOTTAKEN;
	int iPHT = pc & pht_mask;
	int iBHT = PHT[iPHT] & bht_mask;
	if (BHT[iBHT] > 1) {
		lResult = TAKEN;
	}

	int gResult = NOTTAKEN;
	int index = GHR & (choice_len - 1);

	if (gPred[index] > 1) {
		gResult = TAKEN;
	}

	int choiceAdd = GHR & (choice_len - 1);

	if (gResult == outcome && lResult != outcome) {
		choice[choiceAdd]++;
	}
	else if (gResult != outcome && lResult == outcome) {
		choice[choiceAdd]--;
	}

	if (choice[choiceAdd] > 3) {
		choice[choiceAdd] = 3;
	}
	else if (choice[choiceAdd] < 0) {
		choice[choiceAdd] = 0;
	}

	if (outcome == TAKEN) {
		gPred[choiceAdd]++;
	}
	else {
		gPred[choiceAdd]--;
	}

	if (gPred[choiceAdd] > 3) {
		gPred[choiceAdd] = 3;
	}
	else if (gPred[choiceAdd] < 0) {
		gPred[choiceAdd] = 0;
	}

	GHR <<= 1;
	GHR &= (choice_len - 1);
	if (outcome == TAKEN) {
		GHR += 1;
	}

	train_predictor_local(pc, outcome);
}

int sign(float y) {
    if (y < 0.0)
        return -1;
    else
        return 1;
}

void train_predictor_perceptron(unsigned int pc, bool outcome)
{
    int result = -1;
    if (outcome == TAKEN) {
        result = TAKEN;
    }

    int ghr = arrToint();
    int indexPercep = (pc^ghr) & percep_mask;
    
    if ((sign(y_perceptron) != result) || (abs(y_perceptron) <= theta)) {
        percep_weights[indexPercep][globalhistBits] += result;
        for (int i = 0; i< globalhistBits; i++) {
            percep_weights[indexPercep][i] += (result * history[i]);
        }
    }
            
    for (int i = 0; i<= globalhistBits; i++) {
        if (percep_weights[indexPercep][i] > MAX_WEIGHT) {
            percep_weights[indexPercep][i] = MAX_WEIGHT;
        } else if (percep_weights[indexPercep][i] < MIN_WEIGHT) {
            percep_weights[indexPercep][i] = MIN_WEIGHT;
        }
    }
    
    for (int i = globalhistBits; i > 0; i--) {
          history[i] = history[i-1];
    }
    history[0] = result;
}


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
