/*
 *	Branch Predictor - Bimodal
 */
#pragma once

typedef BPT BP_Bimodal;

typedef struct BP_Gshare
{
	GHR* global_history_register;
	BPT* branch_prediction_table;
}BP_Gshare;

typedef struct BP_Hybrid
{
	BCT* branch_chooser_table;
	BP_Bimodal* bp_bimodal;
	BP_Gshare* bp_gshare;
}BP_Hybrid;

typedef struct BP_Yeh_Patt
{
	BHT* branch_histroy_table;
	BPT* branch_predition_table;
}BP_Yeh_Patt;

typedef struct BP
{
	Predictor predictor_name;
	void *predictor;
}BP;

extern BP* branch_predictor;

void Predictor_Init(Predictor name, uint32_t* width);

Taken_Result Bimodal_Predict(BP_Bimodal *predictor, uint32_t addr);

Taken_Result Gshare_Predict(BP_Gshare *predictor, uint32_t addr);

Result Predictor_Predict(uint32_t addr);

void Bimodal_Update(BP_Bimodal *predictor, uint32_t addr, Result result);

void Gshare_Update(BP_Gshare *predictor, uint32_t addr, Result result);

void Predictor_Update(uint32_t addr, Result result);

void BP_fprintf(FILE *fp);