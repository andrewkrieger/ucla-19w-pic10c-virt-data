# The Makefile is used with a program called `make`, to call the compiler and
# tell the compiler which source files depend on which other files. We may talk
# about Makefiles later this quarter. Briefly...

# This rule says that the program named 'stats' is built from main.o and
# data_source.o, using the recipe `g++ -o <output-file> <input-files>
stats: main.o data_source.o
	g++ -o $@ $+

# These rules respectively say that maino anddata_source.o depend on their .cpp
# files and on data_source.h. `make` has built-in recipes for building `*.o'
# files from '*.cpp' files using a C++ compiler.
main.o: main.cpp data_source.h
data_source.o: data_source.cpp data_source.h
