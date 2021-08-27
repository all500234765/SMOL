TARGET = smol_test
OBJS = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJDIR = ./Junk
BUILD = ./Build

BUILD_PRX = 1
PSP_FW_VERSION = 371

#PCH_SRC = pc.h
INCDIR =
CFLAGS = -O2 -G0 -MP -MMD -D__PSP__
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -std=c++11
ASFLAGS = $(CFLAGS)


CFLAGS += -gdwarf-2


NEW_OBJS := $(filter-out $(patsubst %.h,%.cpp,$(PCH_SRC)),$(OBJS))
SOURCES  := $(NEW_OBJS)

#OBJS     := $(NEW_OBJS:%.cpp=%.o)
#NEW_OBJS = $(OBJS:%.c=%.o)
OBJS     = $(NEW_OBJS)

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

$(info Sources: $(SOURCES))
$(info Objs: $(OBJS))

PSPSDK=$(shell psp-config --pspsdk-path)
include build.mak

#OBJS += $(PSPSDK)/lib.o
#$(info MOAR AWWAWAWAWWAWAWWAAWAW=$(filter $(PSPSDK)%,$(OBJS)) $(addprefix $(OBJDIR)/,$(filter-out $(PSPSDK)%,$(OBJS))))
