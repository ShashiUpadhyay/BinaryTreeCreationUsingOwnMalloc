CC=gcc
CXX=g++
#CC=clang
#CXX=clang++
DEBUG=\
 -DCS550_ASSERT \
 -DCS550_DBG_PRINT \

.PHONY: a.out
a.out:
	$(CC) -O $(DEBUG) -Wall -Wextra test.c malloc.c utility.c -D_XOPEN_SOURCE=500 -std=c99
