#include "post_process.h"

void rearrange_for_plot( int FWTR, int nDOFall, int nDOFsrf, int nNodeX, int nNodeY, int nTstep, int node2GIx[], int node2GIy[], int DOF2node[] )
{
	const char* folderr; /* To check if the output folder exists. */
    struct stat sb;      /* and this one too */

	int i0, i1;
	int ix, iy;
	double *u_all = (double*)malloc(nDOFall*sizeof(double));
	double *u_grid = (double*)malloc(nNodeX*nNodeY*sizeof(double));

	FILE *fid1 = fopen("output/u_all.dat","rb");
	FILE *fid2;
	FILE *fid3;

	char *filename = (char*)malloc(50*sizeof(char));

	if( FWTR==FW ) fid3 = fopen("output/u_srf.dat","wb");

	printf("----- post processing -----\n");

	/* Check if the output directory exists. */
    
    //folderr = "C:\\Users\\SaMaN\\Desktop\\Ppln";
    folderr = "./output/frames";

    if( stat(folderr,&sb)!=0 || !S_ISDIR(sb.st_mode) )
    {
        printf("Output foler does not exist. Creating.\n");
        mkdir(folderr,0755);
    }

    /* Rearrange. */
	for( i0=0; i0<nTstep; i0++ )
	{
		// printf("%4i/%4i\n",i0,nTstep);

		fread(u_all,sizeof(double),nDOFall,fid1);

		// for plotting snapshots
		for( i1=0; i1<nDOFall; i1++ )
		{
			ix = node2GIx[DOF2node[i1]];
			iy = node2GIy[DOF2node[i1]];

			if( ix==-1 || iy==-1 )
				u_grid[nNodeY*ix+iy] = 0.;
			else
				u_grid[nNodeY*ix+iy] = u_all[i1];
		}
		sprintf(filename,"./output/frames/step%04i.dat",i0);
		fid2 = fopen(filename,"wb");
		fwrite(u_grid,sizeof(double),nNodeX*nNodeY,fid2);
		fclose(fid2);

		// for surface response
		if( FWTR==FW )
			fwrite(u_all,sizeof(double),nDOFsrf,fid3);
	}

	fclose(fid1);
	if( FWTR==FW ) fclose(fid3);

	free( u_all );
	free( u_grid );
	free( filename );
}