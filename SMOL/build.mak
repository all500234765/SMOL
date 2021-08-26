# PSP Software Development Kit - http://www.pspdev.org
# -----------------------------------------------------------------------
# Licensed under the BSD license, see LICENSE in PSPSDK root for details.
#
# build.mak - Base makefile for projects using PSPSDK.
#
# Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
# Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
# Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
#
# $Id: build.mak 2333 2007-10-31 19:37:40Z tyranid $

# Note: The PSPSDK make variable must be defined before this file is included.
ifeq ($(PSPSDK),)
$(error $$(PSPSDK) is undefined.  Use "PSPSDK := $$(shell psp-config --pspsdk-path)" in your Makefile)
endif

CC       = psp-gcc
CXX      = psp-g++
GDC      = psp-gdc
AS       = psp-gcc
LD       = psp-gcc
AR       = psp-ar
RANLIB   = psp-ranlib
STRIP    = psp-strip
MKSFO    = mksfo
PACK_PBP = pack-pbp
FIXUP    = psp-fixup-imports
ENC		 = PrxEncrypter

# Add in PSPSDK includes and libraries.
INCDIR   := $(INCDIR) . $(PSPSDK)/include
LIBDIR   := $(LIBDIR) . $(PSPSDK)/lib

CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)

ifeq ($(PSP_LARGE_MEMORY),1)
MKSFO = mksfoex -d MEMSIZE=1
endif

ifeq ($(PSP_FW_VERSION),)
PSP_FW_VERSION=150
endif

CFLAGS += -D_PSP_FW_VERSION=$(PSP_FW_VERSION)
CXXFLAGS += -D_PSP_FW_VERSION=$(PSP_FW_VERSION)

ifeq ($(OBJDIR),)
OBJDIR = .
else
$(shell mkdir -p "$(OBJDIR)")
endif

ifeq ($(BUILD),)
BUILD = .
else
$(shell mkdir -p "$(BUILD)")
endif

CXXFLAGS_EXTRA := 
CFLAGS_EXTRA := 

ifeq ($(PCH_SRC),)
PCH_OUT := 
else
PCH_OUT := $(PCH_SRC).gch
CXXFLAGS_EXTRA += -H -include $(patsubst %.cpp,%.h,$(PCH_SRC))
CFLAGS_EXTRA += -H -include $(patsubst %.cpp,%.h,$(PCH_SRC))
endif

#OBJS     := $(OBJDIR) . $(OBJS)

# Objective-C selection. All Objective C code must be linked against libobjc.a
ifeq ($(USE_OBJC),1)
LIBS     := $(LIBS) -lobjc
endif

ifeq ($(BUILD_PRX),1)
LDFLAGS  := $(addprefix -L,$(LIBDIR)) -specs=$(PSPSDK)/lib/prxspecs -Wl,-q,-T$(PSPSDK)/lib/linkfile.prx $(LDFLAGS)
EXTRA_CLEAN += $(TARGET).elf
# Setup default exports if needed
ifdef PRX_EXPORTS
EXPORT_OBJ=$(patsubst %.exp,%.o,$(PRX_EXPORTS))
EXTRA_CLEAN += $(EXPORT_OBJ)
else 
EXPORT_OBJ=$(PSPSDK)/lib/prxexports.o
endif
else
LDFLAGS  := $(addprefix -L,$(LIBDIR)) $(LDFLAGS)
endif

# Library selection.  By default we link with Newlib's libc.  Allow the
# user to link with PSPSDK's libc if USE_PSPSDK_LIBC is set to 1.

ifeq ($(USE_KERNEL_LIBC),1)
# Use the PSP's kernel libc
PSPSDK_LIBC_LIB = 
CFLAGS := -I$(PSPSDK)/include/libc $(CFLAGS)
else
ifeq ($(USE_PSPSDK_LIBC),1)
# Use the pspsdk libc
PSPSDK_LIBC_LIB = -lpsplibc
CFLAGS := -I$(PSPSDK)/include/libc $(CFLAGS)
else
# Use newlib (urgh)
PSPSDK_LIBC_LIB = -lc
endif
endif


# Link with following default libraries.  Other libraries should be specified in the $(LIBS) variable.
# TODO: This library list needs to be generated at configure time.
#
ifeq ($(USE_KERNEL_LIBS),1)
PSPSDK_LIBS = -lpspdebug -lpspdisplay_driver -lpspctrl_driver -lpspsdk
LIBS     := $(LIBS) $(PSPSDK_LIBS) $(PSPSDK_LIBC_LIB) -lpspkernel
else
ifeq ($(USE_USER_LIBS),1)
PSPSDK_LIBS = -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk
LIBS     := $(LIBS) $(PSPSDK_LIBS) $(PSPSDK_LIBC_LIB) -lpspnet \
			-lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility \
			-lpspuser
else
PSPSDK_LIBS = -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk
LIBS     := $(LIBS) $(PSPSDK_LIBS) $(PSPSDK_LIBC_LIB) -lpspnet \
			-lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility \
			-lpspuser -lpspkernel
endif
endif

# Define the overridable parameters for EBOOT.PBP
ifndef PSP_EBOOT_TITLE
PSP_EBOOT_TITLE = $(TARGET)
endif

ifndef PSP_EBOOT_SFO
PSP_EBOOT_SFO = PARAM.SFO
endif

ifndef PSP_EBOOT_ICON
PSP_EBOOT_ICON = NULL
endif

ifndef PSP_EBOOT_ICON1
PSP_EBOOT_ICON1 = NULL
endif

ifndef PSP_EBOOT_UNKPNG
PSP_EBOOT_UNKPNG = NULL
endif

