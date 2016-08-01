# Copyright (c) 2013, Tencent Inc.
# All rights reserved.
#
# Author:jason  <jasonyjiang@tencent.com>
# Created: 2015-06-12

##=========================================================================
SRCDIRS   = .
PROGRAM   = ../../bin/so_check
OBJDIRS   = /tmp/tnf/tools/so_check/obj/
LDDIRS	  = -L. -L../lib
INCDIR	  =
LDFLAGS   = -ldl
#CFLAGS	  = -g -Wall -fPIC -O2 -pg
CFLAGS	  = -g -Wall -fPIC

##==========================================================================
CC         = gcc
CXX        = g++
RM         = rm -rf

##==========================================================================
SRCEXTS    = .c .C .cc .cpp .CPP .c++ .cxx .cp
HDREXTS    = .h .H .hh .hpp .HPP .h++ .hxx .hp

SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
HEADERS = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(HDREXTS))))
OBJS    = $(addprefix $(OBJDIRS), $(addsuffix .o, $(basename $(SOURCES))))
DEPS    = $(addsuffix .d, $(basename $(SOURCES)))
ODIRS   = $(foreach src, $(SRCDIRS),$(OBJDIRS)/$(src))

.PHONY: all objs tags ctags clean distclean help show mkdir
.SUFFIXES:

define mkdir_obj
endef

all: mkdir $(PROGRAM)

$(OBJDIRS)%.o:%.cpp
	@$(CXX) $(INCDIR) $(CFLAGS) -c $< -o $@
	@echo -e "\\033[1;40;32mCC $< -o $@ \\033[m";
$(OBJDIRS)%.o:%.cc
	@$(CXX) $(INCDIR) $(CFLAGS) -c $< -o $@
	@echo -e "\\033[1;40;32mCC $< -o $@ \\033[m";

$(PROGRAM): $(OBJS)
	@$(CXX) $(OBJS) $(LDDIRS) $(LDFLAGS) -o $@ $(LDFLAGS)
	@echo -e "\\033[1;42;33mLink Program $@ ok\\033[m";

mkdir:
	@mkdir -p $(ODIRS);

clean:
	@$(RM) $(OBJS) $(DEPS) $(PROGRAM)
	@$(RM) $(OBJDIRS)

show:
	@echo 'PROGRAM     :' $(PROGRAM)
	@echo 'SRCDIRS     :' $(SRCDIRS)
	@echo 'HEADERS     :' $(HEADERS)
	@echo 'SOURCES     :' $(SOURCES)
	@echo 'OBJS        :' $(OBJS)
	@echo 'DEPS        :' $(DEPS)

install:
	#install $(PROGRAM) /cfs_sz/preprocess/public/bin/
	install $(PROGRAM) /cfs_sh/preprocess/public/bin/

##############################################################
