CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

vm: vm.c
	$(CC) $(CFLAGS) vm.c -o vm

clean:
	rm -f vm

.PHONY: clean