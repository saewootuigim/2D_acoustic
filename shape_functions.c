#include "shape_functions.h"

void shape1d3( double x, double *N, double *dN ) {
	N[0] = .5*x*(x-1);
	N[1] = -(x+1)*(x-1);
	N[2] = .5*x*(x+1);
	dN[0] = x-.5;
	dN[1] = -2*x;
	dN[2] = x+.5;
}

void shape2d9( double x1, double x2, double *N, double (*dN)[2] ) {
	
	double N1[3], N2[3];
	double dN1[3], dN2[3];

	shape1d3( x1, N1, dN1 );
	shape1d3( x2, N2, dN2 );

	N[0] = N1[0]*N2[0]; dN[0][0] = dN1[0]*N2[0]; dN[0][1] = N1[0]*dN2[0];
	N[1] = N1[2]*N2[0]; dN[1][0] = dN1[2]*N2[0]; dN[1][1] = N1[2]*dN2[0];
	N[2] = N1[2]*N2[2]; dN[2][0] = dN1[2]*N2[2]; dN[2][1] = N1[2]*dN2[2];
	N[3] = N1[0]*N2[2]; dN[3][0] = dN1[0]*N2[2]; dN[3][1] = N1[0]*dN2[2];

	N[4] = N1[1]*N2[0]; dN[4][0] = dN1[1]*N2[0]; dN[4][1] = N1[1]*dN2[0];
	N[5] = N1[2]*N2[1]; dN[5][0] = dN1[2]*N2[1]; dN[5][1] = N1[2]*dN2[1];
	N[6] = N1[1]*N2[2]; dN[6][0] = dN1[1]*N2[2]; dN[6][1] = N1[1]*dN2[2];
	N[7] = N1[0]*N2[1]; dN[7][0] = dN1[0]*N2[1]; dN[7][1] = N1[0]*dN2[1];

	N[8] = N1[1]*N2[1]; dN[8][0] = dN1[1]*N2[1]; dN[8][1] = N1[1]*dN2[1];
}
