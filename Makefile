all: cache
cache:
	g++ -o cache main.cpp
mpi:
	g++ -o criba criba.cpp
	mpic++ -o cribampi cribampi.cpp
clean:
	rm -f cache
