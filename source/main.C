#include <stdio.h>
#include <netinet/in.h>
#include "traceread.h"
#include "predictor.h"

FILE * stream;
FILE * output_stream;
char *buf = NULL;
size_t len = 0;
char * input_filename;
char * output_filename;
unsigned long int budget;

// Process the predictor type option with budget type
// Returns 1 if Successful
int handleCmdLineOptions(char *arg)
{
	if (!strcmp(arg, "--static")) {
		predictorType = DEFAULT;
	}
	else if (!strncmp(arg, "--gshare:", 9)) {
		predictorType = GSHARE;
		sscanf(arg + 9, "%d", &budgetType);
	}
	else if (!strncmp(arg, "--local:", 8)) {
		predictorType = LOCAL;
		sscanf(arg + 8, "%d", &budgetType);
	}
	else if (!strncmp(arg, "--alpha21264:", 13)) {
		predictorType = ALPHA21264;
		sscanf(arg + 13, "%d", &budgetType);
	}
	else if (!strncmp(arg, "--perceptron:", 13)) {
		predictorType = PERCEPTRON;
        sscanf(arg + 13, "%d", &budgetType);
	}
	else {
		return 0;
	}

	return 1;
}

// For getting the budget bits for calculating % budget used
int assignBudget() {
    if (budgetType == 8)
        return 8256;
    if (budgetType == 16)
        return 16512;
    if (budgetType == 32)
        return 33024;
    if (budgetType == 64)
        return 66048;
    if (budgetType == 128)
        return 132096;
    if (budgetType == 1024)
        return 1052672;
}

void setup_trace (const char * filename)
{
  if (filename == NULL)
    stream = stdin;
  else {
    stream = fopen (filename, "r");
  }
}

void close_trace ()
{
  fclose (stream);
}

void setup_output (const char * filename)
{
  if (filename == NULL)
    output_stream = stdout;
  else {
    output_stream = fopen (filename, "a");
  }
}

void close_output ()
{
  fclose (output_stream);
}


int main (int argc, char * argv[])
{
  int mis_preds = 0;
  int num_branches = 0;
  uint32_t pc = 0;
  bool outcome = false;

  predictorType = DEFAULT;

  // Process command line arguments
  for (int i = 1; i < argc; ++i) {
	  if (!strncmp(argv[i], "--", 2)) {
		  if (!handleCmdLineOptions(argv[i])) {
			  printf("Unrecognized option %s\n", argv[i]);
			  exit(1);
		  }
	  }
	  else {
      input_filename = argv[i];
		  setup_trace(input_filename);  
      setup_output("experiment.csv");
	  }
  }

  // Get budget size in bits
  budget = assignBudget();
  // Initialize the predictor
  int size = init_predictor ();

  // Read the number of instructions from the trace
  uint32_t stat_num_insts = 0;
  if (fread (&stat_num_insts, sizeof (uint32_t), 1, stream) != 1) {
    printf ("Could not read input file\n");
    return 1;
  }
  stat_num_insts = ntohl (stat_num_insts);

  // Read each branch from the trace
  while (read_branch (&pc, &outcome)) {

    pc = ntohl (pc);

    num_branches ++;
    
    // Make a prediction and compare with actual outcome
    if (make_prediction (pc) != outcome)
      mis_preds ++;

    // Train the predictor
    train_predictor (pc, outcome);
  }

  // Print out the mispredict statistics
  printf ("Branches\t\t%8d\n", num_branches);
  printf ("Incorrect\t\t%8d\n", mis_preds);
  float mis_pred_rate = (float)mis_preds / float(stat_num_insts / 1000);
  printf ("1000*(wrong_cc_predicts/total insts) ~ 1000*%8d/%8d = %7.3f\n", mis_preds, stat_num_insts, mis_pred_rate);
  if (size < budget)
    fprintf(output_stream, "%d\t%d\t%7.3f\n", num_branches, mis_preds, mis_pred_rate);

  if (argc == 2)
    close_trace ();
  close_output();
  
  return 0;
}
