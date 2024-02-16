#!/bin/bash

set -e

/usr/bin/gcc -c -Iinclude -o src/broker.o src/broker.c
/usr/bin/gcc -c -Iinclude -o src/main.o src/main.c

/usr/bin/gcc -Llibexec -o bin/main src/broker.o src/main.o -lpthread -lthpool -lturnpike -lhyperfunnel -ljemalloc

rm -rf src/*.o
