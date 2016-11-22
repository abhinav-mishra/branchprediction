#include "predictor.h"
#include <stdio.h>
#include <math.h>

extern int* BHT;
extern int* PHT;
extern int bht_mask;
extern int pht_mask;
extern int bht_length;
extern int pht_length;

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

bool make_prediction_local(unsigned int pc) {
	int iPHT = pc & pht_mask;
	int iBHT = PHT[iPHT] & bht_mask;

	if (BHT[iBHT] > 1) {
		return TAKEN;
	}
	return NOTTAKEN;
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

