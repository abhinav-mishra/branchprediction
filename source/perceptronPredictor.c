#include "predictor.h"
#include <stdio.h>
#include <math.h>

extern int** percep_weights; //For perceptron
extern int* history; //For perceptron
extern float y_perceptron; //For making the prediction choice
extern int percep_length;
extern int percep_mask;
extern float theta;

// Assigning statically global history bits and pc bits
void initializeBits_perceptron()
{
    switch(budgetType) {
        case _8K:
            globalhistBits = _8KGLOBALHIST_percep;
            pcBits = _8KPCBITS_percep;
            break;
        case _16K:
            globalhistBits = _16KGLOBALHIST_percep;
            pcBits = _16KPCBITS_percep;
            break;
        case _32K:
            globalhistBits = _32KGLOBALHIST_percep;
            pcBits = _32KPCBITS_percep;
            break;
        case _64K:
            globalhistBits = _64KGLOBALHIST_percep;
            pcBits = _64KPCBITS_percep;
            break;
        case _128K:
            globalhistBits = _128KGLOBALHIST_percep;
            pcBits = _128KPCBITS_percep;
            break;
        case _1M:
            globalhistBits = _1MGLOBALHIST_percep;
            pcBits = _1MPCBITS_percep;
            break;
    }
}

int init_predictor_perceptron()
{
    int sizeinbits = 0;
    percep_length = 1 << pcBits;

    // Calculating theta from global history bits
    theta = (1.93 * (globalhistBits)) + 14;
    percep_mask = percep_length - 1;

    // Creating perceptron weigths 2-D table
    percep_weights = (int **)malloc(percep_length * sizeof(int*));
    for (int i = 0; i< percep_length; i++) {

        // Creating each weight row based on static size of global history bits
        percep_weights[i] = (int*)malloc((globalhistBits+1) * sizeof(int));
        for (int j = 0; j <= globalhistBits; j++) {
            // Initializing each weight to 0
            percep_weights[i][j] = 0;
        }
    }

    // Creating history table based on static global history bits
    history = (int*)malloc(globalhistBits * sizeof(int));
    for (int i = 0; i< globalhistBits; i++) {
        // Assigning all the past history as not taken
        history[i] = -1;
    }

    sizeinbits = ((globalhistBits+1) * percep_length * BW) + globalhistBits;
    printf("sizeinbits = %d+%d = %d\n", ((globalhistBits+1) * percep_length * BW), globalhistBits, sizeinbits);
}

// For getting ghr from history table
int arrToint() {
    int res = 0;
    for (int i=0; i<globalhistBits; i++) {
        if (history[i] == 1)
            res += (1<<i);
    }
    return res;
}

bool make_prediction_perceptron(unsigned int pc)
{
    int ghr = arrToint();

    // Getting row index using pc and ghr
    int iPercep = (pc^ghr) & percep_mask;

    // Initializing y value from last column (which is an extra column for this purpose only)
    y_perceptron = percep_weights[iPercep][globalhistBits];

    // Calculating new value of y based on history and weights in that row (iPercep)
    for (int i = 0; i< globalhistBits; i++) { 
        y_perceptron += percep_weights[iPercep][i] * history[i];
    }
    
    // Making prediction based on calculated y value
    if (y_perceptron < 0)
        return NOTTAKEN;
    return TAKEN;
}

// For getting the sign of y
int sign(float y) {
    if (y < 0.0)
        return -1;
    else
        return 1;
}

void train_predictor_perceptron(unsigned int pc, bool outcome)
{
    // Assigning result (-1 or 1) based on outcome (true or false)
    int result = -1;
    if (outcome == TAKEN) {
        result = TAKEN;
    }

    // Getting row index using pc and ghr
    int ghr = arrToint();
    int indexPercep = (pc^ghr) & percep_mask;
    
    // Checking the y sign with result || restricting to a limit using theta
    if ((sign(y_perceptron) != result) || (abs(y_perceptron) <= theta)) {
        percep_weights[indexPercep][globalhistBits] += result;
        
        // Updating all the weights in that row
        for (int i = 0; i< globalhistBits; i++) {
            percep_weights[indexPercep][i] += (result * history[i]);
        }
    }

    // Restricting each weight to MAX and MIN weight limit        
    for (int i = 0; i<= globalhistBits; i++) {
        if (percep_weights[indexPercep][i] > MAX_WEIGHT) {
            percep_weights[indexPercep][i] = MAX_WEIGHT;
        } else if (percep_weights[indexPercep][i] < MIN_WEIGHT) {
            percep_weights[indexPercep][i] = MIN_WEIGHT;
        }
    }
    
    // Updating the global history table by shifting and adding new result
    for (int i = globalhistBits; i > 0; i--) {
          history[i] = history[i-1];
    }
    history[0] = result;
}

