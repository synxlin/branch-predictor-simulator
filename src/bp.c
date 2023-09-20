/*
 *	Branch Predictor
 */

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "bp.h"


void Predictor_Init(Predictor type, uint32_t* width)
{
	branch_predictor->predictor_type = type;
	switch (type)
	{
	case bimodal:
	{
		branch_predictor->predictor = (BP_Bimodal *)malloc(sizeof(BP_Bimodal));
		if (branch_predictor->predictor == NULL)
			_error_exit("malloc")
		BPT_Initial((BPT *)branch_predictor->predictor, width[BIMODAL]);
		return;
	}
	case gshare:
	{
		branch_predictor->predictor = (BP_Gshare *)malloc(sizeof(BP_Gshare));
		if (branch_predictor->predictor == NULL)
			_error_exit("malloc")
		BP_Gshare *predictor = branch_predictor->predictor;
		/* initial global history register */
		predictor->global_history_register = (GHR *)malloc(sizeof(GHR));
		if (predictor->global_history_register == NULL)
			_error_exit("malloc")
		GHR_Initial(predictor->global_history_register, width[GHRegister]);
		/* initial branch predition table */
		predictor->branch_prediction_table = (BPT *)malloc(sizeof(BPT));
		if (predictor->branch_prediction_table == NULL)
			_error_exit("malloc")	
		BPT_Initial(predictor->branch_prediction_table, width[GSHARE]);
		return;
	}
	case hybrid:
	{
		branch_predictor->predictor = (BP_Hybrid *)malloc(sizeof(BP_Hybrid));
		if (branch_predictor->predictor == NULL)
			_error_exit("malloc")
		BP_Hybrid *predictor = branch_predictor->predictor;
		/* initial branch chooser table */
		predictor->branch_chooser_table = (BCT *)malloc(sizeof(BCT));
		if (predictor->branch_chooser_table == NULL)
			_error_exit("malloc")
		BCT_Initial(predictor->branch_chooser_table, width[BCTable]);
		/* initial bimodal predictor */
		predictor->bp_bimodal = (BP_Bimodal *)malloc(sizeof(BP_Bimodal));
		if (predictor->bp_bimodal == NULL)
			_error_exit("malloc")
		BPT_Initial((BPT *)predictor->bp_bimodal, width[BIMODAL]);
		/* initial gshare predictor*/
		predictor->bp_gshare = (BP_Gshare *)malloc(sizeof(BP_Gshare));
		predictor->bp_gshare->branch_prediction_table = (BPT *)malloc(sizeof(BPT));
		if (predictor->bp_gshare->branch_prediction_table == NULL)
			_error_exit("malloc")
		predictor->bp_gshare->global_history_register = (GHR *)malloc(sizeof(GHR));
		if (predictor->bp_gshare->global_history_register == NULL)
			_error_exit("malloc")
		BPT_Initial(predictor->bp_gshare->branch_prediction_table, width[GSHARE]);
		GHR_Initial(predictor->bp_gshare->global_history_register, width[GHRegister]);
		return;
	}
	case yeh_patt:
	{
		branch_predictor->predictor = (BP_Yeh_Patt *)malloc(sizeof(BP_Yeh_Patt));
		if (branch_predictor->predictor == NULL)
			_error_exit("malloc")
		BP_Yeh_Patt *predictor = branch_predictor->predictor;
		/* initial branch history table */
		predictor->branch_histroy_table = (BHT *)malloc(sizeof(BHT));
		if (predictor->branch_histroy_table == NULL)
			_error_exit("malloc")
		BHT_Initial(predictor->branch_histroy_table, width[BHTable], width[YEH_PATT]);
		/* initial branch predition table */
		predictor->branch_predition_table = (BPT *)malloc(sizeof(BPT));
		if (predictor->branch_predition_table == NULL)
			_error_exit("malloc")
		BPT_Initial(predictor->branch_predition_table, width[YEH_PATT]);
		return;
	}
	}
}

Taken_Result Bimodal_Predict(BP_Bimodal *predictor, uint32_t addr, uint8_t two_byte_inst)
{
	uint32_t index = Get_Index(addr, ((BPT *)predictor)->attributes.index_width, two_byte_inst);
	return BPT_Predict((BPT *)predictor, index);
}

Taken_Result Gshare_Predict(BP_Gshare *predictor, uint32_t addr, uint8_t two_byte_inst)
{
	uint32_t h = predictor->global_history_register->attributes.history_width;
	uint32_t i = predictor->branch_prediction_table->attributes.index_width;
	uint32_t index = Get_Index(addr, i, two_byte_inst);
	uint32_t history = predictor->global_history_register->history;
        uint32_t mask = (1 <<((i-h)))-1;
        uint32_t index_tail = index & mask;
	uint32_t index_head = (index >> (i - h)) ^ history;
	index = ((index_head) << (i - h)) | index_tail;
	return BPT_Predict(predictor->branch_prediction_table, index);
}

