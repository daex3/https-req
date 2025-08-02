CFLAGS = -o out -lssl
MAIN = main.c
CC = clang

all: rel r

dbg: d
	gdb ./out

bin: rel
	doas cp -i out /usr/bin/$(name)

rel:
	$(CC) $(CFLAGS) -O3 $(MAIN)
d:
	$(CC) $(CFLAGS) -g $(MAIN)

mac:
	$(CC) -dM -E $(MAIN)

r:
	./out

tra: rel
	strace ./out -m 30000 -o arch.html archlinux.org > dump 2>&1
	vim -c '%s/mprot/mprot/g' -c '1,.d' -c 'w' dump
