M = minimal
T = test
TK = test-kec
TS = test-scaling
CFLAGS = -g -Wall -Wextra -pedantic -O -std=c++11

all: $(M) $(T) $(TK) $(TS)

$(M): $(M).o
	g++ $(CFLAGS) $(M).o -o $(M)
$(M).o: $(M).cpp Map.hpp
	g++ $(CFLAGS) -c $(M).cpp
$(T): $(T).o
	g++ $(CFLAGS) $(T).o -o $(T)
$(T).o: $(T).cpp Map.hpp
	g++ $(CFLAGS) -c $(T).cpp
$(TK): $(TK).o
	g++ $(CFLAGS) $(TK).o -o $(TK)
$(TK).o: $(TK).cpp Map.hpp
	g++ $(CFLAGS) -c $(TK).cpp
$(TS): $(TS).o
	g++ $(CFLAGS) $(TS).o -o $(TS)
$(TS).o: $(TS).cpp Map.hpp
	g++ $(CFLAGS) -c $(TS).cpp



run: all
	./$(M)

clean:
	rm -rf *.o $(M) $(T) $(TK) $(TS)

memcheck: all
	valgrind ./$(M)
memcheck++: all
	valgrind --leak-check=full --track-origins=yes -v ./$(M)
