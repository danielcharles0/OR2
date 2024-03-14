STD_FLAGS = -std=c99 -Werror -Wall -pedantic -c
OBJS = ./obj/main.o ./obj/settings.o ./obj/utility.o ./obj/validator.o ./obj/reader.o ./obj/generator.o ./obj/point.o ./obj/output.o ./obj/tsp.o ./obj/nearestneighbor.o ./obj/random.o ./obj/2opt.o ./obj/tabu.o

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

./obj/utility.o: ./tsp/utility/utility.h ./tsp/utility/utility.c
	gcc $(FLAGS) ./tsp/utility/utility.c -o ./obj/utility.o

./obj/validator.o: ./tsp/input/settings/validator/validator.h ./tsp/input/settings/validator/validator.c
	gcc $(FLAGS) ./tsp/input/settings/validator/validator.c -o ./obj/validator.o

./obj/reader.o: ./tsp/input/reader/reader.h ./tsp/input/reader/reader.c
	gcc $(FLAGS) ./tsp/input/reader/reader.c -o ./obj/reader.o

./obj/generator.o: ./tsp/input/generator/generator.h ./tsp/input/generator/generator.c
	gcc $(FLAGS) ./tsp/input/generator/generator.c -o ./obj/generator.o

./obj/point.o: ./tsp/point/point.h ./tsp/point/point.c
	gcc $(FLAGS) ./tsp/point/point.c -o ./obj/point.o

./obj/output.o: ./tsp/output/output.h ./tsp/output/output.c
	gcc $(FLAGS) ./tsp/output/output.c -o ./obj/output.o

./obj/tsp.o: ./tsp/tsp.h ./tsp/tsp.c
	gcc $(FLAGS) ./tsp/tsp.c -o ./obj/tsp.o

./obj/nearestneighbor.o: ./tsp/algorithms/nearestneighbor/nearestneighbor.h ./tsp/algorithms/nearestneighbor/nearestneighbor.c
	gcc $(FLAGS) ./tsp/algorithms/nearestneighbor/nearestneighbor.c -o ./obj/nearestneighbor.o

./obj/random.o: ./tsp/algorithms/random/random.h ./tsp/algorithms/random/random.c
	gcc $(FLAGS) ./tsp/algorithms/random/random.c -o ./obj/random.o

./obj/2opt.o: ./tsp/algorithms/refinement/2opt/2opt.h ./tsp/algorithms/refinement/2opt/2opt.c
	gcc $(FLAGS) ./tsp/algorithms/refinement/2opt/2opt.c -o ./obj/2opt.o

./obj/tabu.o: ./tsp/algorithms/refinement/tabu/tabu.h ./tsp/algorithms/refinement/tabu/tabu.c
	gcc $(FLAGS) ./tsp/algorithms/refinement/tabu/tabu.c -o ./obj/tabu.o

debug:
	make DEBUG=1

clean:
	rm -f ./obj/*.o main

rebuild:
	make clean && make

# target: dependencies
# 	action

# https://pages.di.unipi.it/gadducci/PR1L-13/exeC/make.pdf
# We express how to update the target if a file in the dependency list is updated.