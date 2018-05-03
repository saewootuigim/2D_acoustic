#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include "global_variables.h"

#define USAGE                                         \
	"usage:\n"                                        \
	"\tmesh [options]\n"                              \
	"options:\n"                                      \
	"\t-x or --width \twidth of domain(default: 12)\n"\
	"\t-y or --height\theight of domain(default: 5)\n"\
	"\t-s or --size  \tsize of mesh(default: 0.02)\n" \
	"\t-h or --help  \tprint this message\n"

static struct option gLongOptions[] = {
	{"width", required_argument, NULL, 'x'},
	{"height", required_argument, NULL, 'y'},
	{"size", required_argument, NULL, 's'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0}};

int main( int argc, char *argv[] )
{
	int option_char = 0;

	int nNode, nElem;
	int nDOFall, nDOFsrf, nDOFreg;
	double *node2xy;
	int *node2GIx, *node2GIy, *node2DOF, *elem2node, *elem2loc, *DOF2node;
	double x = 12.;
	double y = 5.;
	double h = .02;

	int nNodeX, nNodeY;
	int nElemX, nElemY;

	int i0, i1, i2, i3, i4;

	FILE *fid;

	// input arguments
	while( (option_char=getopt_long( argc, argv, "x:y:s:hx", gLongOptions, NULL )) != -1 )
	{
		switch (option_char)
		{
		case 'x':
			x = atof(optarg);
			break;
		case 'y':
			y = atof(optarg);
			break;
		case 's':
			h = atof(optarg);
			break;
		case 'h':
		default:
			printf("%s",USAGE);
			exit(1);
		}
	}

	// partition arguments
	printf("--- partition arguments ---\n");
	printf(" x | %f\n",x);
	printf(" y | %f\n",y);
	printf(" s | %f\n",h);
	printf("\n");

	// x = 12.; // x and y must be N*h where N being an EVEN number

	nElemX = (int)(x/(h*2));
	nElemY = (int)(y/(h*2));
	nNodeX = 2*nElemX+1;
	nNodeY = 2*nElemY+1;
	nNode = nNodeX*nNodeY;
	nElem = nElemX*nElemY;

	// x and y coordinate and their location in 2D grid
	node2xy = (double*)malloc(nNode*2*sizeof(double));
	node2GIx = (int*)malloc(nNode*sizeof(int));
	node2GIy = (int*)malloc(nNode*sizeof(int));
	for( i1=0; i1<nNodeY; i1++ )
	{
		for( i2=0; i2<nNodeX; i2++ )
		{
			i3 = i1*nNodeX+i2;
			node2xy[i3*2  ] = i2*h;		node2GIx[i3] = i2;
			node2xy[i3*2+1] = i1*h;		node2GIy[i3] = i1;
		}
	}

	// element connectivity
	elem2node = (int*)malloc(nElem*9*sizeof(int));
	elem2loc = (int*)malloc(nElem*sizeof(int));
	for( i1=0; i1<nElemY; i1++ )
	{
		for( i2=0; i2<nElemX; i2++ )
		{
			i3 = i1*nElemX+i2; // element number
			i4 = (i1*2+1)*nNodeX+2*i2+1; // center node number
			elem2node[i3*9  ] = i4-nNodeX-1;
			elem2node[i3*9+1] = i4-nNodeX+1;
			elem2node[i3*9+2] = i4+nNodeX+1;
			elem2node[i3*9+3] = i4+nNodeX-1;
			elem2node[i3*9+4] = i4-nNodeX;
			elem2node[i3*9+5] = i4+1;
			elem2node[i3*9+6] = i4+nNodeX;
			elem2node[i3*9+7] = i4-1;
			elem2node[i3*9+8] = i4;

			// element location
			// surface
			if( fabs(node2xy[elem2node[i3*9+2]*2+1]-y)<.001*h )
				elem2loc[i3] = LOC_SRF;
			else
				elem2loc[i3] = LOC_REG;
		}
	}

	// degree of freedom
	node2DOF = (int*)malloc(nNode*sizeof(int));
	for( i1=0; i1<nNode; i1++ )
		node2DOF[i1] = -1;

	nDOFall = 0;
	nDOFsrf = 0;
	nDOFreg = 0;
	for( i1=0; i1<nNode; i1++ )
	{
		// surface
		if( fabs(node2xy[i1*2+1]-y)<.001*h &&
			fabs(node2xy[i1*2])>.001*h && fabs(node2xy[i1*2])<x-.001*h )
		{
			nDOFsrf++;
			nDOFreg++;
			node2DOF[i1] = (nDOFall)++;
		}
	}
	for( i1=0; i1<nNode; i1++ )
	{
		if( node2DOF[i1]==-1 &&
			fabs(node2xy[i1*2])>.001*h && fabs(node2xy[i1*2])<x-.001*h && fabs(node2xy[i1*2+1])>.001*h )
		{
			nDOFreg++;
			node2DOF[i1] = (nDOFall)++;
		}
	}

	DOF2node = (int*)malloc((nDOFall)*sizeof(int));
	for( i1=0; i1<nNode; i1++ )
	{
		if( node2DOF[i1]!=-1 )
			(DOF2node)[node2DOF[i1]] = i1;
	}

	// Save.
	printf("--- mesh generation ---\n");
	printf(" nNode   | %i\n",nNode);
	printf(" nElem   | %i\n",nElem);
	printf(" nDOFall | %i\n",nDOFall);
	printf(" nDOFsrf | %i\n",nDOFsrf);
	printf(" nDOFreg | %i\n",nDOFreg);
	printf("\n");

	fid = fopen("model_info.txt","wt");
	fprintf(fid,"nNode,%i\n",nNode);
	fprintf(fid,"nElem,%i\n",nElem);
	fprintf(fid,"nNodeX,%i\n",nNodeX);
	fprintf(fid,"nNodeY,%i\n",nNodeY);
	fprintf(fid,"nDOFall,%i\n",nDOFall);
	fprintf(fid,"nDOFsrf,%i\n",nDOFsrf);
	fprintf(fid,"nDOFreg,%i\n",nDOFreg);
	fprintf(fid,"h,%e\n",h);
	fprintf(fid,"x,%e\n",x);
	fprintf(fid,"y,%e\n",y);
	fclose(fid);
	
	fid = fopen("node2xy.txt","wt");
	for( i0=0; i0<nNode; i0++ )
		fprintf(fid,"%e %e\n",node2xy[i0*2],node2xy[i0*2+1]);
	fclose(fid);
	fid = fopen("node2GIx.txt","wt");
	for( i0=0; i0<nNode; i0++ )
		fprintf(fid,"%i ",node2GIx[i0]);
	fclose(fid);
	fid = fopen("node2GIy.txt","wt");
	for( i0=0; i0<nNode; i0++ )
		fprintf(fid,"%i ",node2GIy[i0]);
	fclose(fid);
	fid = fopen("node2DOF.txt","wt");
	for( i0=0; i0<nNode; i0++ )
		fprintf(fid,"%i ",node2DOF[i0]);
	fclose(fid);
	fid = fopen("elem2node.txt","wt");
	for( i0=0; i0<nElem; i0++ )
	{
		for( i1=0; i1<9; i1++ )
			fprintf(fid,"%i ",elem2node[i0*9+i1]);
		fprintf(fid,"\n");
	}
	fclose(fid);
	fid = fopen("elem2loc.txt","wt");
	for( i0=0; i0<nElem; i0++ )
		fprintf(fid,"%i ",elem2loc[i0]);
	fclose(fid);
	fid = fopen("DOF2node.txt","wt");
	for( i0=0; i0<nDOFall; i0++ )
		fprintf(fid,"%i ",(DOF2node)[i0]);
	fclose(fid);

	return 0;
}