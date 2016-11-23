#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NOTTAKEN  false
#define TAKEN     true

// Different Predictor Types
#define DEFAULT      0
#define GSHARE      1
#define LOCAL       2
#define ALPHA21264  3
#define PERCEPTRON  4

// For perceptron
#define BW  6 //bit width
#define MAX_WEIGHT ((1<<(BW-1))-1)
#define MIN_WEIGHT (-MAX_WEIGHT-1)

// Predictor Config
extern int globalhistBits;      // Number of bits for Global History
extern int localhistBits;       // Number of bits for Local History
extern int pcBits;              // Number of bits for PC index
extern int predictorType;       // Branch Predictor Type
extern int budgetType;          // Budget Limit in K

extern unsigned long int budget;

extern FILE * output_stream;
extern char * input_filename;
extern char * output_filename;

// Budget Types
#define _8K 8
#define _16K 16
#define _32K 32
#define _64K 64
#define _128K 128
#define _1M 1024


// Fixed bits for 2-Level Local Predictor
#define _8KLOCALHIST_local 10
#define _8KPCBITS_local 9

#define _16KLOCALHIST_local 11 
#define _16KPCBITS_local 10 

#define _32KLOCALHIST_local 12 
#define _32KPCBITS_local 11 

#define _64KLOCALHIST_local 12
#define _64KPCBITS_local 12 

#define _128KLOCALHIST_local 13
#define _128KPCBITS_local 13 

#define _1MLOCALHIST_local 17 
#define _1MPCBITS_local 15
    
// Fixed bits for G-share Predictor
#define _8KGLOBALHIST_gshare 12 

#define _16KGLOBALHIST_gshare 13 

#define _32KGLOBALHIST_gshare 14 

#define _64KGLOBALHIST_gshare 15 

#define _128KGLOBALHIST_gshare 16 

#define _1MGLOBALHIST_gshare 19

// Fixed bits for Alpha21264 Predictor
#define _8KLOCALHIST_alpha 4 
#define _8KGLOBALHIST_alpha 10
#define _8KPCBITS_alpha 10 

#define _16KLOCALHIST_alpha 10 
#define _16KGLOBALHIST_alpha 11 
#define _16KPCBITS_alpha  9

#define _32KLOCALHIST_alpha  11 
#define _32KGLOBALHIST_alpha 12 
#define _32KPCBITS_alpha  10 

#define _64KLOCALHIST_alpha  12 
#define _64KGLOBALHIST_alpha 13
#define _64KPCBITS_alpha  11 

#define _128KLOCALHIST_alpha  14 
#define _128KGLOBALHIST_alpha 13 
#define _128KPCBITS_alpha 12 

#define _1MLOCALHIST_alpha  16 
#define _1MGLOBALHIST_alpha 16 
#define _1MPCBITS_alpha  15

// Fixed bits for Perceptron Predictor
#define _8KGLOBALHIST_percep 9 
#define _8KPCBITS_percep 7

#define _16KGLOBALHIST_percep 20
#define _16KPCBITS_percep 7 

#define _32KGLOBALHIST_percep 20
#define _32KPCBITS_percep 8 

#define _64KGLOBALHIST_percep 20 
#define _64KPCBITS_percep 9 

#define _128KGLOBALHIST_percep 32 
#define _128KPCBITS_percep 9 

#define _1MGLOBALHIST_percep  34
#define _1MPCBITS_percep 12

/*
  Initialize the predictor.
*/
int init_predictor ();

/*
  Make a prediction for conditional branch instruction at PC 'pc'.
  Returning true indicates a prediction of taken; returning false
  indicates a prediction of not taken.
*/
bool make_prediction (unsigned int pc);

/*
  Train the predictor the last executed branch at PC 'pc' and with
  outcome 'outcome' (true indicates that the branch was taken, false
  indicates that the branch was not taken).
*/
void train_predictor (unsigned int pc, bool outcome);

#endif
