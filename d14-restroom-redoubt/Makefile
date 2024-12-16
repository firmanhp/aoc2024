CC = g++
OUT = out

CCFLAGS = -std=c++20 -I./ -Wextra -fsanitize=integer

p2: p2_sol.cpp
	$(CC) $(CCFLAGS) $(LDFLAGS) -o p2_sol p2_sol.cpp && chmod +x p2_sol

clean:
	rm p2_sol
