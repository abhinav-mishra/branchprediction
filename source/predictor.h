#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
  Define all your tables and their sizes here.
  All tables must be statically-sized.
  Please don't call malloc () or any of its relatives within your
  program.  The predictor will have a budget, namely (32K + 256) bits
  (not bytes).  That encompasses all storage (all tables, plus GHR, if
  necessary -- but not PC) used for the predictor.  That may not be
  the amount of storage your predictor uses, however -- for example,
  you may implement 2-bit predictors with a table of ints, in which
  case the simulator will use more memory -- that's okay, we're only
  concerned about the memory used by the simulated branch predictor.
*/

#define NOTTAKEN  false
#define TAKEN     true

// Different Predictor Types
#define DEFAULT      0
#define GSHARE      1
#define LOCAL       2
#define ALPHA21264  3
#define PERCEPTRON  4
extern const char *predictorName[];

// Predictor Configuration
extern int globalhistBits;      // Number of bits for Global History
extern int localhistBits;       // Number of bits for Local History
extern int pcBits;              // Number of bits for PC index
extern int predictorType;       // Branch Predictor Type

extern unsigned long int budget;

extern FILE * output_stream;
extern char * input_filename;
extern char * output_filename;
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
