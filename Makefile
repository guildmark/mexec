mexec: mexec.c
	gcc -g -std=gnu11 -Werror -Wall -Wextra -Wpedantic	-Wmissing-declarations -Wmissing-prototypes -Wold-style-definition mexec.c -o mexec
	gcc -g -std=gnu11 -Wall   -c -o mexec.o mexec.c