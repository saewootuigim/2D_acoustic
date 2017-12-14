#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include "global_variables.h"

void resolution_nodes( double dx, int loading_node, double node2xy[], int nDOFall, int DOF2node[], int node2GIx[], int node2GIy[], int **DOF2ResIx, int **DOF2ResIy );
void rearrange_for_plot( int nDOFall, int nDOFsrf, int nNodeX, int nNodeY, int nTstep, int node2GIx[], int node2GIy[], int DOF2node[], int DOF2ResIx[], int DOF2ResIy[], double node2xy[] );