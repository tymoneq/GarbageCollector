FLAGS=-Wshadow -Wall -fsanitize=address,leak,undefined,float-divide-by-zero,float-cast-overflow -g -fsignaling-nans -fnon-call-exceptions -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC 


run: main.c gc.c gc.h
	gcc $(FLAGS) main.c gc.c -o main
	./main