OBJS	= predictor.o predictorTypes.o
OUT	= predictors
FLAGS	 = -g -c -Wall

all: $(OBJS)
	g++ -g $(OBJS) -o $(OUT) $(LFLAGS)

predictor.o: predictor.cpp
	g++ $(FLAGS) predictor.cpp -std=c++14

predictorTypes.o: predictorTypes.cpp
	g++ $(FLAGS) predictorTypes.cpp -std=c++14


clean:
	rm -f $(OBJS) $(OUT)