/*
 *	Branch Predictor
 */
#pragma once
#ifndef BP_H_
#define BP_H_

#include "bpt.h"
#include "bht.h"
#include "ghr.h"
#include "bct.h"

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
	Predictor predictor_type;
	void *predictor;
}BP;

extern BP* branch_predictor;

/*
 *	Initial the branch_predictor
 *	input	:
 *		type	:	the type of the predictor
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
void Predictor_Init(Predictor name, uint32_t* width);

/*
 *	Prediction on taken_or_not of bimodal predictor
 */
Taken_Result Bimodal_Predict(BP_Bimodal *predictor, uint32_t addr, uint8_t two_byte_inst);

/*
 *	Prediction on taken_or_not of gshare predictor
 */
Taken_Result Gshare_Predict(BP_Gshare *predictor, uint32_t addr, uint8_t two_byte_inst);

/*
 *	Prediction of bimodal predictor
 */
Result Predictor_Predict(uint32_t addr, uint8_t two_byte_inst);

/*
 *	Update bimodal prediction table
 */
void Bimodal_Update(BP_Bimodal *predictor, uint32_t addr, uint8_t two_byte_inst, Result result);

/*
 *	Update gshare prediction table
 */
void Gshare_Update(BP_Gshare *predictor, uint32_t addr, uint8_t two_byte_inst, Result result);

/*
 *	Update predictior
 */
void Predictor_Update(uint32_t addr, uint8_t two_byte_inst, Result result);

/*
 *	Print the content of branch_predictor to file *fp
 */
void BP_fprintf(FILE *fp);

#endif
