#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "btb.h"
#include "bp.h"

void parse_arguments(int argc, char * argv[], Predictor *type, uint32_t* width)
{
	if (argc < 5 || argc > 9 || argc == 7)
		_output_error_exit("wrong number of input parameters")

	if (strcmp(argv[1], "bimodal"))
	{
		*type = bimodal;
		if (argc != 5)
			_output_error_exit("wrong number of input parameters")
	}
	else if (strcmp(argv[1], "gshare"))
	{
		*type = gshare;
		if (argc != 6)
			_output_error_exit("wrong number of input parameters")
	}
	else if (strcmp(argv[1], "hybrid"))
	{
		*type = hybrid;
		if (argc != 8)
			_output_error_exit("wrong number of input parameters")
	}
	else if (strcmp(argv[1], "yehpatt"))
	{
		*type = yeh_patt;
		if (argc != 6)
			_output_error_exit("wrong number of input parameters")
	}
	else
		_output_error_exit("invalid predictor type")

	switch (*type)
	{
	case bimodal:
	{
		width[BIMODAL] = atoi(argv[2]);
		width[BTBuffer] = atoi(argv[3]);
		width[ASSOC] = atoi(argv[4]);
		break;
	}
	case gshare:
	{
		width[GSHARE] = atoi(argv[2]);
		width[GHRegister] = atoi(argv[3]);
		width[BTBuffer] = atoi(argv[4]);
		width[ASSOC] = atoi(argv[5]);
		break;
	}
	case hybrid:
	{
		width[BCTable] = atoi(argv[2]);
		width[GSHARE] = atoi(argv[3]);
		width[GHRegister] = atoi(argv[4]);
		width[BIMODAL] = atoi(argv[5]);
		width[BTBuffer] = atoi(argv[6]);
		width[ASSOC] = atoi(argv[7]);
		break;
	}
	case yeh_patt:
	{
		width[BHTable] = atoi(argv[2]);
		width[YEH_PATT] = atoi(argv[3]);
		width[BTBuffer] = atoi(argv[4]);
		width[ASSOC] = atoi(argv[5]);
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

uint32_t Get_Index(uint32_t addr, uint32_t index_width)
{
	return (addr << (30 - index_width)) >> (32 - index_width);
}

void Update_Stat(Result result)
{
	if (result.actual_branch == BRANCH)
		stat.num_branches++;
	if (result.predict_branch != result.actual_branch)
		stat.num_misprediction[BTBuffer]++;
	if (result.predict_branch == NOT_BRANCH)
	{
		stat.num_misprediction[5] = stat.num_misprediction[branch_predictor->predictor_type] + stat.num_misprediction[BTBuffer];
		stat.misprediction_rate = (double)stat.num_misprediction[5] / (double)stat.num_prediction * 100.0;
		return;
	}

	if (result.actual_taken != result.predict_taken[branch_predictor->predictor_type])
	{
		stat.num_misprediction[branch_predictor->predictor_type]++;
		if (branch_predictor->predictor_type == hybrid)
			stat.num_misprediction[result.predict_predictor]++;
	}

	if (result.predict_taken[result.predict_predictor] == taken)
		stat.num_prediction++;
	stat.num_misprediction[5] = stat.num_misprediction[branch_predictor->predictor_type] + stat.num_misprediction[BTBuffer];
	stat.misprediction_rate = (double)stat.num_misprediction[5] / (double)stat.num_prediction * 100.0;
}

void Result_fprintf(FILE *fp, int argc, char* argv[])
{
	fprintf(fp, "Command Line:\n");
	int i;
	for (i = 0; i < argc; i++)
		fprintf(fp, "%s ", argv[i]);
	if (branch_target_buffer != NULL)
		BTB_fprintf(fp);
	fprintf(fp, "\n\n");
	BP_fprintf(fp);
	fprintf(fp, "\n");
	fprintf(fp, "Final Branch Predictor Statistics:\n");
	fprintf(fp, "a. Number of branches: %llu\n", stat.num_branches);
	fprintf(fp, "b. Number of predictions from the branch predictor: %llu\n", stat.num_prediction);
	fprintf(fp, "c. Number of mispredictions from the branch predictor: %llu\n", stat.num_misprediction[branch_predictor->predictor_type]);
	fprintf(fp, "d. Number of mispredictions from the BTB: %llu\n", stat.num_misprediction[BTBuffer]);
	fprintf(fp, "e. Misprediction Rate:  %3.2f  percent\n", stat.misprediction_rate);
}