#!/bin/bash


make clean; make

for filename in $PWD/../traces/*
do
	    echo "=========================================================================="
        echo "Trace $filename"
        echo "=========================================================================="

        for budgetNumber in 8 16 32 64 128 1024
        do
            echo "2-LEVEL LOCAL PREDICTOR for ${budgetNumber}K budget"
            ./predictor --local:$budgetNumber $filename
            echo ""
            echo "G-SHARE PREDICTOR for ${budgetNumber}K budget"
            ./predictor --gshare:$budgetNumber $filename
            echo ""
            echo "ALPHA21264 PREDICTOR for ${budgetNumber}K budget"
            ./predictor --alpha21264:$budgetNumber $filename
            echo ""
            echo "PERCEPTRON PREDICTOR for ${budgetNumber}K budget"
            ./predictor --perceptron:$budgetNumber $filename
       
            echo "__________________________________________________________________________"
        done

done


