#include "predictor.h"
#include <stdio.h>
#include <math.h>

extern int GHR;
extern int *BHT;

extern int bht_mask;
extern int bht_length;


// Assigning global history bits from static value
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
    bht_length = 1 << globalhistBits; // Size of BHT from global history bits
	bht_mask = (1 << globalhistBits) - 1;

    // Creating branch table from static value
    BHT = (int*)malloc(bht_length * sizeof(int));
	for (int i = 0; i< bht_length; i++) {
		BHT[i] = 0;
	}
    sizeinbits = (globalhistBits)+(bht_length * 2);
    printf("Size in bits used = %d+%d = %d \n", (globalhistBits), (bht_length * 2), sizeinbits);
    if (sizeinbits < budget)
	    fprintf(output_stream, "G-Share\t%s\t%d\t\t\t%d\t%lu\t%f\t", input_filename, globalhistBits, sizeinbits, budget, (float)sizeinbits/budget*100);
	return sizeinbits;
}

bool make_prediction_gshare(unsigned int pc)
{
	int xr = (GHR^pc) & bht_mask; // Getting index by xoring with pc
	if (BHT[xr] > 1) {
		return TAKEN;
	}
	return NOTTAKEN;
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

    // restricting saturating counters to 2 bit
	if (BHT[xr] > 3) {
		BHT[xr] = 3;
	}
	else if (BHT[xr] < 0) {
		BHT[xr] = 0;
	}

    // Shifting and updating GHR
	GHR <<= 1;
	GHR &= bht_mask;
	if (outcome == TAKEN) {
		GHR += 1;
	}
}

