all: main

FLAGS  = -O0 -g -Wall -I${PETSC_DIR}/include -I$(PETSC_DIR)/$(PETSC_ARCH)/include -I./include -I/opt/hdf5-1.8.19/include/

OBJECT = treat_args.o output.o shape_functions.o matrix_elemental.o matrix_assemble.o body_force.o mesh.o solver.o post_process.o main.o

include ${PETSC_DIR}/lib/petsc/conf/variables
include ${PETSC_DIR}/lib/petsc/conf/rules

main: $(OBJECT) chkopts
	-$(CLINKER) $(FLAGS) -o main $(OBJECT) $(PETSC_KSP_LIB)

%.o: %.c
	mpicc $(FLAGS) $< -c

include ${PETSC_DIR}/lib/petsc/conf/test
