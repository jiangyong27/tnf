# Copyright (c) 2013, Tencent Inc.
# All rights reserved.
#
# Author:jason  <jasonyjiang@tencent.com>
# Created: 2015-06-12

##=========================================================================
SRCDIRS   = .
INCDIRS	  = -I.
PROGRAM   = ./libtnf_jsoncpp.so
OBJDIRS   = ./obj/
LDFLAGS   = -lpthread
CFLAGS    = -g -O2 -fPIC

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
	@$(CXX) $(CFLAGS) $(INCDIRS) -c $< -o $@
	@echo -e "\\033[1;40;32mCC $< -o $@ \\033[m";

$(PROGRAM): $(OBJS)
	@$(CXX) $(LDFLAGS) $(OBJS) $(MY_LIBS) -o $@ -shared
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

##############################################################
