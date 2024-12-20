CC = g++
OUT = out

CCFLAGS = -std=c++20 -I../ -I./ -Wextra -Wthread-safety -fsanitize=undefined

p2: p2_sol.cpp p2_sol_bruteforce.cpp
	$(CC) $(CCFLAGS) $(LDFLAGS) -DTEST -o p2_sol_test p2_sol.cpp && chmod +x p2_sol_test
	$(CC) $(CCFLAGS) $(LDFLAGS) -DSAMPLE -o p2_sol_sample p2_sol.cpp && chmod +x p2_sol_sample
	$(CC) $(CCFLAGS) $(LDFLAGS) -DTEST -o p2_sol_test p2_sol.cpp && chmod +x p2_sol_test
	$(CC) $(CCFLAGS) $(LDFLAGS) -DSAMPLE -o p2_sol_sample p2_sol.cpp && chmod +x p2_sol_sample

p1: p1_sol.cpp
	$(CC) $(CCFLAGS) $(LDFLAGS) -DTEST -o p1_sol_test p1_sol.cpp && chmod +x p1_sol_test
	$(CC) $(CCFLAGS) $(LDFLAGS) -DSAMPLE -o p1_sol_sample p1_sol.cpp && chmod +x p1_sol_sample

clean:
	rm p1_sol
	rm p2_sol
