OBJS	= main.o
OUT		= cache-sim
FLAGS	= -g -c -Wall

all: $(OBJS)
	g++ -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	g++ $(FLAGS) main.cpp -std=c++14

clean:
	rm -f $(OBJS) $(OUT)

