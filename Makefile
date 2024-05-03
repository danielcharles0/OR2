STD_FLAGS = -std=c99 -Werror -Wall -pedantic -c
OBJS = ./obj/main.o ./obj/settings.o ./obj/utility.o ./obj/validator.o ./obj/reader.o ./obj/generator.o ./obj/point.o ./obj/output.o ./obj/tsp.o ./obj/nearestneighbor.o ./obj/random.o ./obj/2opt.o ./obj/tabu.o ./obj/vns.o ./obj/array.o ./obj/refinement.o ./obj/cplex.o ./obj/benders.o ./obj/fischetti.o ./obj/candidate.o
# OBJS = $(find ./obj -exec printf '%s ' {} +)
# OBJS = $(wildcard ./obj/*.o)

CPLEX_LINK_PATH = "/Users/Shared/lib/cplex/CPLEX_Studio2211/cplex/lib/arch/static_pic"
CPLEX_LIB_PATH = "/Users/Shared/lib/cplex/CPLEX_Studio2211/cplex/include"

LIBS = -L $(CPLEX_LINK_PATH) -lcplex

ifdef DEBUG
	FLAGS = $(STD_FLAGS) -g
else
	FLAGS = $(STD_FLAGS) -O3
endif

main: $(OBJS)
	rm -f ./tsp/output/cplex/model.lp
	gcc -o main $(OBJS) $(LIBS)

./obj/main.o: main.c
	mkdir -p ./obj ./gnuplot_out ./cplex_out ./tsp/output/cplex
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
	gcc $(FLAGS) ./tsp/tsp.c -o ./obj/tsp.o -I $(CPLEX_LIB_PATH)

./obj/nearestneighbor.o: ./tsp/algorithms/nearestneighbor/nearestneighbor.h ./tsp/algorithms/nearestneighbor/nearestneighbor.c
	gcc $(FLAGS) ./tsp/algorithms/nearestneighbor/nearestneighbor.c -o ./obj/nearestneighbor.o

./obj/random.o: ./tsp/algorithms/random/random.h ./tsp/algorithms/random/random.c
	gcc $(FLAGS) ./tsp/algorithms/random/random.c -o ./obj/random.o

./obj/2opt.o: ./tsp/algorithms/refinement/2opt/2opt.h ./tsp/algorithms/refinement/2opt/2opt.c
	gcc $(FLAGS) ./tsp/algorithms/refinement/2opt/2opt.c -o ./obj/2opt.o

./obj/tabu.o: ./tsp/algorithms/refinement/tabu/tabu.h ./tsp/algorithms/refinement/tabu/tabu.c
	gcc $(FLAGS) ./tsp/algorithms/refinement/tabu/tabu.c -o ./obj/tabu.o

./obj/vns.o: ./tsp/algorithms/refinement/vns/vns.h ./tsp/algorithms/refinement/vns/vns.c
	gcc $(FLAGS) ./tsp/algorithms/refinement/vns/vns.c -o ./obj/vns.o

./obj/array.o: ./tsp/array/array.h ./tsp/array/array.c
	gcc $(FLAGS) ./tsp/array/array.c -o ./obj/array.o

./obj/refinement.o: ./tsp/algorithms/refinement/refinement.h ./tsp/algorithms/refinement/refinement.c
	gcc $(FLAGS) ./tsp/algorithms/refinement/refinement.c -o ./obj/refinement.o

./obj/cplex.o: ./tsp/algorithms/cplex/cplex.h ./tsp/algorithms/cplex/cplex.c
	gcc $(FLAGS) ./tsp/algorithms/cplex/cplex.c -o ./obj/cplex.o -I $(CPLEX_LIB_PATH)

./obj/benders.o: ./tsp/algorithms/cplex/benders/benders.h ./tsp/algorithms/cplex/benders/benders.c
	gcc $(FLAGS) ./tsp/algorithms/cplex/benders/benders.c -o ./obj/benders.o -I $(CPLEX_LIB_PATH)

./obj/fischetti.o: ./tsp/lib/fischetti/fischetti.h ./tsp/lib/fischetti/fischetti.c
	gcc $(FLAGS) ./tsp/lib/fischetti/fischetti.c -o ./obj/fischetti.o -I $(CPLEX_LIB_PATH)

./obj/candidate.o: ./tsp/algorithms/cplex/candidate/candidate.h ./tsp/algorithms/cplex/candidate/candidate.c
	gcc $(FLAGS) ./tsp/algorithms/cplex/candidate/candidate.c -o ./obj/candidate.o -I $(CPLEX_LIB_PATH)

debug:
	make DEBUG=1

clean:
	rm -f ./obj/*.o main

rebuild:
	make clean && make

# reference: https://www.leandro-coelho.com/install-and-run-concorde-with-cplex/
# NOTE:
#	- Before running make concorde please fix the passage 7, step 'avoid error CPX_PARAM_FASTMIP' of the reference
#	- reference: https://www.ibm.com/docs/en/icos/20.1.0?topic=s-cpxxsavwrite-cpxsavwrite \
#		substitute the deprecated methods CPXXsavwrite & CPXsavwrite with the new ones \
#		reference: https://www.ibm.com/docs/en/icos/22.1.1?topic=z-cpxxwriteprob-cpxwriteprob
concorde:
	mkdir -p /Users/Shared/lib/concorde/CPXREFERENCES /Users/Shared/lib/concorde/build
	ln -s -f /Users/Shared/lib/cplex/CPLEX_Studio2211/cplex/lib/arch/static_pic/libcplex.a /Users/Shared/lib/concorde/CPXREFERENCES/libcplex.a

	ln -s -f /Users/Shared/lib/cplex/CPLEX_Studio2211/cplex/include/ilcplex/cplex.h /Users/Shared/lib/concorde/CPXREFERENCES/cplex.h
#	ln -s -f /Users/Shared/lib/cplex/CPLEX_Studio2211/cplex/include/ilcplex/*.h /Users/Shared/lib/concorde/CPXREFERENCES
	ln -s -f /Users/Shared/lib/cplex/CPLEX_Studio2211/cplex/include/ilcplex/cpxconst.h /Users/Shared/lib/concorde/CPXREFERENCES/cpxconst.h
	
	cd /Users/Shared/lib/concorde/build \
		&& /Users/Shared/lib/concorde/configure --host=darwin --with-cplex=/Users/Shared/lib/concorde/CPXREFERENCES \
		&& make

#	cd /Users/Shared/lib/concorde/build \
#		&& mv concorde.a libconcorde.a \
#		&& make

# target: dependencies
# 	action

# https://pages.di.unipi.it/gadducci/PR1L-13/exeC/make.pdf
# We express how to update the target if a file in the dependency list is updated.