STD_FLAGS = -Wall -pedantic -c
OBJS = ./obj/main.o ./obj/parser.o

ifdef DEBUG
	FLAGS = $(STD_FLAGS) -g
else
	FLAGS = $(STD_FLAGS) -O3
endif

main: $(OBJS)
	gcc -o main $(OBJS)

./obj/main.o: main.c
	gcc $(FLAGS) main.c -o ./obj/main.o

./obj/parser.o: ./input/parser/parser.h ./input/parser/parser.c
	gcc $(FLAGS) ./input/parser/parser.c -o ./obj/parser.o

debug:
	make DEBUG=1

clean:
	rm ./obj/*.o main

rebuild:
	make clean && make

# target: dependencies
# 	action