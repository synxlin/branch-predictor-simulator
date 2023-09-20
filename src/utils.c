#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>


#include "utils.h"
#include "btb.h"
#include "bp.h"

void parse_arguments(int argc, char * argv[], Predictor *type, uint32_t* width, uint8_t* two_byte_inst)
{
	*two_byte_inst = 0;
	int index;
	int c;

        opterr = 0;

	while ((c = getopt (argc, argv, "c")) != -1)
        	switch (c)
		{
			case 'c':
				*two_byte_inst = 1;
				break;
			case '?':
				if (isprint (optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return;
			default:
				abort ();
		}
  
	index = optind - 1;
	int new_argc = argc - index;
  
	if (new_argc < 6 || new_argc > 9 || new_argc == 8)
		_output_error_exit("wrong number of input parameters")

	if (strcmp(argv[1+index], "bimodal") == 0)
	{
		*type = bimodal;
		if (new_argc != 6)
			_output_error_exit("wrong number of input parameters")
	}
	else if (strcmp(argv[1+index], "gshare") == 0)
	{
		*type = gshare;
		if (new_argc != 7)
			_output_error_exit("wrong number of input parameters")
	}
	else if (strcmp(argv[1+index], "hybrid") == 0)
	{
		*type = hybrid;
		if (new_argc != 9)
			_output_error_exit("wrong number of input parameters")
	}
	else if (strcmp(argv[1+index], "yehpatt") == 0)
	{
		*type = yeh_patt;
		if (new_argc != 7)
			_output_error_exit("wrong number of input parameters")
	}
	else
		_output_error_exit("invalid predictor type")

	switch (*type)
	{
	case bimodal:
	{
		width[BIMODAL] = atoi(argv[2+index]);
		width[BTBuffer] = atoi(argv[3+index]);
		width[ASSOC] = atoi(argv[4+index]);
		trace_file = argv[5+index];
		break;
	}
	case gshare:
	{
		width[GSHARE] = atoi(argv[2+index]);
		width[GHRegister] = atoi(argv[3+index]);
		width[BTBuffer] = atoi(argv[4+index]);
		width[ASSOC] = atoi(argv[5+index]);
		trace_file = argv[6+index];
		break;
	}
	case hybrid:
	{
		width[BCTable] = atoi(argv[2+index]);
		width[GSHARE] = atoi(argv[3+index]);
		width[GHRegister] = atoi(argv[4+index]);
		width[BIMODAL] = atoi(argv[5+index]);
		width[BTBuffer] = atoi(argv[6+index]);
		width[ASSOC] = atoi(argv[7+index]);
		trace_file = argv[8+index];
		break;
	}
	case yeh_patt:
	{
		width[BHTable] = atoi(argv[2+index]);
		width[YEH_PATT] = atoi(argv[3+index]);
		width[BTBuffer] = atoi(argv[4+index]);
		width[ASSOC] = atoi(argv[5+index]);
		trace_file = argv[6+index];
		break;
	}
	}
}

void Stat_Init()
{
	stat.num_branches = 0;
	stat.num_prediction = 0;
	stat.misprediction_rate = 0;
	memset(stat.num_misprediction, 0, sizeof(uint64_t) * 6);
}

uint32_t Get_Index(uint32_t addr, uint32_t index_width, uint8_t two_byte_inst)
{
	if(two_byte_inst)
	{
        	uint32_t lsb;
	        lsb = (addr & 2) << index_width;
		addr = addr ^ lsb;
	}
	return (addr << (30 - index_width)) >> (32 - index_width);
}

void Update_Stat(Result result)
{
	if (result.actual_branch == BRANCH)
		stat.num_branches++;
	if (result.predict_branch == not_branch && result.actual_branch == branch && result.actual_taken == taken)
		stat.num_misprediction[BTBuffer]++;
	if (result.predict_branch == not_branch)
	{
		stat.num_misprediction[5] = stat.num_misprediction[branch_predictor->predictor_type] + stat.num_misprediction[BTBuffer];
		stat.misprediction_rate = (double)stat.num_misprediction[5] / (double)stat.num_prediction * 100.0;
		return;
	}
	stat.num_prediction++;
	if (result.actual_taken != result.predict_taken[branch_predictor->predictor_type])
	{
		stat.num_misprediction[branch_predictor->predictor_type]++;
		if (branch_predictor->predictor_type == hybrid)
			stat.num_misprediction[result.predict_predictor]++;
	}
	stat.num_misprediction[5] = stat.num_misprediction[branch_predictor->predictor_type] + stat.num_misprediction[BTBuffer];
	stat.misprediction_rate = (double)stat.num_misprediction[5] / (double)stat.num_branches * 100.0;
}

void Result_fprintf(FILE *fp, int argc, char* argv[])
{
	fprintf(fp, "Command Line:\n");
	int i;
	for (i = 0; i < argc; i++)
		fprintf(fp, "%s ", argv[i]);
	fprintf(fp, "\n\n");
	if (branch_target_buffer != NULL)
	{
		BTB_fprintf(fp);
		fprintf(fp, "\n\n");
	}
	BP_fprintf(fp);
	fprintf(fp, "\n");
	fprintf(fp, "Final Branch Predictor Statistics:\n");
	fprintf(fp, "a. Number of branches: %llu\n", stat.num_branches);
	fprintf(fp, "b. Number of predictions from the branch predictor: %llu\n", stat.num_prediction);
	fprintf(fp, "c. Number of mispredictions from the branch predictor: %llu\n", stat.num_misprediction[branch_predictor->predictor_type]);
	fprintf(fp, "d. Number of mispredictions from the BTB: %llu\n", stat.num_misprediction[BTBuffer]);
	fprintf(fp, "e. Misprediction Rate: %4.2f percent\n", stat.misprediction_rate);
}
