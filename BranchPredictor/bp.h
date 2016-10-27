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