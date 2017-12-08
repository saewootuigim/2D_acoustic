void assemble_matrix( Vec Mreg, Mat Kreg, Mat Ksrf, int nElem, int *elem2loc, int nDOFsrf, int *node2DOF, int *elem2node, double *node2xy );
void assemble_body_force( double dim_x, double dim_y, int nNode, double *node2xy, int *node2DOF, Vec f, int *loading_DOF, int *loading_node, double *dist_max );