# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0
#
# Generate a baremetal application

PROGRAM ?= led
PROGRAM_CFLAGS = -Wall -g -Os
ARCH = rv32imc
# ARCH = rv32im # to disable compressed instructions
SRCS = $(PROGRAM).c init.c

CC = riscv32-unknown-elf-gcc
#CXX = riscv32-unknown-elf-cpp

OBJCOPY ?= $(subst gcc,objcopy,$(wordlist 1,1,$(CC)))
OBJDUMP ?= $(subst gcc,objdump,$(wordlist 1,1,$(CC)))

LINKER_SCRIPT ?= link.ld
#CRT ?= crt/crt0.S
CFLAGS ?= -march=$(ARCH) -mabi=ilp32 -static -mcmodel=medany \
	-fvisibility=hidden -nostdlib -nostartfiles $(PROGRAM_CFLAGS)

OBJS := ${SRCS:.c=.o} #${CRT:.S=.o}
DEPS = $(OBJS:%.o=%.d)

# provides the _init function header
#CRTI_OBJ=crti.o
# print and store the path to compiler provided crt*.o files
# they provide the _init function body
#CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
#CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
# provides the _init function footer (return statement)
#CRTN_OBJ=crtn.o

# order matters
#OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)
#INTERNAL_OBJS:=$(CRTI_OBJ) $(OBJS) $(CRTN_OBJ)

OUTFILES = $(PROGRAM).elf $(PROGRAM).bin $(PROGRAM).txt  $(PROGRAM).hex \
					 $(PROGRAM).vmem $(PROGRAM).bin $(PROGRAM).dis

all: $(OUTFILES)

# $@ -- refers to target
# #< -- refers to dependency

$(PROGRAM).elf: $(OBJS) $(LINKER_SCRIPT)
	$(CC) $(CFLAGS) -T $(LINKER_SCRIPT) $(OBJS) -o $@ $(LIBS)

%.dis: %.elf
	$(OBJDUMP) -SD $^ > $@

# Note: this target requires the srecord package to be installed.
# XXX: This could be replaced by objcopy once
# https://sourceware.org/bugzilla/show_bug.cgi?id=19921
# is widely available.
# XXX: Currently the start address 0x00000000 is hardcoded. It could/should be
# read from the elf file, but is lost in the bin file.
# Switching to objcopy will resolve that as well.
%.vmem: %.bin
	srec_cat $^ -binary -offset 0x0000 -byte-swap 4 -o $@ -vmem

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

%.o: %.cpp %.h
	$(CC) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

clean:
	$(RM) -f *.o *.d

distclean: clean
	$(RM) -f $(OUTFILES)

# extract the binary data from data and text sections to get a binary image of memory
#mem.bin: $(PROGRAM).elf
#	$(OBJCOPY) -O binary --only-section=.data* --only-section=.text* $< $@

# convert to an ASCII hex file for simulation
%.txt: %.bin
	hexdump -v -e '"%08x\n"' $< > $@

# convert to an Intel HEX file for downloading to the FPGA
%.hex: %.txt
	python txt2hex.py $< $@ 4
