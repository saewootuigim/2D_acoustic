#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "petscmat.h"

#include "global_variables.h"
#include "solvers.h"

double loading_fw_time_signal( double t );

double dabsmax( int nsize, const double *arr ) {
	
	int i0;
	double maximum;

	maximum = 0.;

	for( i0=0; i0<nsize; i0++ ) {
		if( maximum<fabs(arr[i0]) ) maximum = arr[i0];
	}

	return maximum;
}

void solver_RK4( int FWTR, Vec Mreg, Mat Kreg, Mat Ksrf, Vec f, 
	int nDOFall, int nDOFsrf, double *node2xy, int *node2DOF, int DOFload, 
	double h, double dist_max, int *nTstep, double *dt )
{
	clock_t start = clock(), diff, msec;

	Vec k11, k12, k21, k22, k31, k32, k41, k42;
	Vec x1n, x2n;
	Vec temp;

	double maxval, maxval_new;

	int i0, i1;
	double offset;

	// print load/target response
	PetscInt DOF_load[1];
	PetscScalar disp_load[1];

	// print entire domain response
	const PetscScalar *u_all;
	PetscInt ix[nDOFsrf];
	double *u_srf_0 = (double*)malloc(nDOFsrf*sizeof(double)); // for f^n
	double *u_srf_h = (double*)malloc(nDOFsrf*sizeof(double)); // for f^(n+1/2)
	double *u_srf_1 = (double*)malloc(nDOFsrf*sizeof(double)); // for f^(n+1)
	Vec u_srf_0_PETSc;
	Vec u_srf_h_PETSc;
	Vec u_srf_1_PETSc;

	// files
	FILE *fid_u_all, *fid_u_srf, *fid_u_trg, *fid_sgnl;
	FILE *fid;

	double temp_double[1], temp_dt, temp_t;

	double t, T;
	int time_step_update;

	printf("----- solver RK4 -----\n");

	DOF_load[0] = DOFload;

	*dt = h/sqrt(2.); // c=1
	offset = (*dt)*10;
	*nTstep = dist_max/(*dt) + offset/(*dt) + 11;
	// T = (*dt)*(*nTstep);
	// *dt = (*dt<.005) ? *dt : .005;
	// *nTstep = (int)(T/(*dt));

	printf(" nTstep=%i\n",*nTstep);
	printf("     dt=%f\n",*dt);

	VecDuplicate( Mreg, &k11 ); VecDuplicate( Mreg, &k12 );
	VecDuplicate( Mreg, &k21 ); VecDuplicate( Mreg, &k22 );
	VecDuplicate( Mreg, &k31 ); VecDuplicate( Mreg, &k32 );
	VecDuplicate( Mreg, &k41 ); VecDuplicate( Mreg, &k42 );
	VecDuplicate( Mreg, &x1n ); VecDuplicate( Mreg, &x2n );
	VecDuplicate( Mreg, &temp );

	/* Vec to read surface response of forward step. */
	// if( FWTR==TR ) {
	// 	VecCreateMPI( PETSC_COMM_WORLD, PETSC_DECIDE, nDOFsrf, &u_srf );
	// 	ux_srf_ftr = (double*)malloc(nDOFsrf/2*sizeof(double));
	// 	uy_srf_ftr = (double*)malloc(nDOFsrf/2*sizeof(double));
	// }

	VecReciprocal( Mreg );
	MatDiagonalScale( Kreg, Mreg, NULL );
	MatScale( Kreg, -1. );
	if( FWTR==FW ){
		VecPointwiseMult( f, f, Mreg );
	} else if( FWTR==TR ) {
		// MatScale( Ksrf, -1. );
	}

	VecSet( x1n, 0. );
	VecSet( x2n, 0. );
	if( FWTR==TR )
	{
		VecSet( f, 0. );
		VecDuplicate( f, &u_srf_0_PETSc );
		VecDuplicate( f, &u_srf_h_PETSc );
		VecDuplicate( f, &u_srf_1_PETSc );
		for( i0=0; i0<nDOFsrf; i0++ )
			ix[i0] = i0;
	}
	maxval = 0.; maxval_new = 0.;

	/* Open file for input and output. */
	fid_u_all = fopen("output/u_all.dat","wb");
	if( FWTR==FW )
		fid_sgnl = fopen("output/sgnl.dat","wb");
	else if( FWTR==TR )
		fid_u_srf = fopen("output/u_srf.dat","rb");
	fid_u_trg = fopen("output/u_trg.dat","wb");

	/* Start time stepping. */
	// i0 = 0;
	// t = 0.;
	// T = (*dt)*(*nTstep);
	// while( t<T )
	for( i0=0; i0<(*nTstep); i0++ )
	{
		/* loading */
		VecSet( k12, 0. );
		VecSet( k22, 0. );
		VecSet( k32, 0. );
		VecSet( k42, 0. );
		if( FWTR==FW )
		{
			temp_double[0] = loading_fw_time_signal(i0*(*dt)-offset);
			fwrite( &temp_double, sizeof(double), 1, fid_sgnl );
			VecAXPY( k12, loading_fw_time_signal((i0   )*(*dt)-offset), f );
			VecAXPY( k22, loading_fw_time_signal((i0+.5)*(*dt)-offset), f );
			VecAXPY( k32, loading_fw_time_signal((i0+.5)*(*dt)-offset), f );
			VecAXPY( k42, loading_fw_time_signal((i0+1.)*(*dt)-offset), f );
		}
		else if( FWTR==TR )
		{
			if( i0!=(*nTstep)-1 )
			{
				fseek( fid_u_srf, -(i0+2)*nDOFsrf*sizeof(double), SEEK_END );
				fread( u_srf_1, sizeof(double), nDOFsrf, fid_u_srf );
				fread( u_srf_0, sizeof(double), nDOFsrf, fid_u_srf );
				for( i1=0; i1<nDOFsrf; i1++ )
					u_srf_h[i1] = .5*(u_srf_0[i1]+u_srf_1[i1]);
			}
			else
			{
				fseek( fid_u_srf, -(*nTstep)*nDOFsrf*sizeof(double), SEEK_END );
				fread( u_srf_0, sizeof(double), nDOFsrf, fid_u_srf );
				for( i1=0; i1<nDOFsrf; i1++ )
				{
					u_srf_1[i1] = 0.;
					u_srf_h[i1] = .5*u_srf_0[i1];
				}
			}
			VecSetValues( u_srf_0_PETSc, nDOFsrf, ix, u_srf_0, INSERT_VALUES );
			VecSetValues( u_srf_h_PETSc, nDOFsrf, ix, u_srf_h, INSERT_VALUES );
			VecSetValues( u_srf_1_PETSc, nDOFsrf, ix, u_srf_1, INSERT_VALUES );
			VecAssemblyBegin( u_srf_0_PETSc );
			VecAssemblyEnd( u_srf_0_PETSc );
			VecAssemblyBegin( u_srf_h_PETSc );
			VecAssemblyEnd( u_srf_h_PETSc );
			VecAssemblyBegin( u_srf_1_PETSc );
			VecAssemblyEnd( u_srf_1_PETSc );

			VecPointwiseMult( u_srf_0_PETSc, u_srf_0_PETSc, Mreg );
			VecPointwiseMult( u_srf_h_PETSc, u_srf_h_PETSc, Mreg );
			VecPointwiseMult( u_srf_1_PETSc, u_srf_1_PETSc, Mreg );

			VecAXPY( k12, 1., u_srf_0_PETSc );
			VecAXPY( k22, 1., u_srf_h_PETSc );
			VecAXPY( k32, 1., u_srf_h_PETSc );
			VecAXPY( k42, 1., u_srf_1_PETSc );
		}

		/* Runge-Kutta 4 */
		VecCopy( x2n, k11 );
		MatMultAdd( Kreg, x1n, k12, k12 );

		VecWAXPY( k21, .5*(*dt), k12, x2n );
		VecWAXPY( temp, .5*(*dt), k11, x1n );
		MatMultAdd( Kreg, temp, k22, k22 );

		VecWAXPY( k31, .5*(*dt), k22, x2n );
		VecWAXPY( temp, .5*(*dt), k21, x1n );
		MatMultAdd( Kreg, temp, k32, k32 );

		VecWAXPY( k41, (*dt), k32, x2n );
		VecWAXPY( temp, (*dt), k31, x1n );
		MatMultAdd( Kreg, temp, k42, k42 );

		/* Update solution vectors. */
		VecSet( temp, 0. );
		VecAXPY( temp, 1., k11 );
		VecAXPY( temp, 2., k21 );
		VecAXPY( temp, 2., k31 );
		VecAXPY( temp, 1., k41 );
		VecAXPY( x1n, (*dt)/6., temp );

		VecSet( temp, 0. );
		VecAXPY( temp, 1., k12 );
		VecAXPY( temp, 2., k22 );
		VecAXPY( temp, 2., k32 );
		VecAXPY( temp, 1., k42 );
		VecAXPY( x2n, (*dt)/6., temp );

		/* Print surface response. */
		VecGetArrayRead( x1n, &u_all );

		// Write response.
		fwrite( u_all, sizeof(double), nDOFall, fid_u_all );
		
		// Update maximum value.
		maxval_new = dabsmax( nDOFall, u_all );
		if( maxval<maxval_new ) maxval = maxval_new;
		
		VecRestoreArrayRead( x1n, &u_all );

		// timing
		diff = clock() - start;
		msec = diff * 1000 / CLOCKS_PER_SEC;
		// printf(" step %i/%i elapse=%4i seconds %4i milliseconds\n", i0+1,*nTstep, msec/1000, msec%1000);
		start = clock();

		/* Print response on the loading nodes. */
		VecGetValues( x1n, 1, DOF_load, disp_load );
		fwrite( disp_load, sizeof(PetscScalar), 1, fid_u_trg );
		if( i0%10==0 )
			printf("      t=%8.5f, u_trg=%11.4e\n",i0*(*dt),disp_load[0]);
	}

	// if( FWTR==TR ) {
	// 	free( ux_srf_ftr );
	// 	free( uy_srf_ftr );
	// }

	free( u_srf_0 );
	free( u_srf_h );
	free( u_srf_1 );

	fclose( fid_u_all );
	if( FWTR==FW ) fclose( fid_sgnl );
	else if( FWTR==TR ) fclose( fid_u_srf );
	fclose( fid_u_trg );

	fid = fopen("output/simulation_info.txt","wt");
	fprintf(fid,"nTstep,%i\n",*nTstep);
	fprintf(fid,"dt,%f\n",*dt);
	fprintf(fid,"maxval,%e\n",maxval);
	fclose(fid);
}