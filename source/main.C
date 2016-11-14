#include <stdio.h>
#include <netinet/in.h>
#include "traceread.h"
#include "predictor.h"

FILE * stream;
char *buf = NULL;
size_t len = 0;

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
        sscanf(arg + 13, "%d:%d", &globalhistBits, &pcBits);
	}
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
		  setup_trace(argv[i]);  
	  }
  }

  // Initialize the predictor
  init_predictor ();

  // Read the number of instructions from the trace
  uint32_t stat_num_insts = 0;
  if (fread (&stat_num_insts, sizeof (uint32_t), 1, stream) != 1) {
    printf ("Could not read intput file\n");
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

  if (argc == 2)
    close_trace ();
  
  return 0;
}
