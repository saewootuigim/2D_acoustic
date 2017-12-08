#include <stdio.h>
#include <string.h>
#include "petscmat.h"
#include "petscviewer.h"

#include "treat_args.h"
#include "mesh.h"
#include "assembly.h"
#include "solvers.h"
#include "post_process.h"

void assemble_initial_condition( double x, double y, double h, int nNodeX, int nNodeY, int *node2GIx, int *node2GIy, int nNode, double *node2xy, int *node2DOF, Vec u_init )
{
	double x0=.5*x;
	double y0=.6*y;
	double r0=10.*h;

	int i0, i1;
	double r;

	double *val=(double*)malloc(nNodeX*nNodeY*sizeof(double));

	FILE *fid;

	printf("----- initial condition -----\n");
	printf("location: (%f,%f)\n",x0,y0);
	printf("  radius: %f\n",r0);

	for( i0=0; i0<nNode; i0++ )
	{
		r = sqrt((node2xy[i0*2]-x0)*(node2xy[i0*2]-x0)+(node2xy[i0*2+1]-y0)*(node2xy[i0*2+1]-y0));
		if( r<r0 )
		{
			VecSetValue( u_init, node2DOF[i0], cos(.5*M_PI/r0*r)*cos(.5*M_PI/r0*r), INSERT_VALUES );
			if( node2GIy[i0]>=0 && node2GIx[i0]>=0 )
				val[node2GIy[i0]*nNodeX+node2GIx[i0]] = cos(.5*M_PI/r0*r)*cos(.5*M_PI/r0*r);
			else
				val[node2GIy[i0]*nNodeX+node2GIx[i0]] = 0.;
		}
	}
	VecAssemblyBegin( u_init );
	VecAssemblyEnd( u_init );

	fid = fopen("initial_condition.txt","wt");
	for( i0=0; i0<nNodeY; i0++ )
	{
		for( i1=0; i1<nNodeX; i1++ )
			fprintf(fid,"%e ",val[i0*nNodeX+i1]);
		fprintf(fid,"\n");
	}
	fclose(fid);

	free(val);
	// exit(1);
}

int main( int argc, char *args[] ) {

	int nNode, nNodeX, nNodeY;
	int nElem;
	double x, y, h;
	double *node2xy;
	int *node2GIx, *node2GIy;
	int *elem2loc, *elem2node;
	int *DOF2ResIx, *DOF2ResIy;

	int *DOF2node;

	int *node2DOF;
	int loading_DOF, loading_node;
	int nDOFall, nDOFsrf, nDOFreg;

	int nTstep;
	double dt;

	double dist_max;

	Vec Mreg;
	Mat Ksrf, Kreg;
	Vec f;
	Vec u_init;

	PetscInitialize( &argc, &args, NULL, NULL );

	/* Treat command line arguments. */
	treat_arguments( argc, args );

	/* Create model. */
	mesh( &nNode, &nNodeX, &nNodeY, &nElem, &nDOFall, &nDOFsrf, &nDOFreg, &node2xy, &node2GIx, &node2GIy, &node2DOF, &elem2node, &elem2loc, &DOF2node, &x, &y, &h );

	/* Create PETSc objects. */
	VecCreateMPI( PETSC_COMM_WORLD, PETSC_DECIDE, nDOFreg, &Mreg );
	VecSetOption( Mreg, VEC_IGNORE_NEGATIVE_INDICES, PETSC_TRUE );

	VecDuplicate( Mreg, &f );
	VecDuplicate( Mreg, &u_init );

	MatCreateAIJ( PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, nDOFall, nDOFall, 72, NULL, 72, NULL, &Kreg );
	MatSetOption( Kreg, MAT_SYMMETRIC, PETSC_TRUE );
	MatSetOption( Kreg, MAT_IGNORE_ZERO_ENTRIES, PETSC_TRUE );

	MatCreateAIJ( PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, nDOFall, nDOFsrf, 72, NULL, 72, NULL, &Ksrf );
	MatSetOption( Ksrf, MAT_IGNORE_ZERO_ENTRIES, PETSC_TRUE );

	/* Create matrices and force vector. */
	assemble_matrix( Mreg, Kreg, Ksrf, nElem, elem2loc, nDOFsrf, node2DOF, elem2node, node2xy );
	assemble_body_force( x, y, nNode, node2xy, node2DOF, f, &loading_DOF, &loading_node, &dist_max );
	assemble_initial_condition( x, y, h, nNodeX, nNodeY, node2GIx, node2GIy, nNode, node2xy, node2DOF, u_init );

	/* Solve. */
	solver_RK4( Mreg, Kreg, Ksrf, f, u_init, nDOFall, nDOFsrf, node2xy, node2DOF, loading_DOF, h, dist_max, &nTstep, &dt );

	/* Rearrange the solution into 2D grid. */
	resolution_nodes( h, loading_node, node2xy, nDOFall, DOF2node, node2GIx, node2GIy, &DOF2ResIx, &DOF2ResIy );
	rearrange_for_plot( nDOFall, nDOFsrf, nNodeX, nNodeY, nTstep, node2GIx, node2GIy, DOF2node, DOF2ResIx, DOF2ResIy );

	/* Wrap up. */
	VecDestroy( &Mreg );
	MatDestroy( &Kreg );
	MatDestroy( &Ksrf );
	VecDestroy( &f );

	free( node2xy );
	free( elem2loc );
	free( elem2node );
	free( node2DOF );
	free( DOF2ResIx );
	free( DOF2ResIy );

	printf("----- end of program -----\n");

	PetscFinalize();

	return 0;
}