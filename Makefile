all: proj1 proj2 proj3

proj1: proj1.c
	gcc proj1.c -o proj1 -Wall -Wextra -Werror -pedantic -std=c99

proj2: proj2.c
	gcc proj2.c -o proj2 -lm -Wall -Wextra -Werror -pedantic -std=c99

proj3: proj3.c
	gcc proj3.c -o proj3 -lm -Wall -Wextra -Werror -pedantic -std=c99

clean:
	rm proj1 proj2 proj3 > /dev/null


