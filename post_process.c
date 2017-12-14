#include "post_process.h"
#include <math.h>

void resolution_nodes( double h, int loading_node, double node2xy[], int nDOFall, int DOF2node[], int node2GIx[], int node2GIy[], int **DOF2ResNodeX, int **DOF2ResNodeY )
{
	int i0;

	double xx, yy;

	double x = node2xy[2*loading_node];
	double y = node2xy[2*loading_node+1];

	*DOF2ResNodeX = (int*)malloc(nDOFall*sizeof(int));
	*DOF2ResNodeY = (int*)malloc(nDOFall*sizeof(int));

	// Visit DOFs and see if they have the same x or y.
	for( i0=0; i0<nDOFall; i0++ )
	{
		xx = node2xy[2*DOF2node[i0]];
		yy = node2xy[2*DOF2node[i0]+1];
		if( fabs(yy-y)<.1*h )
			(*DOF2ResNodeX)[i0] = DOF2node[i0];
		else
			(*DOF2ResNodeX)[i0] = -1;
		if( fabs(xx-x)<.1*h )
			(*DOF2ResNodeY)[i0] = DOF2node[i0];
		else
			(*DOF2ResNodeY)[i0] = -1;
	}
}

void rearrange_for_plot( int nDOFall, int nDOFsrf, int nNodeX, int nNodeY, int nTstep, int node2GIx[], int node2GIy[], int DOF2node[], int DOF2ResNodeX[], int DOF2ResNodeY[], double node2xy[] )
{
	const char* folderr; /* To check if the output folder exists. */
    struct stat sb;      /* and this one too */

	int i0, i1;
	int ix, iy;
	double *u_all = (double*)malloc(nDOFall*sizeof(double));
	double *u_grid = (double*)malloc(nNodeX*nNodeY*sizeof(double));
	double *u_res_x = (double*)malloc(nNodeX*sizeof(double));
	double *u_res_y = (double*)malloc(nNodeY*sizeof(double));

	FILE *fid1;
	FILE *fid2;
	FILE *fid3;
	FILE *fid_temp;

	char *filename = (char*)malloc(50*sizeof(char));

	printf("----- post processing -----\n");

	/* Check if the output directory exists. */
    
    //folderr = "C:\\Users\\SaMaN\\Desktop\\Ppln";
    folderr = "./output/frames";

    if( stat(folderr,&sb)!=0 || !S_ISDIR(sb.st_mode) )
    {
        printf("Output foler does not exist. Creating.\n");
        mkdir(folderr,0755);
    }

    /* Extract surface response. */
	if( cmd_arg.FWTR==FW )
	{
		printf("extracting surface response...\n");
		fid1 = fopen("output/u_all.dat","rb");
		fid3 = fopen("output/u_srf.dat","wb");
	    for( i0=0; i0<nTstep; i0++ )
	    {
	    	fread(u_all,sizeof(double),nDOFall,fid1);
			fwrite(u_all,sizeof(double),nDOFsrf,fid3);
	    }
		fclose(fid3);
		fclose(fid1);
	}	

	/* Extract response of horizontal and vertical line that pass the loading node. */
	printf("extracting resolution information...\n");

	fid1 = fopen("output/u_all.dat","rb");
	if( cmd_arg.FWTR==FW )
	{
		sprintf(filename,"./output/u_res_x_fw.dat");
		fid2 = fopen(filename,"wb");
		sprintf(filename,"./output/u_res_y_fw.dat");
		fid3 = fopen(filename,"wb");
	}
	else if( cmd_arg.FWTR==TR )
	{
		sprintf(filename,"./output/u_res_x_tr.dat");
		fid2 = fopen(filename,"wb");
		sprintf(filename,"./output/u_res_y_tr.dat");
		fid3 = fopen(filename,"wb");
	}
	else if( cmd_arg.FWTR==TRFR )
	{
		sprintf(filename,"./output/u_res_x_trfr.dat");
		fid2 = fopen(filename,"wb");
		sprintf(filename,"./output/u_res_y_trfr.dat");
		fid3 = fopen(filename,"wb");
	}
	else if( cmd_arg.FWTR==TRFI )
	{
		sprintf(filename,"./output/u_res_x_trfi.dat");
		fid2 = fopen(filename,"wb");
		sprintf(filename,"./output/u_res_y_trfi.dat");
		fid3 = fopen(filename,"wb");
	}
	else if( cmd_arg.FWTR==TRFN )
	{
		sprintf(filename,"./output/u_res_x_trfn.dat");
		fid2 = fopen(filename,"wb");
		sprintf(filename,"./output/u_res_y_trfn.dat");
		fid3 = fopen(filename,"wb");
	}

	for( i0=0; i0<nTstep; i0++ )
	{
		fread(u_all,sizeof(double),nDOFall,fid1);

		for( i1=0; i1<nDOFall; i1++ )
		{
			if( DOF2ResNodeX[i1]!=-1 )
			{
				if( node2GIx[DOF2ResNodeX[i1]]>=0 && node2GIx[DOF2ResNodeX[i1]]<nNodeX )
					u_res_x[node2GIx[DOF2ResNodeX[i1]]] = u_all[i1];
				else
					printf("DOF2ResNodeX[%i]=%i, node2GIx[%i]=%i, nNodeX-2=%i, node2xy=%.3f %.3f\n",
						i1,DOF2ResNodeX[i1],DOF2ResNodeX[i1],node2GIx[DOF2ResNodeX[i1]],nNodeX-2,node2xy[2*DOF2ResNodeX[i1]],node2xy[2*DOF2ResNodeX[i1]+1]);
			}
			if( DOF2ResNodeY[i1]!=-1 )
			{
				if( node2GIy[DOF2ResNodeY[i1]]>=0 && node2GIy[DOF2ResNodeY[i1]]<nNodeY )
					u_res_y[node2GIy[DOF2ResNodeY[i1]]] = u_all[i1];
				else
					printf("DOF2ResNodeY[%i]=%i, node2GIy[%i]=%i, nNodeY-1=%i, node2xy=%.3f %.3f\n",
						i1,DOF2ResNodeY[i1],DOF2ResNodeY[i1],node2GIy[DOF2ResNodeY[i1]],nNodeY-1,node2xy[2*DOF2ResNodeY[i1]],node2xy[2*DOF2ResNodeY[i1]+1]);
			}
		}
		fwrite(u_res_x,sizeof(double),nNodeX,fid2);
		fwrite(u_res_y,sizeof(double),nNodeY,fid3);
	}
	
	printf("fid1 closed %i\n",fclose(fid1));
	printf("fid2 closed %i\n",fclose(fid2));
	printf("fid3 closed %i\n",fclose(fid3));

    /* Rearrange for snapshots. */
	printf("rearranging response for snapshots...\n");
	fid1 = fopen("output/u_all.dat","rb");

	for( i0=0; i0<nTstep; i0+=cmd_arg.PS )
	{
		// printf("%4i/%4i\n",i0,nTstep);
		fseek(fid1,sizeof(double)*nDOFall*i0,SEEK_SET);
		fread(u_all,sizeof(double),nDOFall,fid1);

		// for plotting snapshots
		for( i1=0; i1<nDOFall; i1++ )
		{
			ix = node2GIx[DOF2node[i1]];
			iy = node2GIy[DOF2node[i1]];

			if( ix==-1 || iy==-1 )
				u_grid[nNodeX*iy+ix] = 0.;
			else
				u_grid[nNodeX*iy+ix] = u_all[i1];
		}
		sprintf(filename,"./output/frames/step%04i.dat",i0);
		fid2 = fopen(filename,"wb");
		fwrite(u_grid,sizeof(double),nNodeX*nNodeY,fid2);
		fclose(fid2);
	}

	fclose(fid1);

	free( u_all );
	free( u_grid );
	free( filename );

	free( u_res_x );
	free( u_res_y );
}