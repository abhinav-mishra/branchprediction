# CSE240A project - Branch Prediction

How to run all predictors for all budget sizes for all traces:
    
    ./runAllPredictors.sh

How to run a particular predictor:

   Step 1. make clean
   Step 2. make
   Step 3. ./predictor --<predictor>:<budget> <filename>
   Step 4. Example: ./predictor --local:128 ../traces/DIST-INT-1 

Usage Help:

    Keyword for <predictor> :

        local ---> 2-level local predictor
        gshare ---> G-Share predictor
        alpha21264 ---> Alpha-21264 predictor
        perceptron ---> Perceptron predictor

    Keyword for <budget> :
        8 ---> 8K + 64 bits
        16 ---> 16K + 128 bits
        32 ---> 32K + 256 bits
        64 ---> 64K + 512 bits
        128 ---> 128K + 1K bits
        1024 ----> 1M + 4K bits