ifndef PSP_EBOOT_PIC1
PSP_EBOOT_PIC1 = NULL
endif

ifndef PSP_EBOOT_SND0
PSP_EBOOT_SND0 = NULL
endif

ifndef PSP_EBOOT_PSAR
PSP_EBOOT_PSAR = NULL
endif

ifndef PSP_EBOOT
PSP_EBOOT = EBOOT.PBP
endif

ifeq ($(BUILD_PRX),1)
ifneq ($(TARGET_LIB),)
$(error TARGET_LIB should not be defined when building a prx)
else
FINAL_TARGET = $(BUILD)/$(TARGET).prx
endif
else
ifneq ($(TARGET_LIB),)
FINAL_TARGET = $(TARGET_LIB)
else
FINAL_TARGET = $(OBJDIR)/$(TARGET).elf
endif
endif

all: $(PCH_OUT) $(EXTRA_TARGETS) $(FINAL_TARGET)

kxploit: $(OBJDIR)/$(TARGET).elf $(BUILD)/$(PSP_EBOOT_SFO)
	mkdir -p "$(TARGET)"
	$(STRIP) $(OBJDIR)/$(TARGET).elf -o $(BUILD)/$(TARGET)/$(PSP_EBOOT)
	mkdir -p "$(TARGET)%"
	$(PACK_PBP) "$(BUILD)/$(TARGET)%/$(PSP_EBOOT)" $(BUILD)/$(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0) NULL $(PSP_EBOOT_PSAR)

SCEkxploit: $(OBJDIR)/$(TARGET).elf $(BUILD)/$(PSP_EBOOT_SFO)
	mkdir -p "$(OBJDIR)/__SCE__$(TARGET)"
	$(STRIP) $(OBJDIR)/$(TARGET).elf -o $(BUILD)/__SCE__$(TARGET)/$(PSP_EBOOT)
	mkdir -p "$(OBJDIR)/%__SCE__$(TARGET)"
	$(PACK_PBP) "$(BUILD)/%__SCE__$(TARGET)/$(PSP_EBOOT)" $(BUILD)/$(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0) NULL $(PSP_EBOOT_PSAR)

ifeq ($(NO_FIXUP_IMPORTS), 1)
$(OBJDIR)/$(TARGET).elf: $(OBJS) $(EXPORT_OBJ)
	$(LINK.c) $^ $(CFLAGS_EXTRA) $(LIBS) -o $@
else
$(OBJDIR)/$(TARGET).elf: $(OBJS) $(EXPORT_OBJ)
	$(LINK.c) $^ $(CFLAGS_EXTRA) $(LIBS) -o $@
	$(FIXUP) $@
endif

$(TARGET_LIB): $(OBJS)
	$(AR) cru $@ $(OBJDIR)/$(OBJS)
	$(RANLIB) $(OBJDIR)/$@

$(BUILD)/$(PSP_EBOOT_SFO): 
	$(MKSFO) '$(PSP_EBOOT_TITLE)' $@

ifeq ($(BUILD_PRX),1)
$(PSP_EBOOT): $(BUILD)/$(TARGET).prx $(BUILD)/$(PSP_EBOOT_SFO)
ifeq ($(ENCRYPT), 1)
	- $(ENC) $(BUILD)/$(TARGET).prx $(BUILD)/$(TARGET).prx
endif
	$(PACK_PBP) $(BUILD)/$(PSP_EBOOT) $(BUILD)/$(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0)  $(BUILD)/$(TARGET).prx $(PSP_EBOOT_PSAR)
else
$(PSP_EBOOT): $(OBJDIR)/$(TARGET).elf $(BUILD)/$(PSP_EBOOT_SFO)
	$(STRIP) $(OBJDIR)/$(TARGET).elf -o $(OBJDIR)/$(TARGET)_strip.elf
	$(PACK_PBP) $(BUILD)/$(PSP_EBOOT) $(BUILD)/$(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0)  $(OBJDIR)/$(TARGET)_strip.elf $(PSP_EBOOT_PSAR)
	-rm -f $(OBJDIR)/$(TARGET)_strip.elf
endif

$(BUILD)/%.prx: $(OBJDIR)/%.elf
	psp-prxgen $< $@

%.c: %.exp
	psp-build-exports -b $< > $(OBJDIR)/$@

#%.o: %.m
#	$(CC) $(CFLAGS_EXTRA) $(CFLAGS) -c -o $@ $(OBJDIR)/$<

#%.o: %.mm
#	$(CXX) $(CXXFLAGS_EXTRA) $(CXXFLAGS) -c -o $@ $(OBJDIR)/$<

%.cpp: %.d
	$(CXX) $(CXXFLAGS_EXTRA) $(CXXFLAGS) -c -o $@ $(OBJDIR)/$<
	$(GDC) $(DFLAGS) -c $(OBJDIR)/$< -o $(OBJDIR)/$<.d

%.o: %.d
	$(GDC) $(DFLAGS) -c $@ -o $(OBJDIR)/$<

%.cpp.gch:
	$(CXX) -x c++-header $(CXXFLAGS) -c $(patsubst %.gch,%,$@) -o $@

%.h.gch:
	$(CXX) $(CFLAGS) -c $(patsubst %.h.gch,%.h,$@) -o $@
#-IC:/pspsdk_15/psp/include/c++/8.2.0/psp/ -IC:/pspsdk_15/psp/include/c++/8.2.0/ 

clean: 
	-rm -f $(FINAL_TARGET) $(EXTRA_CLEAN) $(OBJDIR)/$(OBJS) $(BUILD)/$(PSP_EBOOT_SFO) $(BUILD)/$(PSP_EBOOT) $(EXTRA_TARGETS) $(PCH_OUT)

rebuild: clean all
