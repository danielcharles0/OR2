STD_FLAGS = -ansi -Wall -pedantic -c
OBJS = ./obj/main.o ./obj/settings.o ./obj/reader.o ./obj/generator.o ./obj/point.o ./obj/tsp.o

ifdef DEBUG
	FLAGS = $(STD_FLAGS) -g
else
	FLAGS = $(STD_FLAGS) -O3
endif

main: $(OBJS)
	gcc -o main $(OBJS)

./obj/main.o: main.c
	mkdir -p ./obj
	gcc $(FLAGS) main.c -o ./obj/main.o

./obj/settings.o: ./tsp/input/settings/settings.h ./tsp/input/settings/settings.c
	gcc $(FLAGS) ./tsp/input/settings/settings.c -o ./obj/settings.o

./obj/reader.o: ./tsp/input/reader/reader.h ./tsp/input/reader/reader.c
	gcc $(FLAGS) ./tsp/input/reader/reader.c -o ./obj/reader.o

./obj/generator.o: ./tsp/input/generator/generator.h ./tsp/input/generator/generator.c
	gcc $(FLAGS) ./tsp/input/generator/generator.c -o ./obj/generator.o

./obj/point.o: ./tsp/input/point/point.h ./tsp/input/point/point.c
	gcc $(FLAGS) ./tsp/input/point/point.c -o ./obj/point.o

./obj/tsp.o: ./tsp/tsp.h ./tsp/tsp.c
	gcc $(FLAGS) ./tsp/tsp.c -o ./obj/tsp.o

debug:
	make DEBUG=1

clean:
	rm ./obj/*.o main

rebuild:
	make clean && make

# target: dependencies
# 	action

# https://pages.di.unipi.it/gadducci/PR1L-13/exeC/make.pdf
# We express how to update the target if a file in the dependency list is updated.