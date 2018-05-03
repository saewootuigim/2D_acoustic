all: partition mesh

FLAG = -I../include

partition: partition.c
	gcc -O3 -Wno-unused-result $(FLAG) -I/home/crispy-roll/metis/include -L/home/crispy-roll/metis/lib $< -o $@ -lmetis

mesh: mesh.c
	gcc -O3 -Wno-unused-result $(FLAG) $< -o $@
