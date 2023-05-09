#Makefile for bare metal ARM development on Freedom FRDM-KL25Z
#Douglas Summerville, Binghamton University, 2018
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
OBJSIZE = arm-none-eabi-size
INCLUDES = -Idrivers -Ibaremetal
VPATH = src:drivers:baremetal
SYS_CLOCK = 48000000L
-include config.make

LINKSCRIPT=baremetal/mkl25z4.ld 

OPT_LEVEL = -Og
#OPT_LEVEL = -Os -flto
OPTS = $(OPT_LEVEL) "-DSYS_CLOCK=$(SYS_CLOCK)" $(USER_MACRO)
DEBUG_OPTS = -g3
TARGET = cortex-m0plus
CFLAGS = -ffreestanding -nodefaultlibs -nostartfiles \
	 -ffunction-sections -fdata-sections -Wall \
	 -fmessage-length=0 -mcpu=$(TARGET) -mthumb -mfloat-abi=soft \
	 $(DEBUG_OPTS) $(OPTS) $(INCLUDES)
CFLAGSS = -ffreestanding -nodefaultlibs -nostartfiles \
	 -ffunction-sections -fdata-sections -Wall \
	 -fmessage-length=0 -mcpu=$(TARGET) -mthumb -mfloat-abi=soft \
	 $(DEBUG_OPTS) $(OPTS) $(INCLUDES)
NUM_ELFS := $(shell  ls -dq *.elf 2>/dev/null | wc -l)

.PHONY:	clean usage board_plugged_in program

# -----------------------------------------------------------------------------

usage: 
	@clear
	@cat USAGE.txt

board_plugged_in:
ifeq ("$(wildcard /media/*/DAPLINK)","")
	$(error DAPLINK not found.  Is the board plugged in?)
else
	@echo DAPLINK found
endif

program: board_plugged_in
ifeq ($(NUM_ELFS),0)
	$(error No executable .elf file exists in the current directory)
else
ifeq ($(NUM_ELFS),1)
	openocd  -f interface/cmsis-dap.cfg -f target/kl25.cfg -c "init" \
		-c "program $(wildcard *.elf)" -c "reset" -c "exit"
else
	$(error There is more than one executable .elf file in the current \
		directory)  
endif
endif

erase: board_plugged_in
	openocd  -f interface/cmsis-dap.cfg -f target/kl25.cfg -c "init" -c "kinetis mdm mass_erase" -c "exit"

clean:
	-rm -f *.o *.elf *.srec  *.dump

_startup.o: _startup.c
	$(CC) $(CFLAGSS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.dump: %.elf
	$(OBJDUMP) --disassemble $< >$@

%.srec: %.elf
	$(OBJCOPY) -O srec $< $@

#_startup.o must be first in link order- else LTO removes IRQ Handlers
%.elf: _startup.o %.o $(LIBS)
	$(CC) $(CFLAGS) -T $(LINKSCRIPT) -o $@ $^
	#@echo Generated Program has the following segment sizes:
	@$(OBJSIZE) $@

