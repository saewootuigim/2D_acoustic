all: partition

partition: partition.c
	gcc -O3 -Wno-unused-result -I/home/crispy-roll/metis/include -L/home/crispy-roll/metis/lib $< -o $@ -lmetis