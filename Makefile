CC=gcc-12
SOURCES=$(wildcard ./src/*.c) $(wildcard ./src/packet/*.c)
OPTIMIZATION_LEVEL=-O2
INCLUDE=-I ./lib/tomlc99 -I ./lib/base64/include -I ./src
FLAGS=-Wall -Werror $(OPTIMIZATION_LEVEL)

ifeq (, $(shell which mold))
	LINKER=
else
	LINKER=-fuse-ld=mold
endif

optimized: optimized_objects
	$(CC) ./*.o ./bin/*.o -o cinecraft $(FLAGS)
	$(MAKE) clean
optimized_objects:
	$(CC) -c $(SOURCES) $(FLAGS) $(INCLUDE)
libs: lib_dirs toml base64
lib_dirs:
	mkdir -p bin
toml:
	cd ./lib/tomlc99 && $(CC) -c toml.c $(OPTIMIZATION_LEVEL)
	mv ./lib/tomlc99/toml.o ./bin/toml.o 
base64:
	cd ./lib/base64 && $(CC) -c src/cencode.c $(OPTIMIZATION_LEVEL) -I ./include
	mv ./lib/base64/cencode.o ./bin/cencode.o
clean:
	rm -f ./*.o
