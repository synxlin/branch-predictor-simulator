/*
 *	branch chooser table
 */


#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "bct.h"

 /*
  *	Inital the branch chooser table
  */
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

/*
 *	Search the BranchChooserTable for PC "addr" and make prediction
 *	input	:
 *		addr	:	PC
 *	return	:
 *		the prediction on which predictor is chosen
 */
Predictor BCT_Predict(BCT* BranchChooseTable, uint32_t addr)
{
	uint32_t index = Get_Index(addr, BranchChooseTable->attributes.index_width);
	switch (BranchChooseTable->chooser[index])
	{
	case strongly_gshare:
	case weakly_gshare:
		return gshare;
	default:
		return bimodal;
	}
}

/*
 *	Update the BranchChooserTable
 *	input	:
 *		addr	:	PC
 *		result	:	struct "Result", the prediction and actual result
 */
void BCT_Update(BCT* BranchChooseTable, uint32_t addr, Result result)
{
	if (result.actual_taken == result.predict_taken[BIMODAL] && result.actual_taken == result.predict_taken[GSHARE])
		return;
	if (result.actual_taken != result.predict_taken[BIMODAL] && result.actual_taken != result.predict_taken[GSHARE])
		return;
	uint32_t index = Get_Index(addr, BranchChooseTable->attributes.index_width);
	if (result.actual_branch == result.predict_taken[GSHARE])
	{
		switch (BranchChooseTable->chooser[index])
		{
		case strongly_bimodal:
		case weakly_bimodal:
		case weakly_gshare:
			BranchChooseTable->chooser[index]++;
		default:
			return;
		}
	}
	else
	{
		switch (BranchChooseTable->chooser[index])
		{
		case strongly_gshare:
		case weakly_gshare:
		case weakly_bimodal:
			BranchChooseTable->chooser[index]--;
		default:
			return;
		}
	}
}