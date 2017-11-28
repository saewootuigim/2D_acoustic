#include <stdio.h>
#include <math.h>
#include "petscvec.h"

void assemble_body_force( double dim_x, double dim_y, int nNode, double *node2xy, int *node2DOF, Vec f, int *DOFload, double *dist_max ) {

	int i0;
	double x, y, x0, y0, r, dist_min;

	int node_load;

	printf("----- force vector assembly -----\n");

	x0 = .5*dim_x;
	y0 = .3*dim_y;
	dist_min = sqrt( dim_x*dim_x + dim_y*dim_y );
	*dist_max = ( sqrt(x0*x0+(dim_y-y0)*(dim_y-y0)) > sqrt((dim_x-x0)*(dim_x-x0)+(dim_y-y0)*(dim_y-y0)) )
		? sqrt(x0*x0+(dim_y-y0)*(dim_y-y0)) : sqrt((dim_x-x0)*(dim_x-x0)+(dim_y-y0)*(dim_y-y0));

	for( i0=0; i0<nNode; i0++ ) {
		x = node2xy[2*i0  ] - x0;
		y = node2xy[2*i0+1] - y0;
		r = sqrt( x*x + y*y );
		if( r<dist_min ) {
			dist_min = r;
			node_load = i0;
		}
	}

	*DOFload = node2DOF[node_load];
	VecSet( f, 0. );
	VecSetValue( f, *DOFload, 1., INSERT_VALUES );
	VecAssemblyBegin( f );
	VecAssemblyEnd  ( f );

	printf(" loading node=%i, loading DOF=%i, coordinate=%f %f\n",node_load,*DOFload,node2xy[2*node_load],node2xy[2*node_load+1]);
}

double loading_fw_time_signal( double t )
{
	/* derivative of Gaussian */
	return -2.*(100.*t)*exp(-(100.*t)*(100.*t))*1e5;
}

