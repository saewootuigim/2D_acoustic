#include <stdio.h>
#include "shape_functions.h"

#include "elements.h"

/* Gauss-Legendre-Lobatto
   N points are enought to integrate polynomial of 2N-3 degree */
const double xi[]={-1., 0., 1.};
const double wi[]={1./3., 4./3., 1./3.};


void element_reg( int elem, double *node2xy, int *elem2node, double *k_elem, double *m_elem )
{

	int i0, i1, i2, i3, i4;
	double XT[2][9], Jacobi_mat[2][2], Jacobi_det, Jacobi_inv[2][2];
	double N[9], dN[9][2], dNi[9][2];

	/* Initialize. */
	for( i0=0; i0<9; i0++ )
		m_elem[i0] = 0.;

	for( i0=0; i0<81; i0++ )
		k_elem[i0] = 0.;

	/* coordinate transpose */
	for( i0=0; i0<2; i0++ )
		for( i1=0; i1<9; i1++ )
			XT[i0][i1] = node2xy[elem2node[elem*9+i1]*2+i0];

	/* quadrature */
	for( i0=0; i0<3; i0++ )
	{
		for( i1=0; i1<3; i1++ )
		{

			shape2d9( xi[i0], xi[i1], N, dN );

			for( i2=0; i2<2; i2++ ) {
				for( i3=0; i3<2; i3++ ) {
					Jacobi_mat[i2][i3] = .0;
					for( i4=0; i4<9; i4++ ){
						Jacobi_mat[i2][i3] += XT[i2][i4]*dN[i4][i3];
					}
				}
			}

			Jacobi_det = Jacobi_mat[0][0]*Jacobi_mat[1][1] - Jacobi_mat[0][1]*Jacobi_mat[1][0];

			Jacobi_inv[0][0] =  Jacobi_mat[1][1]/Jacobi_det;
			Jacobi_inv[1][1] =  Jacobi_mat[0][0]/Jacobi_det;
			Jacobi_inv[0][1] = -Jacobi_mat[0][1]/Jacobi_det;
			Jacobi_inv[1][0] = -Jacobi_mat[1][0]/Jacobi_det;

			for( i2=0; i2<9; i2++ ) {
				for( i3=0; i3<2; i3++ ) {
					dNi[i2][i3] = .0;
					for( i4=0; i4<2; i4++ ){
						dNi[i2][i3] += dN[i2][i4]*Jacobi_inv[i4][i3];
					}
				}
			}

			for( i2=0; i2<9; i2++ )
			{
				m_elem[i2] += (N[i2]*N[i2])*wi[i0]*wi[i1]*Jacobi_det;
                for( i3=0; i3<9; i3++ )
                	k_elem[i2*9+i3] += (dNi[i2][0]*dNi[i3][0] + dNi[i2][1]*dNi[i3][1])*wi[i0]*wi[i1]*Jacobi_det;
			}
		}
	}
}
