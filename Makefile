CC=gcc
CFLAGS= -std=gnu99
FILES= src/ballistic_table.c lib/pbPlots.c lib/supportLib.c
OUT=out/

all:
	$(CC) $(CFLAGS) $(FILES) -o $(OUT)ballistic_table -lm

run:
	$(OUT)ballistic_table > $(OUT)ballistic_table.txt
