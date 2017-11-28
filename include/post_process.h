#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "global_variables.h"

void rearrange_for_plot( int FWTR, int nDOFall, int nDOFsrf, int nNodeX, int nNodeY, int nTstep, int node2GIx[], int node2GIy[], int DOF2node[] );