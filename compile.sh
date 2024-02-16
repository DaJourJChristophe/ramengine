#!/bin/bash

set -e

/usr/bin/gcc -c -Iinclude -o main.o main.c

/usr/bin/gcc -Llibexec -o bin/main main.o -lpthread -lthpool -lturnpike -lhyperfunnel -ljemalloc

rm -rf *.o
