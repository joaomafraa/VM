CC = cc
CFLAGS = -Wall -Wextra -std=c11 -pthread

vm: vm.c
	$(CC) $(CFLAGS) vm.c -o vm

clean:
	rm -f vm

.PHONY: clean