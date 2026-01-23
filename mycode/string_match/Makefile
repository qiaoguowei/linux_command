
all: brute_force rabin_karp

brute_force: brute_force.cpp
	g++ -o $@ $^ -fsanitize=address -O0 -g

rabin_karp: rabin_karp.cpp
	g++ -o $@ $^

clean:
	rm brute_force rabin_karp