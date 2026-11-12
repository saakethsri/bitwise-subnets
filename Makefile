subnet: subnet.c
	gcc -Wall -o subnet subnet.c

clean:
	rm -f subnet
