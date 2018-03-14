Branch Predictor Simulator
==========================

(Latest update: 2016/11/30)

## Introduction

This is a branch predictor simulator.

## Usage

Using the command shown as follows.

To complie the program,

>make

To run the program,

1. Bimodal predictor:

	> sim_bp bimodal < i_B \> < i_BTB \> < assoc_BTB \> < tracefile \>

	>> < i_B \> is the number of PC bits used to index the bimodal table, the rest are obvious.

	>> < i_BTB \> is the number of PC bits used to index the BTB.

	>> < assoc_BTB \> is the BTB associativity.

	>> Setting either < i_BTB \> or < assoc_BTB \> to 0 implies that there is no BTB.

2. Gshare predictor: 

	> sim_bp gshare < i_G \> < h \> < i_BTB \> < assoc_BTB \> < tracefile \>

	>> < i_G \> and < h \> are the number of PC and GHR bits used to index the gshare table, respectively. 

	>> The other parameters are identical to the bimodal case.  

3. Hydbrid predictor:  

	> sim_bp hybrid < i_C \> < i_G \> < h > < i_B \> < i_BTB \> < assoc_BTB \> < tracefile \>
	
	>> < i_C \> is the number of PC bits used to index the chooser table. The other parameters are same as in the bimodal and gshare cases. 

4. YehPatt predictor:

	> sim_bp yehpatt < h \> < p \> < i_BTB \> < assoc_BTB \> < tracefile \>

	>> < h \> and < p \> are the number of PC and local history register bits used to index the history table and pattern table, respectively. The other parameters are identical to the bimodal case.

### Output

The output of program includes

1. The commandline content

2. The branch target buffer 

3. The predictor content (such as prediction table, chooser table, global history register, history table)

4. The statistics

	a. Number of branches 

	b. Number of predictions from the branch predictor 

	c. Number of mispredictions from the branch predictor 

	d. Number of mispredictions from the BTB 

	e. Branch misprediction rate ((item d + item c) / item a) 

### Trace File
The simulator reads a trace file in the following format: 

< branch PC in hex \> t|n

< branch PC in hex \> t|n

...

The first field is the address of the branch instruction in memory. This field is used to index into predictors. The second field indicates whether the branch is actually taken or not. Here is a sample trace. 

00a3b5fc t

00a3b604 t

00a3b60c n
