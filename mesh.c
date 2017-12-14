#include "mesh.h"

void mesh(
	int* nNode,
	int* nNodeX,
	int* nNodeY,
	int* nElem,
	int* nDOFall,
	int* nDOFsrf,
	int* nDOFreg,
	double** node2xy,
	int** node2GIx,
	int** node2GIy,
	int** node2DOF,
	int** elem2node,
	int** elem2loc,
	int** DOF2node,
	double *x,
	double *y,
	double *h
)
{
	int nElemX, nElemY;

	int i1, i2, i3, i4;

	struct stat sb;
	FILE *fid;

	printf("----- mesh generation -----\n");

	*x = 12.; // x and y must be N*h where N being an EVEN number
	*y = 5.;
	*h = .02;

	nElemX = (int)((*x)/((*h)*2));
	nElemY = (int)((*y)/((*h)*2));
	*nNodeX = 2*nElemX+1;
	*nNodeY = 2*nElemY+1;
	*nNode = (*nNodeX)*(*nNodeY);
	*nElem = nElemX*nElemY;

	// x and y coordinate and their location in 2D grid
	*node2xy = (double*)malloc((*nNode)*2*sizeof(double));
	*node2GIx = (int*)malloc((*nNode)*sizeof(int));
	*node2GIy = (int*)malloc((*nNode)*sizeof(int));
	for( i1=0; i1<(*nNodeY); i1++ )
	{
		for( i2=0; i2<(*nNodeX); i2++ )
		{
			i3 = i1*(*nNodeX)+i2;
			(*node2xy)[i3*2  ] = i2*(*h);		(*node2GIx)[i3] = i2;
			(*node2xy)[i3*2+1] = i1*(*h);		(*node2GIy)[i3] = i1;
		}
	}

	// element connectivity
	(*elem2node) = (int*)malloc((*nElem)*9*sizeof(int));
	(*elem2loc) = (int*)malloc((*nElem)*sizeof(int));
	for( i1=0; i1<nElemY; i1++ )
	{
		for( i2=0; i2<nElemX; i2++ )
		{
			i3 = i1*nElemX+i2; // element number
			i4 = (i1*2+1)*(*nNodeX)+2*i2+1; // center node number
			(*elem2node)[i3*9  ] = i4-(*nNodeX)-1;
			(*elem2node)[i3*9+1] = i4-(*nNodeX)+1;
			(*elem2node)[i3*9+2] = i4+(*nNodeX)+1;
			(*elem2node)[i3*9+3] = i4+(*nNodeX)-1;
			(*elem2node)[i3*9+4] = i4-(*nNodeX);
			(*elem2node)[i3*9+5] = i4+1;
			(*elem2node)[i3*9+6] = i4+(*nNodeX);
			(*elem2node)[i3*9+7] = i4-1;
			(*elem2node)[i3*9+8] = i4;

			// element location
			// surface
			if( fabs((*node2xy)[(*elem2node)[i3*9+2]*2+1]-(*y))<.001*(*h) )
				(*elem2loc)[i3] = LOC_SRF;
			else
				(*elem2loc)[i3] = LOC_REG;
		}
	}

	// degree of freedom
	(*node2DOF) = (int*)malloc((*nNode)*sizeof(int));
	for( i1=0; i1<(*nNode); i1++ )
		(*node2DOF)[i1] = -1;

	*nDOFall = 0;
	*nDOFsrf = 0;
	*nDOFreg = 0;
	for( i1=0; i1<(*nNode); i1++ )
	{
		// surface
		if( fabs((*node2xy)[i1*2+1]-(*y))<.001*(*h) &&
			fabs((*node2xy)[i1*2])>.001*(*h) && fabs((*node2xy)[i1*2])<(*x)-.001*(*h) )
		{
			(*nDOFsrf)++;
			(*nDOFreg)++;
			(*node2DOF)[i1] = (*nDOFall)++;
		}
	}
	for( i1=0; i1<(*nNode); i1++ )
	{
		if( (*node2DOF)[i1]==-1 &&
			fabs((*node2xy)[i1*2])>.001*(*h) && fabs((*node2xy)[i1*2])<(*x)-.001*(*h) && fabs((*node2xy)[i1*2+1])>.001*(*h) )
		{
			(*nDOFreg)++;
			(*node2DOF)[i1] = (*nDOFall)++;
		}
	}

	*DOF2node = (int*)malloc((*nDOFall)*sizeof(int));
	for( i1=0; i1<(*nNode); i1++ )
	{
		if( (*node2DOF)[i1]!=-1 )
			(*DOF2node)[(*node2DOF)[i1]] = i1;
	}

	/* Check if the output directory exists. */
    if( stat("./output",&sb)!=0 || !S_ISDIR(sb.st_mode) )
    {
        printf("Output foler does not exist. Creating.\n");
        mkdir("./output",0755);
    }

	fid = fopen("./output/model_info.txt","wt");
	fprintf(fid,"nNodeX,%i\n",*nNodeX);
	fprintf(fid,"nNodeY,%i\n",*nNodeY);
	fprintf(fid,"nDOFall,%i\n",*nDOFall);
	fprintf(fid,"nDOFsrf,%i\n",*nDOFsrf);
	fprintf(fid,"dx,%e\n",*h);
	fprintf(fid,"x,%e\n",*x);
	fprintf(fid,"y,%e\n",*y);
	fclose(fid);

	printf(" nNodeX=%i\n",*nNodeX);
	printf(" nNodeY=%i\n",*nNodeY);
	printf("nDOFall=%i\n",*nDOFall);
	printf("nDOFsrf=%i\n",*nDOFsrf);
	printf("     h=%g\n",*h);
}

// int main()
// {
// 	int nNode, nElem, nDOFall, nDOFsrf, nDOFreg;
// 	double* node2xy;
// 	int *node2GIx, *node2GIy, *node2DOF;
// 	int *elem2node, *elem2loc;
// 	double x, y, h;

// 	mesh(&nNode,&nElem,&nDOFall,&nDOFsrf,&nDOFreg,&node2xy,&node2GIx,&node2GIy,&node2DOF,&elem2node,&elem2loc,&x,&y,&h);

// 	free( node2xy );
// 	free( node2GIx );
// 	free( node2GIy );
// 	free( node2DOF );
// 	free( elem2node );
// 	free( elem2loc );
// }
