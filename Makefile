all: cache
cache:
	g++ -o cache main.cpp

clean:
	rm -f cache
