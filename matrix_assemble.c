#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "petscmat.h"
#include "elements.h"

#include "global_variables.h"
#include "assembly.h"

void assemble_matrix( Vec Mreg, Mat Kreg, Mat Ksrf, int nElem, int *elem2loc, int nDOFsrf, int *node2DOF, int *elem2node, double *node2xy ) {

	clock_t start = clock(), diff;
	int msec;

	int i0, i1;

	double m_elem[9], k_elem[81];

	int address_reg[9];

	printf("----- matrix assembly -----\n");

	// if( FWTR==FW )
	// {
		for( i0=0; i0<nElem; i0++ )
		{
			element_reg( i0, node2xy, elem2node, k_elem, m_elem );

			for( i1=0; i1<9; i1++ )
				address_reg[i1] = node2DOF[elem2node[i0*9+i1]];

			VecSetValues( Mreg, 9, address_reg, m_elem, ADD_VALUES );
			MatSetValues( Kreg, 9, address_reg, 9, address_reg, k_elem, ADD_VALUES );
		}

		VecAssemblyBegin( Mreg );
		VecAssemblyEnd  ( Mreg );
		MatAssemblyBegin( Kreg, MAT_FINAL_ASSEMBLY );
		MatAssemblyEnd  ( Kreg, MAT_FINAL_ASSEMBLY );

	// }
	// else if( FWTR==1 ) {
	// 	for (i0=0; i0<nElem; i0++) {

	// 		element_reg( i0, lambda, mu, rho, node2xy, elem2node, k_elem, m_eleMreg, m_elem );

	// 		for (i1=0; i1<2; i1++) {
	// 			for (i2=0; i2<9; i2++) {
	// 				address    [i1*9+i2] = node2DOF[elem2node[i0*9+i2]*2+i1];
	// 				address_srf[i1*9+i2] = -node2DOF[elem2node[i0*9+i2]*2+i1]-2;
	// 			}
	// 		}

	// 		VecSetValues( Mreg, 18, address, m_eleMreg, ADD_VALUES );
	// 		MatSetValues( Kreg, 18, address, 18, address, k_elem, ADD_VALUES );
	// 		if( elem2loc[i0]==0 ) { // for surface element
	// 			// MatSetValues( Ms, 18, address, 18, address_srf, m_elem, ADD_VALUES );
	// 			MatSetValues( Ksrf, 18, address, 18, address_srf, k_elem, ADD_VALUES );
	// 		}
	// 	}

	// 	VecAssemblyBegin( Mreg );
	// 	VecAssemblyEnd  ( Mreg );
	// 	MatAssemblyBegin( Kreg, MAT_FINAL_ASSEMBLY );
	// 	MatAssemblyEnd  ( Kreg, MAT_FINAL_ASSEMBLY );
	// 	// MatAssemblyBegin( Ms, MAT_FINAL_ASSEMBLY );
	// 	// MatAssemblyEnd  ( Ms, MAT_FINAL_ASSEMBLY );
	// 	MatAssemblyBegin( Ksrf, MAT_FINAL_ASSEMBLY );
	// 	MatAssemblyEnd  ( Ksrf, MAT_FINAL_ASSEMBLY );
	// }

    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
	printf(" elapse=%4i seconds %4i milliseconds\n", msec/1000, msec%1000);
}