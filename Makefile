# Profeanu Ioana, 333CA
build:
	g++ perfect_powers.cpp processing.cpp mappers.cpp reducers.cpp -o tema1 -lpthread -lm -Wall -Werror

clean:
	rm -rf tema1
