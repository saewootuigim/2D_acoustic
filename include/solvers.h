void solver_RK4( Vec Mreg, Mat Kreg, Mat Ksrf, Vec f, Vec u_init,
	int nDOFall, int nDOFsrf, double *node2xy, int *node2DOF, int DOFload, 
	double h, double dist_max, int *nTstep, double *dt );