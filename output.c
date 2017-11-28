#include <stdio.h>
#include "petscsys.h"

void print_result( int nDOF, int *DOF2node, int *DOF2dir, int iTstep, double t, const PetscScalar *u ) {

	int i0;
	FILE *fidx, *fidy;
	char filenamex[100], filenamey[100];

	sprintf( filenamex, "output/step%04ix.txt", iTstep );
	sprintf( filenamey, "output/step%04iy.txt", iTstep );

	fidx = fopen( filenamex, "wt" );
	fidy = fopen( filenamey, "wt" );

	fprintf( fidx, "%e\n", t );
	fprintf( fidy, "%e\n", t );

	for( i0=0; i0<nDOF; i0++ ) {
		if( DOF2dir[i0]==1 ) {
			fprintf( fidx, "%e\n", u[DOF2node[i0]] );
		} else {
			fprintf( fidy, "%e\n", u[DOF2node[i0]] );
		}
	}

	fclose( fidx );
	fclose( fidy );
}