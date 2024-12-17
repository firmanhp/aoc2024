CC = g++
OUT = out

CCFLAGS = -std=c++20 -I../ -I./ -Wextra -fsanitize=undefined

p2: p2_sol.cpp p2_sol_handwrite.cpp p2_sol_bruteforce.cpp
	$(CC) $(CCFLAGS) $(LDFLAGS) -o p2_sol p2_sol.cpp && chmod +x p2_sol
	$(CC) $(CCFLAGS) $(LDFLAGS) -o p2_sol_handwrite p2_sol_handwrite.cpp && chmod +x p2_sol_handwrite
	$(CC) $(CCFLAGS) $(LDFLAGS) -o p2_sol_bruteforce p2_sol_bruteforce.cpp && chmod +x p2_sol_bruteforce

p1: p1_sol.cpp
	$(CC) $(CCFLAGS) $(LDFLAGS) -o p1_sol p1_sol.cpp && chmod +x p1_sol

clean:
	rm p1_sol
	rm p2_sol
