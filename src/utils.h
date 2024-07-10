#pragma once

#ifndef UTILS_H_
#define UTILS_H_

#define UINT32_MAX 4294967295

#define BIMODAL 0
#define GSHARE 1
#define HYBRID 2
#define YEH_PATT 3
#define BTBuffer 4
#define GHRegister 5
#define BCTable 6
#define BHTable 7
#define ASSOC 8

#define NOT_BRANCH 0
#define BRANCH 1

#define NOT_TAKEN 0
#define TAKEN 1

#define VALID 1
#define INVALID 0

#define CORRECT 0
#define WRONG 1

#define _error_exit(fun) { perror(fun); exit(EXIT_FAILURE); }

#define _output_error_exit(msg) { printf("error: %s", msg); exit(EXIT_FAILURE);}

#define pow_2(num) (1 << ((int)(num)))

typedef char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef enum Branch_Result
{ 
	not_branch = NOT_BRANCH,
	branch = BRANCH 
} Branch_Result;

typedef enum Taken_Result
{ 
	not_taken = NOT_TAKEN,
	taken = TAKEN 
} Taken_Result;

typedef enum Predictor
{
	bimodal = BIMODAL,
	gshare = GSHARE,
	hybrid = HYBRID,
	yeh_patt = YEH_PATT
}Predictor;

typedef struct Result
{
	Predictor predict_predictor;
	Branch_Result predict_branch;
	Taken_Result predict_taken[4];
	Branch_Result actual_branch;
	Taken_Result actual_taken;
}Result;

typedef struct Stat
{
	uint64_t num_branches;
	uint64_t num_prediction;
	uint64_t num_misprediction[6];
	double misprediction_rate;
}Stat;

extern Stat stat;
extern char *trace_file;

/*
 *	Parse the arguments to Predictor "type" and array "width"
 *	input(output)	:
 *		type	:	the type of predictor
 *		width	:	the array of width
 *					width[BIMODAL]		:	bimodal, hybrid		i_B
 *					width[GSHARE]		:	gshare, hybrid		i_G
 *					width[GHRegister]	:	gshare, hybrid		h
 *					width[BCTable]		:	hybrid				i_C
 *					width[BHTable]		:	yet_patt			h
 *					width[YEH_PATT]		:	yet_patt			p
 *					width[BTBuffer]		:	all					i_BTB
 *					width[ASSOC]		:	all					assoc
 */
void parse_arguments(int argc, char * argv[], Predictor *type, uint32_t* width, uint8_t *two_byte_inst);

/*
 *	Initial the stat (global statistic data)
 */
void Stat_Init();

/*
 *	get index from "addr"
 */
uint32_t Get_Index(uint32_t addr, uint32_t index_width, uint8_t two_byte_inst);

/*
 *	Update the stat according to result
 */
void Update_Stat(Result result);

/*
 *	Print the result to file *fp
 */
void Result_fprintf(FILE *fp, int argc, char* argv[]);

#endif
