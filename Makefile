# 
# MIT License
#
# Copyright (c) 2024 Emrah Karagoz, Pakize Sanal, Abhraneel Dutta, Edoardo Persichetti
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

#--------------------------------------------------------------------------------
# How to use MakeFile 
#--------------------------------------------------------------------------------
# make test_speed EXT_DEG=24781 INVERSE_METHOD=TYT GF2X_POLYMUL=0


#--------------------------------------------------------------------------------
# Compiler configuration
#--------------------------------------------------------------------------------
CC = /usr/bin/gcc
CFLAGS = -march=native -O3 -Wno-format
# CFLAGS += -Wall -g
# CFLAGS += -fsanitize=address

#-------------------------------
# Source Files
#-------------------------------
SRC  = gf2x_base.c 
SRC += gf2x_rand.c gf2x_print.c
SRC += gf2x_add.c gf2x_mul.c gf2x_sqr.c gf2x_red.c
SRC += bench.c

#--------------------------------------------------------------------------------
# Platform-specific Configuration 
#--------------------------------------------------------------------------------

# Determine the CPU architecture
ARCH := $(shell uname -m)

# Add -mpclmul flag if the CPU architecture is x86 or x86_64
ifeq ($(ARCH), i686)   # x86 32-bit
    CFLAGS += -mpclmul
endif
ifeq ($(ARCH), x86_64) # x86 64-bit
    CFLAGS += -mpclmul
endif

#--------------------------------------------------------------------------------
# PolynInv Flags
#--------------------------------------------------------------------------------

POLYINV_FLAGS = 

# Polynomial Extension Degree
EXT_DEG = 
POLYINV_FLAGS += -DEXT_DEG=$(EXT_DEG)

# Polynomial Inverse Method
INVERSE_METHOD = 
POLYINV_FLAGS += -DINVERSE_METHOD=$(INVERSE_METHOD)

# Static/Dynamic Polynomial Storage (BYI vs others)
ifeq ($(INVERSE_METHOD), BYI)
	SRC += gf2x_inv_byi.c
else
	POLYINV_FLAGS += -DUSE_STATIC_POLY
	SRC += gf2x_inv_flt.c
	SRC += gf2x_inv_cea.c
	SRC += gf2x_inv_tyt.c
	SRC += gf2x_inv_sac.c
endif

#--------------------------------------------------------------------------------
# Test the correctness of inversion algorithms
#--------------------------------------------------------------------------------
# TEST_INV_OUT = test_inv_P$(EXT_DEG)
TEST_INV_OUT := $(if $(findstring BYI,$(INVERSE_METHOD)),test_inv_P$(EXT_DEG)_BYI,test_inv_P$(EXT_DEG))
TEST_INV_RES = res_$(TEST_INV_OUT).txt

test_inv: test_inv.c
	@echo Compiling...
	@$(CC) $(CFLAGS) $(POLYINV_FLAGS) -o $(TEST_INV_OUT) $^ $(SRC)
	@echo Running... 
	@./$(TEST_INV_OUT)
	

#--------------------------------------------------------------------------------
# Test speed of inversion algorithms 
#--------------------------------------------------------------------------------
TEST_SPEED_OUT := $(if $(findstring BYI,$(INVERSE_METHOD)),test_speed_P$(EXT_DEG)_BYI,test_speed_P$(EXT_DEG))
TEST_SPEED_RES = res_$(TEST_SPEED_OUT).txt
test_speed: test_speed.c
	@echo Compiling...
	@$(CC) $(CFLAGS) $(POLYINV_FLAGS) -o $(TEST_SPEED_OUT) $^ $(SRC)
	@echo Running... 
	@./$(TEST_SPEED_OUT)


#--------------------------------------------------------------------------------
# Count the function calls in inversion algorithms 
#--------------------------------------------------------------------------------
TEST_CNT_OUT = test_cnt_P$(EXT_DEG)_$(INVERSE_METHOD)
test_count: test_count.c
	@$(CC) $(CFLAGS) $(POLYINV_FLAGS) -DTEST_COUNT=1 -o $(TEST_CNT_OUT) $^ $(SRC)

	
#--------------------------------------------------------------------------------
# Clean 
#--------------------------------------------------------------------------------
clean:
	@echo "Cleaning..."
	rm -f test_inv_P* test_speed_P* test_cnt_P*

.PHONY: clean
