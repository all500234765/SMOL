TARGET = smol_test
OBJS = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJDIR = ./Junk

BUILD_PRX = 1
PSP_FW_VERSION = 371

PCH_SRC = pc.h
INCDIR =
CFLAGS = -gdwarf-2 -O2 -G0 -MP -MMD -D__PSP__
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -std=c++11
ASFLAGS = $(CFLAGS)

#-lstdc++ -lpng -lz \
#         -lpsphprm -lpspsdk -lpspctrl -lpspumd -lpsprtc -lpsppower -lpspgu -lpspgum  -lpspaudiolib -lpspaudio -lpsphttp -lpspssl -lpspwlan \
#         -lpspnet_adhocmatching -lpspnet_adhoc -lpspnet_adhocctl -lm -ljpeg

LIBDIR =
LIBS = -lstdc++ -lpng -lz \
         -lpsphprm -lpspsdk -lpspctrl -lpspumd -lpsprtc -lpsppower -lpspgu -lpspgum  -lpspaudiolib -lpspaudio -lpsphttp -lpspssl -lpspwlan \
         -lpspnet_adhocmatching -lpspnet_adhoc -lpspnet_adhocctl -lm -ljpeg

#-lstdc++ -lpspdisplay -lpspge -lpspctrl -lpspsdk -lpspgum -lpspgu -lm

LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = SMOL

# SRC = $(wildcard *.cpp)
#-include $(OBJS:.cpp=.o)

$(info Sources: $(OBJS))

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
