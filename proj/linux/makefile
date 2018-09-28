####################################################################################################
.PHONY: all clean
####################################################################################################
CXX 				= g++
CC 					= gcc
LD 					= ld
AR 					= ar
####################################################################################################
# DEFS
DEFS	+=
ifeq ($(shell getconf LONG_BIT), 32)
	DEFS += -DEYE_PTR_SIZE=4
else
	DEFS += -DEYE_PTR_SIZE=8
endif
####################################################################################################
# include path
CFLAGS				+= $(IncludePath)
CXXFLAGS			+= $(IncludePath)
CFLAGS				+= $(DEFS)
CXXFLAGS			+= $(DEFS)
####################################################################################################
# lib path. -shared for shared lib
LDFLAGS				+=
LIBS				+=
####################################################################################################
# compiler options. -fPIC for shared lib
BuildOpts			:= -MMD -Wall -g -O3 -DEYE_LINUX
####################################################################################################
# link compiler. cxx for executable program. ld for lib shared. ar for lib static
Link				:= $(AR)
####################################################################################################
FlagOut				:= -rs
####################################################################################################
# project 
ProjectName			:= eye
####################################################################################################
# output
OutPutPrefix		:= lib
OutPutSuffix		:= .a
OutPutName			:= $(OutPutPrefix)$(ProjectName)$(OutPutSuffix)
OutPutDir			:= lib
####################################################################################################
# target
TargetName			:= $(OutPutName)
ObjDirPath			:= obj
####################################################################################################
# src path filter & filter out
SourcePath			+= ../../src
SourceFilter		:= %.cpp %.c %.cc
SourceFilterOut		+= 
####################################################################################################
# include file path
IncludePath			+= -I../../src/core
IncludePath			+= -I../../src/event
IncludePath			+= -I../../src/os/unix
IncludePath			+= -I../../src/net
####################################################################################################
# traversal all files. pattern $2
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
srcs=$(foreach src, $(SourcePath),\
$(subst $(dir $(src)),, $(filter-out $(addsuffix %,$(SourceFilterOut)),\
$(filter $(SourceFilter), $(call rwildcard,$(src),*.*)))))
####################################################################################################
# objs
objs:=$(addprefix $(ObjDirPath)/, $(addsuffix .o,$(basename $(srcs))))
####################################################################################################
all:$(TargetName)
####################################################################################################
# DEPTS
DEPTS:=$(patsubst %.o, %.d, $(objs))
-include $(DEPTS)
####################################################################################################
SourceDirPath := $(dir $(SourcePath))
VPATH += $(SourceDirPath)
####################################################################################################
$(TargetName):$(objs)
	@echo "ld $@ <= $(OutPutDir)/$@"
	@if [ ! -d $(dir $(OutPutDir)/) ]; then mkdir -p $(dir $(OutPutDir)/);fi
	@$(Link) $(FlagOut) $(OutPutDir)/$@ $^ $(LDFLAGS) $(LIBS)
####################################################################################################
$(ObjDirPath)/%.o: %.cpp
	@echo "cxx $@ <= $<"
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@);fi
	@$(CXX) -c $< -o $@ $(CXXFLAGS) $(BuildOpts)
####################################################################################################
$(ObjDirPath)/%.o:%.c
	@echo "cc $@ <= $<"
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@);fi
	@$(CC) -c $< -o $@ $(CFLAGS) $(BuildOpts)
####################################################################################################
$(ObjDirPath)/%.o:%.cc
	@echo "cxx $@ <= $<"
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@);fi
	@$(CXX) -c $< -o $@ $(CXXFLAGS) $(BuildOpts)

####################################################################################################
clean:
	@-rm $(objs) $(DEPTS) $(addprefix $(OutPutDir)/,$(TargetName)) -f