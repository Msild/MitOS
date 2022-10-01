#
#	File: Source/lib/Makefile
#	Copyright: MLworkshop
#	Author: Msild
#

TARGET    = stdlib.lib

BUILDPATH = ../../../Tool/build/
INCPATH   = ../include/

MAKE      = $(BUILDPATH)make.exe -r
NASK      = $(BUILDPATH)nask.exe
NASM       = $(BUILDPATH)nasm.exe -fcoff
MCC       = $(BUILDPATH)mcc.exe -I$(INCPATH) -I../ -Os -Wall -quiet
GAS2NASK  = $(BUILDPATH)gas2nask.exe -a
GOLIB     = $(BUILDPATH)golib00.exe
COPY      = copy
DEL       = del

OBJS = abs.obj srand.obj memchr.obj memcmp.obj memmove.obj memset.obj rand.obj \
       sprintf.obj strcat.obj strcmp.obj strcpy.obj strcspn.obj strdup.obj \
       strlen.obj strncat.obj strncmp.obj strncpy.obj strpbrk.obj strrchr.obj \
       strspn.obj strstr.obj strtol.obj strtoul.obj strtoul0.obj vsprintf.obj \
       strtok.obj strlwr.obj strupr.obj errno.obj

# 默认动作

.PHONY : default
default :
	$(MAKE) $(TARGET)

# 文件生成规则
		
$(TARGET) : Makefile $(OBJS)
	$(GOLIB) $(OBJS) out:$(TARGET)

# 一般规则

%.gas : %.c Makefile
	$(MCC) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

%.obj : %.asm Makefile
	$(NASM) $*.asm -o$*.obj -l$*.lst

# 命令

.PHONY : clean
clean :
	-$(DEL) *.lst
	-$(DEL) *.obj

.PHONY : mrproper
mrproper :
	$(MAKE) clean
	-$(DEL) $(TARGET)
