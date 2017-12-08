#ifndef _GLOBAL_VARIABLES_H_
#define _GLOBAL_VARIABLES_H_
enum {LOC_SRF, LOC_REG };
enum {FW,TR,TRFR,TRFI,TRFA,TRFN};

struct _cmd_args_
{
	int FWTR;
	int PS;
} cmd_arg;
// struct _cmd_args_ cmd_arg;

#endif