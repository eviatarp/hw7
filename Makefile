# Defined veriables CC, CCLINK, CFLAGS, and OBJS
CC=gcc # Compiler
CCLINK=$(CC) # Read variable CC using $(CC)
CFLAGS=-g -Wall -std=c99 # Flag rules
OBJS=grades.o 

# creating library
libgrades.so: grades.o
	$(CCLINK) -shared grades.o -o libgrades.so -l linked-list -L.
	
# grades is dependent on linked-list
grades.o: grades.c grades.h linked-list.h
	$(CC) $(CFLAGS) -c -fpic grades.c
	
# Gets rid of objects and executable files
clean:
	rm -rf grades.o libgrades.so
