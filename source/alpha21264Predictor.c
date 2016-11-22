#include "predictor.h"
#include <stdio.h>
#include <math.h>

extern int GHR;

extern int* BHT;
extern int* PHT;
extern int* gPred;
extern int* choice;

extern int bht_mask;
extern int pht_mask;
extern int bht_length;
extern int pht_length;
extern int gPred_length;
extern int choice_len;

extern void train_predictor_local(unsigned int pc, bool outcome);

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
