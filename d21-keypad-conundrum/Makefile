CC = g++
OUT = out

CCFLAGS = -std=c++20 -I../ -I./ -Wextra -Wthread-safety -fsanitize=undefined

p2: sequences.o p2_sol.cpp
	$(CC) $(CCFLAGS) -c -DTEST -o p2_sol_test.o p2_sol.cpp
	$(CC) $(CCFLAGS) -c -DSAMPLE -o p2_sol_sample.o p2_sol.cpp
	$(CC) $(CCFLAGS) sequences.o p2_sol_test.o -o p2_sol_test
	$(CC) $(CCFLAGS) sequences.o p2_sol_sample.o -o p2_sol_sample
	chmod +x p2_sol_test
	chmod +x p2_sol_sample

p1: sequences.o p1_sol.cpp
	$(CC) $(CCFLAGS) -c -DTEST -o p1_sol_test.o p1_sol.cpp
	$(CC) $(CCFLAGS) -c -DSAMPLE -o p1_sol_sample.o p1_sol.cpp
	$(CC) $(CCFLAGS) sequences.o p1_sol_test.o -o p1_sol_test
	$(CC) $(CCFLAGS) sequences.o p1_sol_sample.o -o p1_sol_sample
	chmod +x p1_sol_test
	chmod +x p1_sol_sample

sequences.o: sequences.cpp
	$(CC) $(CCFLAGS) -c -o sequences.o sequences.cpp


clean:
	rm p1_sol
	rm p2_sol
	rm sequences.o