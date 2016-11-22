CC=g++
OPTS=-g

all: main.o traceread.o predictor.o localPredictor.o gsharePredictor.o alpha21264Predictor.o perceptronPredictor.o
	$(CC) $(OPTS) -lm -o predictor main.o traceread.o predictor.o localPredictor.o gsharePredictor.o alpha21264Predictor.o perceptronPredictor.o

main.o: main.C traceread.h predictor.h
	$(CC) $(OPTS) -c main.C

traceread.o: traceread.h traceread.C
	$(CC) $(OPTS) -c traceread.C

predictor.o: predictor.h predictor.C
	$(CC) $(OPTS) -c predictor.C

localPredictor.o: predictor.h localPredictor.c
	$(CC) $(OPTS) -c localPredictor.c

gsharePredictor.o: predictor.h gsharePredictor.c
	$(CC) $(OPTS) -c gsharePredictor.c

alpha21264Predictor.o: predictor.h localPredictor.c alpha21264Predictor.c
	$(CC) $(OPTS) -c localPredictor.c alpha21264Predictor.c

perceptronPredictor.o: predictor.h perceptronPredictor.c
	$(CC) $(OPTS) -c perceptronPredictor.c

clean:
	rm -f *.o predictor;
