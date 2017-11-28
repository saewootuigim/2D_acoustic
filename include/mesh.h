#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "global_variables.h"

void mesh(
	int* nNode,
	int *nNodeX,
	int *nNodeY,
	int* nElem,
	int* nDOFall,
	int* nDOFsrf,
	int *nDOFreg,
	double** node2xy,
	int** node2GIx,
	int** node2GIy,
	int** node2DOF,
	int** elem2node,
	int** elem2loc,
	int** DOF2node,
	double *x,
	double *y,
	double *h
);