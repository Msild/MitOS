#
#	File: Source/method/Makefile
#	Copyright: MLworkshop
#	Author: Msild
#

TARGET    = alloca.obj asmfunc.obj crc32.obj dsctbl.obj fifo.obj memory.obj \
            mtask.obj time.obj

BUILDPATH =
INCPATH   = ../include/

MAKE      = $(BUILDPATH)make.exe -r
NASK      = $(BUILDPATH)nask.exe
NASM      = $(BUILDPATH)nasm.exe -fcoff
MCC       = $(BUILDPATH)mcc.exe -I$(INCPATH) -Os -Wall -quiet
CONVNAS   = $(BUILDPATH)convnas.exe -a
ATTRIB    = attrib
COPY      = copy
DEL       = del

# 默认动作

.PHONY : default
default :
	$(MAKE) $(TARGET)

# 一般规则

%.gas : %.c ../MitOS.h Makefile
	$(MCC) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(CONVNAS) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

%.obj : %.asm Makefile
	$(NASM) $*.asm -o$*.obj -l$*.lst

# 命令

.PHONY : clean
clean :
	-$(DEL)    *.lst

.PHONY : mrproper
mrproper :
	$(MAKE) clean
	-$(DEL) *.obj
