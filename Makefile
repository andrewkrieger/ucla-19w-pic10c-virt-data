stats: main.o data_source.o
	g++ -o $@ $+

main.o: main.cpp data_source.h
data_source.o: data_source.cpp data_source.h
