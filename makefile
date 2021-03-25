assembler: main.o first_run.o init.o structs.o check_funcs.o sec_run.o clean.o print_to_file.o
	gcc -g -ansi -Wall -pedantic  main.o first_run.o init.o sec_run.o structs.o clean.o print_to_file.o check_funcs.o -o assembler

main.o: main.c   
	gcc -c -ansi -Wall -pedantic main.c -o main.o
			
first_run.o: first_run.c first_run.h structs.h
	gcc -c -ansi -Wall -pedantic  first_run.c -o first_run.o

sec_run.o: sec_run.c sec_run.h structs.h
	gcc -c -ansi -Wall -pedantic  sec_run.c -o sec_run.o

init.o: init.c init.h structs.h
	gcc -c -ansi -Wall -pedantic init.c -o init.o

clean.o: clean.c structs.h clean.h
	gcc -c -ansi -Wall -pedantic clean.c -o clean.o

print_to_file.o: print_to_file.c print_to_file.h structs.h
	gcc -c -ansi -Wall -pedantic print_to_file.c -o print_to_file.o
	
check_funcs.o: check_funcs.c check_funcs.h structs.h
	gcc -c -ansi -Wall -pedantic check_funcs.c -o check_funcs.o

structs.o: structs.c structs.h
	gcc -c -ansi -Wall -pedantic structs.c -o structs.o

