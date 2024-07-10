/*
 *	branch chooser table
 */


#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "bct.h"


void BCT_Initial(BCT* BranchChooserTable, uint32_t index_width)
{
	BranchChooserTable->attributes.index_width = index_width;
	BranchChooserTable->attributes.chooser_num = (uint32_t)pow_2(index_width);

	BranchChooserTable->chooser = (Two_Bit_Chooser *)malloc(sizeof(Two_Bit_Chooser) * BranchChooserTable->attributes.chooser_num);
	if (BranchChooserTable->chooser == NULL)
		_error_exit("malloc")
	uint32_t i;
	for (i = 0; i < BranchChooserTable->attributes.chooser_num; i++)
		BranchChooserTable->chooser[i] = weakly_bimodal;
}

Predictor BCT_Predict(BCT* BranchChooserTable, uint32_t addr, uint8_t two_byte_inst)
{
  uint32_t index = Get_Index(addr, BranchChooserTable->attributes.index_width, two_byte_inst);
	switch (BranchChooserTable->chooser[index])
	{
	case strongly_gshare:
	case weakly_gshare:
		return gshare;
	default:
		return bimodal;
	}
}

void BCT_Update(BCT* BranchChooserTable, uint32_t addr, uint8_t two_byte_inst, Result result)
{
	if (result.actual_taken == result.predict_taken[BIMODAL] && result.actual_taken == result.predict_taken[GSHARE])
		return;
	if (result.actual_taken != result.predict_taken[BIMODAL] && result.actual_taken != result.predict_taken[GSHARE])
		return;
	uint32_t index = Get_Index(addr, BranchChooserTable->attributes.index_width, two_byte_inst);
	if (result.actual_taken == result.predict_taken[GSHARE])
	{
		switch (BranchChooserTable->chooser[index])
		{
		case strongly_bimodal:
		case weakly_bimodal:
		case weakly_gshare:
			BranchChooserTable->chooser[index]++;
		default:
			return;
		}
	}
	else
	{
		switch (BranchChooserTable->chooser[index])
		{
		case strongly_gshare:
		case weakly_gshare:
		case weakly_bimodal:
			BranchChooserTable->chooser[index]--;
		default:
			return;
		}
	}
}

void BCT_fprintf(BCT* BranchChooserTable, FILE *fp)
{
	uint32_t i;
	for (i = 0; i < BranchChooserTable->attributes.chooser_num; i++)
		fprintf(fp, "Choice table[%u]: %u\n", i, BranchChooserTable->chooser[i]);
}