Result Predictor_Predict(uint32_t addr, uint8_t two_byte_inst)
{
	Result result;
	result.predict_predictor = branch_predictor->predictor_type;
	switch (branch_predictor->predictor_type)
	{
	case bimodal:
	{
		result.predict_taken[BIMODAL] = Bimodal_Predict((BP_Bimodal *)branch_predictor->predictor, addr, two_byte_inst);
		return result;
	}
	case gshare:
	{
		result.predict_taken[GSHARE] = Gshare_Predict((BP_Gshare *)branch_predictor->predictor, addr, two_byte_inst);
		return result;
	}
	case hybrid:
	{
		BP_Hybrid *predictor = branch_predictor->predictor;
		result.predict_taken[BIMODAL] = Bimodal_Predict(predictor->bp_bimodal, addr, two_byte_inst);
		result.predict_taken[GSHARE] = Gshare_Predict(predictor->bp_gshare, addr, two_byte_inst);
		result.predict_predictor = BCT_Predict(predictor->branch_chooser_table, addr, two_byte_inst);
		if (result.predict_predictor == bimodal)
			result.predict_taken[HYBRID] = result.predict_taken[BIMODAL];
		else
			result.predict_taken[HYBRID] = result.predict_taken[GSHARE];
		return result;
	}
	case yeh_patt:
	{
		BP_Yeh_Patt *predictor = branch_predictor->predictor;
		uint64_t history = BHT_Search(predictor->branch_histroy_table, addr, two_byte_inst);
		result.predict_taken[YEH_PATT] = BPT_Predict(predictor->branch_predition_table, history);
		return result;
	}
	default:
		return result;
	}
}

void Bimodal_Update(BP_Bimodal *predictor, uint32_t addr, uint8_t two_byte_inst, Result result)
{
	uint32_t index = Get_Index(addr, ((BPT *)predictor)->attributes.index_width, two_byte_inst);
	BPT_Update(predictor, index, result);
}

void Gshare_Update(BP_Gshare *predictor, uint32_t addr, uint8_t two_byte_inst, Result result)
{
	uint32_t h = predictor->global_history_register->attributes.history_width;
	uint32_t i = predictor->branch_prediction_table->attributes.index_width;
	uint32_t index = Get_Index(addr, i, two_byte_inst);
	uint32_t history = predictor->global_history_register->history;
        uint32_t mask = (1 <<((i-h)))-1;
        uint32_t index_tail = index & mask;
	uint32_t index_head = (index >> (i - h)) ^ history;
	index = ((index_head) << (i - h)) | index_tail;
	BPT_Update(predictor->branch_prediction_table, index, result);
}

void Predictor_Update(uint32_t addr, uint8_t two_byte_inst, Result result)
{
	switch (branch_predictor->predictor_type)
	{
	case bimodal:
	{
		Bimodal_Update((BP_Bimodal *)branch_predictor->predictor, addr, two_byte_inst, result);
		return;
	}
	case gshare:
	{
		BP_Gshare *predictor = branch_predictor->predictor;
		Gshare_Update(predictor, addr, two_byte_inst, result);
		GHR_Update(predictor->global_history_register, result);
		return;
	}
	case hybrid:
	{
		BP_Hybrid *predictor = branch_predictor->predictor;
		if (result.predict_predictor == bimodal)
			Bimodal_Update(predictor->bp_bimodal, addr, two_byte_inst, result);
		else
			Gshare_Update(predictor->bp_gshare, addr, two_byte_inst, result);
		GHR_Update(predictor->bp_gshare->global_history_register, result);
		BCT_Update(predictor->branch_chooser_table, addr, two_byte_inst, result);
		return;
	}
	case yeh_patt:
	{
		BP_Yeh_Patt *predictor = branch_predictor->predictor;
		uint64_t history = BHT_Search(predictor->branch_histroy_table, addr, two_byte_inst);
		BPT_Update(predictor->branch_predition_table, history, result);
		BHT_Update(predictor->branch_histroy_table, addr, two_byte_inst, result);
		return;
	}
	}
}

void BP_fprintf(FILE *fp)
{
	switch (branch_predictor->predictor_type)
	{
	case bimodal:
	{
		fprintf(fp, "Final Bimodal Table Contents: \n");
		BPT_fprintf((BPT *)(branch_predictor->predictor), fp);
		return;
	}
	case gshare:
	{
		BP_Gshare *predictor = branch_predictor->predictor;
		fprintf(fp, "Final GShare Table Contents: \n");
		BPT_fprintf(predictor->branch_prediction_table, fp);
		fprintf(fp, "\n");
		fprintf(fp, "Final GHR Contents: ");
		GHR_fprintf(predictor->global_history_register, fp);
		return;
	}
	case hybrid:
	{
		BP_Hybrid *predictor = branch_predictor->predictor;
		fprintf(fp, "Final Bimodal Table Contents: \n");
		BPT_fprintf((BPT *)predictor->bp_bimodal, fp);
		fprintf(fp, "\n");
		fprintf(fp, "Final GShare Table Contents: \n");
		BPT_fprintf(predictor->bp_gshare->branch_prediction_table, fp);
		fprintf(fp, "\n");
		fprintf(fp, "Final GHR Contents: ");
		GHR_fprintf(predictor->bp_gshare->global_history_register, fp);
		fprintf(fp, "\n");
		fprintf(fp, "Final Chooser Table Contents : \n");
		BCT_fprintf(predictor->branch_chooser_table, fp);
		return;
	}
	case yeh_patt:
	{
		BP_Yeh_Patt *predictor = branch_predictor->predictor;
		fprintf(fp, "Final History Table Contents: \n");
		BHT_fprintf(predictor->branch_histroy_table, fp);
		fprintf(fp, "\n");
		fprintf(fp, "Final Prediction Table Contents: \n");
		BPT_fprintf(predictor->branch_predition_table, fp);
		return;
	}
	}
}
