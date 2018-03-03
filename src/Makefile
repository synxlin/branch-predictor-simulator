
# Branch Predictor simulator Makefile
PROG = sim_bp
SRCS = main.c utils.c btb.c bct.c bht.c bpt.c ghr.c bp.c
OBJS = $(SRCS:.c=.o)
CLEANFILES = $(PROG) $(OBJS)

# input parameter
# for debug, input $make FLAG="-g -D DBG"
FLAG = 

# Compiler options
CC = gcc
WARN = -Wall
OPTIMIZER = -O3
CFLAGS = $(WARN) -c $(FLAG) $(OPTIMIZER)
LFLAGS = $(WARN) $(FLAG) $(OPTIMIZER)

# default rule
all: $(PROG)
	@echo "my work is done here..."

# rule for making sim_bp
$(PROG): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $@
	@echo "-----------DONE WITH SIM_BP-----------"

# generic rule for converting any .c file to any .o file
.c.o:
	$(CC) $(CFLAGS) $< -o $@

# type "make clean" to remove all .o files plus the sim_cache binary
clean:
	$(RM) $(CLEANFILES)

# type "make clobber" to remove all .o files (leaves sim_cache binary)
clobber:
	$(RM) $(OBJS)
