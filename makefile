shell379: main.o shell379.o test.o
	g++ -o shell379 main.o shell379.o 
	g++ -o test test.o


shell379.o: shell379.cpp shell379.h
	g++ -c shell379.cpp -o shell379.o

main.o: main.cpp
	g++ -c main.cpp -o main.o

test.o: test.cpp
	g++ -c test.cpp -o test.o

clean:
	rm ./*o ./shell379