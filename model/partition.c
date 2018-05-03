#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "metis.h"

#define USAGE                                                                      \
	"**************************************************************************\n" \
	"program usage:\n"                                                             \
	"\ta.out [options]\n"                                                          \
	"options:\n"                                                                   \
	"\t-n or --nparts\tnumber of parts to be decomposed (default: 1)\n"                         \
	"\t-h or --help  \tprint this message\n"                                       \
	"**************************************************************************\n\n"

static struct option gLongOptions[] = {
	{"nparts", required_argument, NULL, 'n'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0}};

int main( int argc, char *argv[] )
{
	int option_char = 0;

	idx_t options[METIS_NOPTIONS];

	int i0, i1, i2;
	FILE *fid;

	int nNode, nElem, nDOFall, nDOFsrf, nDOFreg;
	double x, y, h;
	double *node2xy;
	int *node2GIx, *node2GIy, *node2DOF, *elem2node, *elem2loc, *DOF2node;

	idx_t *eptr, *eind;
	idx_t nparts=1;
	idx_t objval, *epart, *npart;

	// input arguments
	while( (option_char=getopt_long( argc, argv, "n:hx", gLongOptions, NULL )) != -1 )
	{
		switch (option_char)
		{
		case 'n': // number of partitions
			nparts = atoi(optarg);
			break;
		case 'h':
		default:
			printf("%s",USAGE);
			exit(1);
		}
	}

	// partition arguments
	printf("--- partition arguments ---\n");
	printf(" nparts  | %i\n",nparts);
	printf("\n");

	// Read model.
	fid = fopen("model_info.txt","rt");
    fscanf(fid,"%i",&nNode);
	fscanf(fid,"%i",&nElem);
	fscanf(fid,"%i",&nDOFall);
	fscanf(fid,"%i",&nDOFsrf);
	fscanf(fid,"%i",&nDOFreg);
	fscanf(fid,"%le",&x);
	fscanf(fid,"%le",&y);
	fscanf(fid,"%le",&h);
	fclose(fid);
	
	printf("--- model info ---\n");
	printf(" nNode   | %i\n",nNode);
	printf(" nElem   | %i\n",nElem);
	printf(" nDOFall | %i\n",nDOFall);
	printf(" nDOFsrf | %i\n",nDOFsrf);
	printf(" nDOFreg | %i\n",nDOFreg);
	printf(" x       | %f\n",x);
	printf(" y       | %f\n",y);
	printf(" h       | %f\n",h);
	printf("\n");

	node2xy = (double*)malloc(nNode*2*sizeof(double));
	fid = fopen("node2xy.txt","rt");
	for( i0=0; i0<nNode; i0++ )
		fscanf(fid," %le %le",&(node2xy[i0*2]),&(node2xy[i0*2+1]));
	fclose(fid);

	node2GIx = (int*)malloc(nNode*sizeof(int));
    fid = fopen("node2GIx.txt","rt");
	for( i0=0; i0<nNode; i0++ )
		fscanf(fid," %i",&(node2GIx[i0]));
	fclose(fid);

	node2GIy = (int*)malloc(nNode*sizeof(int));
    fid = fopen("node2GIy.txt","rt");
	for( i0=0; i0<nNode; i0++ )
		fscanf(fid," %i",&(node2GIy[i0]));
	fclose(fid);

	node2DOF = (int*)malloc(nNode*sizeof(int));
    fid = fopen("node2DOF.txt","rt");
	for( i0=0; i0<nNode; i0++ )
		fscanf(fid," %i",&(node2DOF[i0]));
	fclose(fid);

	elem2node = (int*)malloc(nElem*9*sizeof(int));
    fid = fopen("elem2node.txt","rt");
	for( i0=0; i0<nElem; i0++ )
		for( i1=0; i1<9; i1++ )
			fscanf(fid," %i",&(elem2node[i0*9+i1]));
	fclose(fid);

	elem2loc = (int*)malloc(nElem*sizeof(int));
    fid = fopen("elem2loc.txt","rt");
	for( i0=0; i0<nElem; i0++ )
		fscanf(fid," %i",&(elem2loc[i0]));
	fclose(fid);

	DOF2node = (int*)malloc(nDOFall*sizeof(int));
    fid = fopen("DOF2node.txt","rt");
	for( i0=0; i0<nDOFall; i0++ )
		fscanf(fid," %i",&(DOF2node[i0]));
	fclose(fid);

	// Form eptr and eind for partitioning.
	eptr = (idx_t*)malloc((nElem+1)*sizeof(idx_t));
	eind = (idx_t*)malloc(nElem*9*sizeof(idx_t));

	i2 = 0;
	for( i0=0; i0<nElem; i0++ )
	{
		eptr[i0] = i2;
		for( i1=0; i1<9; i1++ )
			eind[i2++] = elem2node[i0*9+i1];
	}
	eptr[nElem] = i2;

	// Partition.
	epart = (idx_t*)malloc(nElem*sizeof(idx_t));
	npart = (idx_t*)malloc(nNode*sizeof(idx_t));

	METIS_SetDefaultOptions(options);
	options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_VOL;

	METIS_PartMeshNodal( &nElem, &nNode, eptr, eind, NULL, NULL, &nparts, NULL, options, &objval, epart, npart );

	// Print the partition result.
	fid = fopen("epart.txt","wt");
	for( i0=0; i0<nElem; i0++ )
		fprintf(fid,"%i ",epart[i0]);
	fclose(fid);

	fid = fopen("npart.txt","wt");
	for( i0=0; i0<nNode; i0++ )
		fprintf(fid,"%i ",npart[i0]);
	fclose(fid);

	// Wrap up.
	free(node2xy);
	free(node2GIx);
	free(node2GIy);
	free(node2DOF);
	free(elem2node);
	free(elem2loc);
	free(DOF2node);
	free(eptr);
	free(eind);
	free(epart);
	free(npart);

	return 0;
}