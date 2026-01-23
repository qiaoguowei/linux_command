.PHONY: clean

CC = g++
RM = rm

all: abstract_factory chain observer proxy reflect singleton
	@echo "compile abstract_factory chain observer proxy reflect singleton"

abstrct_factory: abstract_factory.cpp
	$(CC) -o $@ $^

chain: chain_of_responsibility.cpp
	$(CC) -o $@ $^

observer: observer.cpp
	$(CC) -o $@ $^

proxy: proxy.cpp
	$(CC) -o $@ $^

reflect: reflect.cpp
	$(CC) -o $@ $^

singleton: singleton.cpp
	$(CC) -o $@ $^ -lpthread

clean:
	$(RM) abstract_factory chain observer proxy reflect singleton