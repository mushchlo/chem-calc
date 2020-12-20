all: stdc

stdc:
	gcc -o chemcalc -O2 chemcalc.c
	chmod 755 chemcalc
