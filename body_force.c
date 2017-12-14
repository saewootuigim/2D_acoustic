#include <stdio.h>
#include <math.h>
#include "petscvec.h"

void assemble_body_force( double dim_x, double dim_y, int nNode, double *node2xy, int *node2DOF, Vec f, int *loading_DOF, int *loading_node, double *dist_max ) {

	int i0;
	double x, y, x0, y0, r, dist_min;

	printf("----- force vector assembly -----\n");

	x0 = .5*dim_x;
	y0 = .8*dim_y;
	dist_min = sqrt( dim_x*dim_x + dim_y*dim_y );
	*dist_max = ( sqrt(x0*x0+(dim_y-y0)*(dim_y-y0)) > sqrt((dim_x-x0)*(dim_x-x0)+(dim_y-y0)*(dim_y-y0)) )
		? sqrt(x0*x0+(dim_y-y0)*(dim_y-y0)) : sqrt((dim_x-x0)*(dim_x-x0)+(dim_y-y0)*(dim_y-y0));

	for( i0=0; i0<nNode; i0++ ) {
		x = node2xy[2*i0  ] - x0;
		y = node2xy[2*i0+1] - y0;
		r = sqrt( x*x + y*y );
		if( r<dist_min ) {
			dist_min = r;
			*loading_node = i0;
		}
	}

	*loading_DOF = node2DOF[*loading_node];
	VecSet( f, 0. );
	VecSetValue( f, *loading_DOF, 1., INSERT_VALUES );
	VecAssemblyBegin( f );
	VecAssemblyEnd  ( f );

	printf(" loading node=%i, loading DOF=%i, coordinate=%f %f\n",*loading_node,*loading_DOF,node2xy[2*(*loading_node)],node2xy[2*(*loading_node)+1]);
}

double loading_fw_time_signal( double t )
{
	/* derivative of Gaussian */
	return -2.*(100.*t)*exp(-(100.*t)*(100.*t))*1e5;
}

