project1:	main.o symbols.o
	gcc -o project4 main.o symbols.o

main.o:	main.c
	gcc -c main.c

symbols.o:	symbols.c
	gcc -c symbols.c


clean:	
	rm *.o -f
	touch *.c
	rm project4 -f
