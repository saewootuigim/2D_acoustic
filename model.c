#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "model.h"

void create_model( int FWTR, int *nNode, double *dim_x, double *dim_y, double **node2xy, int *nElem, int **elem2node, int **elem2loc, 
	int *nDOF, int **node2DOF, int **DOFx, int **DOFy, int *nDOFsrf, int **DOFx_srf, int **DOFy_srf, double *h )
{

	int i0, i1, i2;
	int nNodeX, nNodeY;
	int nElemX, nElemY;

	int elem, node;

	FILE *fid;

	const char* folderr; /* To check if the output folder exists. */
    struct stat sb;      /* and this one too */

	printf("----- creating model -----\n");

	/* coordinates, 4 vertices of a quadrilateral */
	*dim_x = 40.;
	*dim_y = 40.;
	*h = .1;
	nNodeX = (int)((*dim_x)/(*h) + 1e-3) + 1;
	nNodeY = (int)((*dim_y)/(*h) + 1e-3) + 1;
	*nNode = nNodeX*nNodeY;

	(*node2xy) = (double *)malloc((*nNode)*2*sizeof(double));

	for (i0 = 0; i0 < nNodeX; i0++) {
		for (i1 = 0; i1 < nNodeY; i1++) {
			node = i0*nNodeY + i1;
			(*node2xy)[2*node  ] = i0*(*h);
			(*node2xy)[2*node+1] = i1*(*h);
		}
	}
// // DEBUG: check nodal coordinates
// fid = fopen("node2xy.txt","wt");
// for( i0=0; i0<*nNode; i0++ ){
// 	fprintf(fid,"%i ",i0);
// 	for( i1=0; i1<2; i1++ ){
// 		fprintf(fid,"%e ",(*node2xy)[2*i0+i1]);
// 	}
// 	fprintf(fid,"\n");
// }
// fclose(fid);
// exit(1);
	/* element connectivity */
	nElemX = (nNodeX-1)/2;
	nElemY = (nNodeY-1)/2;
	*nElem = nElemX*nElemY;

	(*elem2node) = (int *)malloc((*nElem)*9*sizeof(int));

	for( i0=0; i0<nElemX; i0++ ) {
		for( i1=0; i1<nElemY; i1++ ) {
			elem = i0*nElemY + i1;
			(*elem2node)[9*elem  ] = (2*i0  )*nNodeY+(2*i1  );
			(*elem2node)[9*elem+1] = (2*i0+2)*nNodeY+(2*i1  );
			(*elem2node)[9*elem+2] = (2*i0+2)*nNodeY+(2*i1+2);
			(*elem2node)[9*elem+3] = (2*i0  )*nNodeY+(2*i1+2);
		}
	}

	for( i0=0; i0<*nElem; i0++ ) {
		/* bottom */
		(*elem2node)[9*i0+4] = ((*elem2node)[9*i0  ]+(*elem2node)[9*i0+1])/2;
		/* right */
		(*elem2node)[9*i0+5] = (*elem2node)[9*i0+1]+1;
		/* top */
		(*elem2node)[9*i0+6] = ((*elem2node)[9*i0+2]+(*elem2node)[9*i0+3])/2;
		/* left */
		(*elem2node)[9*i0+7] = (*elem2node)[9*i0+3]-1;
		/* center */
		(*elem2node)[9*i0+8] = ((*elem2node)[9*i0  ]+(*elem2node)[9*i0+1])/2+1;
	}
// // DEBUG: check element connectivity.
// fid = fopen("elem2node.txt","wt");
// for( i0=0; i0<*nElem; i0++ ){
// 	fprintf(fid,"%i ",i0);
// 	for( i1=0; i1<9; i1++ ){
// 		fprintf(fid,"%i ",(*elem2node)[9*i0+i1]);
// 	}
// 	fprintf(fid,"\n");
// }
// fclose(fid);
// exit(1);
	/* DOF */
	(*node2DOF) = (int *)malloc((*nNode)*2*sizeof(int));
	for( i0=0; i0<(*nNode)*2; i0++ ) {
		(*node2DOF)[i0] = -1;
	}

	(*nDOF) = 0;
	(*nDOFsrf) = 0;
	// Number the surface DOF first.
	for( i0=0; i0<(*nNode); i0++ ) {
		if( (*node2xy)[2*i0]>.001 && (*node2xy)[2*i0]<(*dim_x)-.001 && (*node2xy)[2*i0+1]>(*dim_y)-.001 ) {
			(*node2DOF)[2*i0  ] = (*nDOF)++;
			(*nDOFsrf)++;
			(*node2DOF)[2*i0+1] = (*nDOF)++;
			(*nDOFsrf)++;
		}
	}

	// Number the rest DOF.
	for( i0=0; i0<(*nNode); i0++ ) {
		if( (*node2xy)[2*i0]>.001 && (*node2xy)[2*i0]<(*dim_x)-.001 && (*node2xy)[2*i0+1]>.001 ) {
			if( (*node2DOF)[2*i0  ]==-1 ) (*node2DOF)[2*i0  ] = (*nDOF)++;
			if( (*node2DOF)[2*i0+1]==-1 ) (*node2DOF)[2*i0+1] = (*nDOF)++;
		}
	}

	/* If it is TR, adjust DOF. */
	if( FWTR==1 ) {

		printf(" adjusting DOF...\n");
		for( i0=0; i0<(*nNode); i0++ ) {
			for( i1=0; i1<2; i1++ ) {
				// surface DOF starts from -2 to negative
				if( (*node2DOF)[2*i0+i1]<(*nDOFsrf) && (*node2DOF)[2*i0+i1]>-1 )
					(*node2DOF)[2*i0+i1] = -(*node2DOF)[2*i0+i1]-2;
				// rest DOF starts from 0 to positive
				else if( (*node2DOF)[2*i0+i1]>(*nDOFsrf)-1 )
					(*node2DOF)[2*i0+i1] -= (*nDOFsrf);
			}
		}
		(*nDOF) -= (*nDOFsrf);
	}
// // DEBUG: check node to DOF mapping
// fid = fopen("node2DOF.txt","wt");
// for( i0=0; i0<*nNode; i0++ ){
// 	fprintf(fid,"%i ",i0);
// 	for( i1=0; i1<2; i1++ ){
// 		fprintf(fid,"%i ",(*node2DOF)[2*i0+i1]);
// 	}
// 	fprintf(fid,"\n");
// }
// fclose(fid);
// exit(1);
	/* DOF to node for regular domain except surface */
	(*DOFx) = (int *)malloc((*nDOF)/2*sizeof(int));
	(*DOFy) = (int *)malloc((*nDOF)/2*sizeof(int));
	i1 = 0;

	/* Check if the output directory exists. */
    
    //folderr = "C:\\Users\\SaMaN\\Desktop\\Ppln";
    folderr = "./output/";

    if( stat(folderr,&sb)!=0 || !S_ISDIR(sb.st_mode) )
    {
        printf("Output foler does not exist. Creating.\n");
        mkdir(folderr,0777);
    }

	fid = fopen("output/node2xy.txt","wt");
	for( i0=0; i0<(*nNode); i0++ ) {
		if( (*node2DOF)[2*i0]>-1 ) {
			(*DOFx)[i1  ] = (*node2DOF)[2*i0  ];
			(*DOFy)[i1++] = (*node2DOF)[2*i0+1];
			fprintf( fid, "%e %e\n", (*node2xy)[2*i0], (*node2xy)[2*i0+1] );
		}
	}
	fclose( fid );
// // DEBUG: check regular domain DOF numbers
// fid = fopen("DOF.txt","wt");
// for( i0=0; i0<(*nDOF)/2; i0++ ) {
// 	fprintf(fid,"%i %i %i\n",i0,(*DOFx)[i0],(*DOFy)[i0]);
// }
// fclose(fid);
// exit(1);
	printf("  dim x=%f, dim y=%f\n",(*dim_x),(*dim_y));
	printf("   nDOF=%i\n",(*nDOF));
	printf("  nNode=%i\n",(*nNode));
	printf("  nElem=%i\n",(*nElem));
	printf("nDOFsrf=%i\n",(*nDOFsrf));

	/* DOF of surface node */
	(*DOFx_srf) = (int *)malloc((*nDOFsrf)/2*sizeof(int));
	(*DOFy_srf) = (int *)malloc((*nDOFsrf)/2*sizeof(int));
	i1 = 0;
	if( FWTR==0 ) {
		for( i0=0; i0<(*nNode); i0++ ) {
			if( (*node2xy)[2*i0]>.001 && (*node2xy)[2*i0]<(*dim_x)-.001 && (*node2xy)[2*i0+1]>(*dim_y)-.001 ) {
				(*DOFx_srf)[i1  ] = (*node2DOF)[2*i0  ];
				(*DOFy_srf)[i1++] = (*node2DOF)[2*i0+1];
			}
		}
	} else if( FWTR==1 ) {
		for( i0=0; i0<(*nNode); i0++ ) {
			if( (*node2xy)[2*i0]>.001 && (*node2xy)[2*i0]<(*dim_x)-.001 && (*node2xy)[2*i0+1]>(*dim_y)-.001 ) {
				(*DOFx_srf)[i1  ] = -(*node2DOF)[2*i0  ]-2;
				(*DOFy_srf)[i1++] = -(*node2DOF)[2*i0+1]-2;
			}
		}
	}
// // DEBUG: check surface DOF numbers
// fid = fopen("DOF.txt","wt");
// for( i0=0; i0<(*nDOFsrf)/2; i0++ ) {
// 	fprintf(fid,"%i %i %i\n",i0,(*DOFx_srf)[i0],(*DOFy_srf)[i0]);
// }
// fclose(fid);
// exit(1);

	/* element to location */
	(*elem2loc) = (int *)malloc((*nElem)*sizeof(int));

	for( i0=0; i0<(*nElem); i0++ ) {

		// top center node
		i1 = (*elem2node)[9*i0+6];
		
		// if top center node is on surface
		if( (*node2xy)[2*i1+1]>(*dim_y)-.001 && (*node2xy)[2*i1]>.001 && (*node2xy)[2*i1]<(*dim_x)-.001 )
			(*elem2loc)[i0] = 0;
		// otherwise
		else
			(*elem2loc)[i0] = 1;
	}
// // DEBUG: check element location
// fid = fopen("elem2loc.txt","wt");
// for( i0=0; i0<(*nElem); i0++ ) {
// 	fprintf(fid,"%i %i\n",i0,(*elem2loc)[i0]);
// }
// fclose(fid);
// exit(1);

	fid = fopen("output/model_info.txt","wt");
	fprintf(fid,"number of nodes,");
	fprintf(fid,"%i\n",*nNode);
	fprintf(fid,"number of elements,");
	fprintf(fid,"%i\n",*nElem);
	fprintf(fid,"number of degrees of freedom,");
	fprintf(fid,"%i\n",*nDOF); 
	fprintf(fid,"number of surface degrees of freedom,");
	fprintf(fid,"%i\n",*nDOFsrf);
	fprintf(fid,"size of medel,");
	fprintf(fid,"%f,%f\n",*dim_x,*dim_y);
	fprintf(fid,"mesh size,");
	fprintf(fid,"%f\n",*h);
	fprintf(fid,"nodes on one side (X Y),");
	fprintf(fid,"%i,%i\n",nNodeX,nNodeY);
	fprintf(fid,"elements on one side (X Y),");
	fprintf(fid,"%i,%i\n",nElemX,nElemY);
	fclose(fid);

	fid = fopen("output/elem2node.txt","wt");
	for( i0=0; i0<*nElem; i0++ )
	{
		for( i1=0; i1<9; i1++ )
			fprintf(fid,"%i ",(*elem2node)[9*i0+i1]);
		fprintf(fid,"\n");
	}
	fclose(fid);

	fid = fopen("output/elem2loc.txt","wt");
	for( i0=0; i0<(*nElem); i0++ )
		fprintf(fid,"%i\n",(*elem2loc)[i0]);
	fclose(fid);

	fid = fopen("output/node2DOF.txt","wt");
	for( i0=0; i0<(*nNode); i0++ )
		fprintf(fid,"%i %i\n",(*node2DOF)[i0],(*node2DOF)[i0+1]);
	fclose(fid);

	fid = fopen("output/DOF_srf.txt","wt");
	for( i0=0; i0<(*nDOFsrf)/2; i0++ )
		fprintf(fid,"%i %i\n",(*DOFx_srf)[i0],(*DOFy_srf)[i0]);
	fclose(fid);

	fid = fopen("output/DOFx2node.txt","wt");
	for( i0=0; i0<(*nDOF)/2; i0++ )
		fprintf(fid,"%i\n",(*DOFx)[i0]);
	fclose(fid);

	fid = fopen("output/DOFy2node.txt","wt");
	for( i0=0; i0<(*nDOF)/2; i0++ )
		fprintf(fid,"%i\n",(*DOFy)[i0]);
	fclose(fid);
}