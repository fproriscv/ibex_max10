# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0
#
# Generate a baremetal application

PROGRAM ?= led
PROGRAM_CFLAGS = -Wall -g -Os
ARCH = rv32imc

BUILD = build

# name of the Quartus program
FPGA_PROJECT_PATH = ../quartus
FPGA_PROJECT = top_de10_lite

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

OBJS := $(SRCS:%.c=build/%.o) #${CRT:.S=.o}
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

OUTFILES = $(BUILD)/$(PROGRAM).elf $(BUILD)/$(PROGRAM).bin $(BUILD)/$(PROGRAM).txt \
					 $(BUILD)/$(PROGRAM).hex $(BUILD)/$(PROGRAM).vmem $(BUILD)/$(PROGRAM).bin \
					 $(BUILD)/$(PROGRAM).dis

all: dirs $(OUTFILES)

# $@ -- refers to target
# #< -- refers to dependency

$(BUILD)/$(PROGRAM).elf: $(OBJS) $(LINKER_SCRIPT)
	$(CC) $(CFLAGS) -T $(LINKER_SCRIPT) $(OBJS) -o $@ $(LIBS)

$(BUILD)/%.dis: $(BUILD)/%.elf
	$(OBJDUMP) -SD $^ > $@

# Note: this target requires the srecord package to be installed.
# XXX: This could be replaced by objcopy once
# https://sourceware.org/bugzilla/show_bug.cgi?id=19921
# is widely available.
# XXX: Currently the start address 0x00000000 is hardcoded. It could/should be
# read from the elf file, but is lost in the bin file.
# Switching to objcopy will resolve that as well.
$(BUILD)/%.vmem: $(BUILD)/%.bin
	srec_cat $^ -binary -offset 0x0000 -byte-swap 4 -o $@ -vmem

$(BUILD)/%.bin: $(BUILD)/%.elf
	$(OBJCOPY) -O binary $^ $@

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

#$(BUILD)/%.o: %.cpp %.h
#	$(CC) -c -o $@ $<

$(BUILD)/%.o: %.S
	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

clean:
	$(RM) -f *.o *.d

distclean: clean
	$(RM) -rf $(BUILD)

# extract the binary data from data and text sections to get a binary image of memory
#mem.bin: $(PROGRAM).elf
#	$(OBJCOPY) -O binary --only-section=.data* --only-section=.text* $< $@

# convert to an ASCII hex file for simulation
$(BUILD)/%.txt: $(BUILD)/%.bin
	hexdump -v -e '"%08x\n"' $< > $@

# convert to an Intel HEX file for downloading to the FPGA
$(BUILD)/%.hex: $(BUILD)/%.txt
	python txt2hex.py $< $@ 4

dirs:
	mkdir -p $(BUILD)

# update the memory files inside the FPGA bitfile
update-mem:	all
	cd ${FPGA_PROJECT_PATH} && quartus_cdb ${FPGA_PROJECT} -c ${FPGA_PROJECT} --update_mif
	cd ${FPGA_PROJECT_PATH} && quartus_asm --read_settings_files=on --write_settings_files=off ${FPGA_PROJECT} -c ${FPGA_PROJECT}

# download the bitfile to your board
download:
	cd ${FPGA_PROJECT_PATH} && quartus_pgm -m jtag -o P\;output_files/${FPGA_PROJECT}.sof@2

# build the whole FPGA from the command line
# not that it's harder to spot warnings in this output compared with the GUI
build_fpga:
	cd ${FPGA_PROJECT_PATH} && quartus_sh --flow compile ${FPGA_PROJECT}
