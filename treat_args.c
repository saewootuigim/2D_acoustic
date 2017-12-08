#include "treat_args.h"

void simulation_type( char *str )
{
	if( !strcmp(str,"FW") ) {
		cmd_arg.FWTR = FW;
		printf(" forward\n");
	} else if( !strcmp(str,"TR") ) {
		cmd_arg.FWTR = TR;
		printf(" time reversal\n");
	} else if( !strcmp(str,"TRFR") ) {
		cmd_arg.FWTR = TRFR;
		printf(" time reversal with filtered record, real part\n");
	} else if( !strcmp(str,"TRFI") ) {
		cmd_arg.FWTR = TRFI;
		printf(" time reversal with filtered record, imag part\n");
	} else if( !strcmp(str,"TRFA") ) {
		cmd_arg.FWTR = TRFA;
		printf(" time reversal with filtered record, real + imag\n");
	} else if( !strcmp(str,"TRFN") ) {
		cmd_arg.FWTR = TRFN;
		printf(" time reversal with filtered record, real^2+imag^2\n");
	} else {
		printf(" unrecognized option for -s %s\n",str);
		exit(1);
	}
}

void print_step( char *str )
{
	cmd_arg.PS = atoi(str);
	if( cmd_arg.PS<=0 )
	{
		printf(" print time step must be >0.\n");
		exit(1);
	}
	else
	{
		printf(" print time step = %i\n",cmd_arg.PS);
	}
}

void print_help()
{
	printf("Usage: ./main [options]\n");
	printf(" -s str   simulation type\n");
	printf("          FW = forward\n");
	printf("          TR = time reversal with unfiltered source\n");
	printf("          TRFR = time reversal with filtered source, real part only\n");
	printf("          TRFI = time reversal with filtered source, imaginary part only\n");
	printf("          TRFN = time reversal with filtered source, modulus\n");
	printf("\n");
	printf(" -p N     print time step\n");
	printf("          in post-processing, program will print every N step.\n");
	printf("\n");
	exit(1);
}

void treat_arguments( int argc, char *argv[] )
{
	cmd_arg.FWTR = -1;
	cmd_arg.PS=1;

	int i0;

	if( argc<2 ) print_help();

	for( i0=1; i0<argc; i0+=2 )
	{
		if( strcmp(argv[i0],"-s")==0 )
			simulation_type(argv[i0+1]);
		// else if( strcmp(argv[i0],"-b")==0 )
		// 	BF = body_force(argv[i0+1]);
		// else if( strcmp(argv[i0],"-i")==0 )
		// 	IC = initial_condition(argv[i0+1]);
		// else if( strcmp(argv[i0],"-o")==0)
		// 	OS = offset(argv[i0+1]);
		else if( strcmp(argv[i0],"-p")==0 )
			print_step(argv[i0+1]);
		else
			print_help();
	}
}