void solver_RK4( int FWTR, Vec Mreg, Mat Kreg, Mat Ksrf, Vec f, 
	int nDOFall, int nDOFsrf, double *node2xy, int *node2DOF, int DOFload, 
	double h, double dist_max, int *nTstep, double *dt );