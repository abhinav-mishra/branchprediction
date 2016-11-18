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

// Process the predictor option and number of bits
// Returns 1 if Successful
int handle_cmd_options(char *arg)
{
	if (!strcmp(arg, "--static")) {
		predictorType = DEFAULT;
	}
	else if (!strncmp(arg, "--gshare:", 9)) {
		predictorType = GSHARE;
		sscanf(arg + 9, "%d", &globalhistBits);
	}
	else if (!strncmp(arg, "--local:", 8)) {
		predictorType = LOCAL;
		sscanf(arg + 8, "%d:%d", &localhistBits, &pcBits);
	}
	else if (!strncmp(arg, "--alpha21264:", 13)) {
		predictorType = ALPHA21264;
		sscanf(arg + 13, "%d:%d:%d", &globalhistBits, &localhistBits, &pcBits);
	}
	else if (!strncmp(arg, "--perceptron:", 13)) {
		predictorType = PERCEPTRON;
	}
  else if (!strncmp(arg, "--budget:", 9)) {
    sscanf(arg + 9, "%lu", &budget);
  }
  // else if (!strncmp(arg, "--output:", 9)) {
  //   sscanf(arg + 9, "%s", output_filename);
  // }
	else {
		return 0;
	}

	return 1;
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
  //fprintf(output_stream, "Predictor\tInput\tGlobal History Bits\tLocal History Bits\tPC Bits\tTotal Budget\tBranches\tIncorrect\tMisprediction Rate\n");
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

  // Set default
  predictorType = DEFAULT;

  // Process cmdline Arguments
  for (int i = 1; i < argc; ++i) {
	  if (!strncmp(argv[i], "--", 2)) {
		  if (!handle_cmd_options(argv[i])) {
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
    //printf("%x %d\n",pc, outcome);    
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
