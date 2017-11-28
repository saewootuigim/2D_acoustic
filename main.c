#include <stdio.h>
#include <string.h>
#include "petscmat.h"
#include "petscviewer.h"

#include "global_variables.h"
#include "mesh.h"
#include "assembly.h"
#include "solvers.h"
#include "post_process.h"

int main( int argc, char *args[] ) {

	int FWTR;
	int nNode, nNodeX, nNodeY;
	int nElem;
	double x, y, h;
	double *node2xy;
	int *node2GIx, *node2GIy;
	int *elem2loc, *elem2node;

	int *DOF2node;

	int *node2DOF;
	int DOFload;
	int nDOFall, nDOFsrf, nDOFreg;

	int nTstep;
	double dt;

	double dist_max;

	Vec Mreg;
	Mat Ksrf, Kreg;
	Vec f;

	PetscInitialize( &argc, &args, NULL, NULL );

	/* Is it FW or TR? */
	if( argc!=2 ) {
		printf(" input argument must be provided. (FW/TR)\n");
		PetscFinalize();
		return 0;
	} else if( !strcmp(args[1],"FW") ) {
		FWTR = FW;
		printf(" forward\n");
	} else if( !strcmp(args[1],"TR") ) {
		FWTR = TR;
		printf(" time reversal\n");
	} else {
		printf(" input argument must be provided. (FW/TR)\n");
		PetscFinalize();
		return 0;
	}

	/* Create model. */
	mesh( &nNode, &nNodeX, &nNodeY, &nElem, &nDOFall, &nDOFsrf, &nDOFreg, &node2xy, &node2GIx, &node2GIy, &node2DOF, &elem2node, &elem2loc, &DOF2node, &x, &y, &h );

	/* Create PETSc objects. */
	VecCreateMPI( PETSC_COMM_WORLD, PETSC_DECIDE, nDOFreg, &Mreg );
	VecSetOption( Mreg, VEC_IGNORE_NEGATIVE_INDICES, PETSC_TRUE );

	VecDuplicate( Mreg, &f );

	MatCreateAIJ( PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, nDOFall, nDOFall, 72, NULL, 72, NULL, &Kreg );
	MatSetOption( Kreg, MAT_SYMMETRIC, PETSC_TRUE );
	MatSetOption( Kreg, MAT_IGNORE_ZERO_ENTRIES, PETSC_TRUE );

	MatCreateAIJ( PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, nDOFall, nDOFsrf, 72, NULL, 72, NULL, &Ksrf );
	MatSetOption( Ksrf, MAT_IGNORE_ZERO_ENTRIES, PETSC_TRUE );

	/* Create matrices and force vector. */
	assemble_matrix( FWTR, Mreg, Kreg, Ksrf, nElem, elem2loc, nDOFsrf, node2DOF, elem2node, node2xy );
	assemble_body_force( x, y, nNode, node2xy, node2DOF, f, &DOFload, &dist_max );

	/* Solve. */
	solver_RK4( FWTR, Mreg, Kreg, Ksrf, f, nDOFall, nDOFsrf, node2xy, node2DOF, DOFload, h, dist_max, &nTstep, &dt );

	/* Rearrange the solution into 2D grid. */
	rearrange_for_plot( FWTR, nDOFall, nDOFsrf, nNodeX, nNodeY, nTstep, node2GIx, node2GIy, DOF2node );

	/* Wrap up. */
	VecDestroy( &Mreg );
	MatDestroy( &Kreg );
	MatDestroy( &Ksrf );
	VecDestroy( &f );

	free( node2xy );
	free( elem2loc );
	free( elem2node );
	free( node2DOF );

	printf("----- end of program -----\n");

	PetscFinalize();

	return 0;
}