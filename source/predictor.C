#include "predictor.h"
#include <stdio.h>
#include <math.h>

const char *predictorName[5] = { "Static", "Gshare", "Local",
"Alpha21264", "Perceptron" };

int globalhistBits;	// Number of bits for Global History
int localhistBits;	// Number of bits for Local History
int pcBits;			// Number of bits for PC index
int predictorType;  // Branch Predictor Type
int budgetType;     // Budget Limit in K

//intialize predictors
void initializeBits_local();
void initializeBits_gshare();
void initializeBits_alpha21264();
void initializeBits_perceptron();
int init_predictor_gshare();
int init_predictor_local();
int init_predictor_alpha21264();
int init_predictor_perceptron();


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
float theta = 0.0;

int bht_mask = 0;
int pht_mask = 0;
int bht_length = 0;
int pht_length = 0;
int gPred_length = 0;
int choice_len = 0;

int init_predictor ()
{
	switch (predictorType) {
		case DEFAULT:
			break;
		case LOCAL:
            //initializeBits_local();
			return init_predictor_local();
			break;
		case GSHARE:
            //initializeBits_gshare();
			return init_predictor_gshare();
			break;
		case ALPHA21264:
            //initializeBits_alpha21264();
			return init_predictor_alpha21264();
			break;
		case PERCEPTRON:
            //initializeBits_perceptron();
			return init_predictor_perceptron();
			break;
		default:
			return -1;
			break;
		}
	return -1;
}

void initializeBits_local()
{
    switch(budgetType) {
        case _8K:
            localhistBits = _8KLOCALHIST_local;
            pcBits = _8KPCBITS_local;
            break;
        case _16K:
            localhistBits = _16KLOCALHIST_local;
            pcBits = _16KPCBITS_local;
            break;
        case _32K:
            localhistBits = _32KLOCALHIST_local;
            pcBits = _32KPCBITS_local;
            break;
        case _64K:
            localhistBits = _64KLOCALHIST_local;
            pcBits = _64KPCBITS_local;
            break;
        case _128K:
            localhistBits = _128KLOCALHIST_local;
            pcBits = _128KPCBITS_local;
            break;
        case _1M:
            localhistBits = _1MLOCALHIST_local;
            pcBits = _1MPCBITS_local;
            break;
    }
}

int init_predictor_local() 
{
    int sizeinbits = 0;
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
    
    sizeinbits = (pht_length*localhistBits)+(bht_length * 2);
    printf("sizeinbits = %d+%d = %d \n", (pht_length * localhistBits), (bht_length * 2), sizeinbits);
    printf("Percent of budget: %f\n",(float)sizeinbits/budget);
    if (sizeinbits < budget)
	    fprintf(output_stream, "Local\t%s\t\t%d\t%d\t%d\t%lu\t%f\t", input_filename, localhistBits, pcBits, sizeinbits, budget, (float)sizeinbits/budget*100);
	return sizeinbits;
}

void initializeBits_gshare()
{
    switch(budgetType) {
        case _8K:
            globalhistBits = _8KGLOBALHIST_gshare;
            break;
        case _16K:
            globalhistBits = _16KGLOBALHIST_gshare;
            break;
        case _32K:
            globalhistBits = _32KGLOBALHIST_gshare;
            break;
        case _64K:
            globalhistBits = _64KGLOBALHIST_gshare;
            break;
        case _128K:
            globalhistBits = _128KGLOBALHIST_gshare;
            break;
        case _1M:
            globalhistBits = _1MGLOBALHIST_gshare;
            break;
    }
}

int init_predictor_gshare()
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
    if (sizeinbits < budget)
	    fprintf(output_stream, "G-Share\t%s\t%d\t\t\t%d\t%lu\t%f\t", input_filename, globalhistBits, sizeinbits, budget, (float)sizeinbits/budget*100);
	return sizeinbits;
}

void initializeBits_alpha21264()
{
    switch(budgetType) {
        case _8K:
            globalhistBits = _8KGLOBALHIST_alpha;
            localhistBits = _8KLOCALHIST_alpha;
            pcBits = _8KPCBITS_alpha;
            break;
        case _16K:
            globalhistBits = _16KGLOBALHIST_alpha;
            localhistBits = _16KLOCALHIST_alpha;
            pcBits = _16KPCBITS_alpha;
            break;
        case _32K:
            globalhistBits = _32KGLOBALHIST_alpha;
            localhistBits = _32KLOCALHIST_alpha;
            pcBits = _32KPCBITS_alpha;
            break;
        case _64K:
            globalhistBits = _64KGLOBALHIST_alpha;
            localhistBits = _64KLOCALHIST_alpha;
            pcBits = _64KPCBITS_alpha;
            break;
        case _128K:
            globalhistBits = _128KGLOBALHIST_alpha;
            localhistBits = _128KLOCALHIST_alpha;
            pcBits = _128KPCBITS_alpha;
            break;
        case _1M:
            globalhistBits = _1MGLOBALHIST_alpha;
            localhistBits = _1MLOCALHIST_alpha;
            pcBits = _1MPCBITS_alpha;
            break;
    }
}

int init_predictor_alpha21264()
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
    if (sizeinbits < budget)
	    fprintf(output_stream, "Alpha 21264\t%s\t%d\t%d\t%d\t%d\t%lu\t%f\t", input_filename, globalhistBits, localhistBits, pcBits, sizeinbits, budget, (float)sizeinbits/budget*100);
	return sizeinbits;
}


void initializeBits_perceptron()
{
    switch(budgetType) {
        case _8K:
            localhistBits = _8KLOCALHIST_percep;
            pcBits = _8KPCBITS_percep;
            break;
        case _16K:
            localhistBits = _16KLOCALHIST_percep;
            pcBits = _16KPCBITS_percep;
            break;
        case _32K:
            localhistBits = _32KLOCALHIST_percep;
            pcBits = _32KPCBITS_percep;
            break;
        case _64K:
            localhistBits = _64KLOCALHIST_percep;
            pcBits = _64KPCBITS_percep;
            break;
        case _128K:
            localhistBits = _128KLOCALHIST_percep;
            pcBits = _128KPCBITS_percep;
            break;
        case _1M:
            localhistBits = _1MLOCALHIST_percep;
            pcBits = _1MPCBITS_percep;
            break;
    }
}

int init_predictor_perceptron()
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
