# Jovan Rayhaga
# makefile for rayhaga_client

CC=gcc
CFLAGS=-g -Wall

all: rayhaga_client

rayhaga_client: rayhaga_client.c
	$(CC) $(CFLAGS) -o rayhaga_client rayhaga_client.c

clean:
	rm rayhaga_client