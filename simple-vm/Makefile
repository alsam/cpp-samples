# -*- Makefile -*-


#---------------------------------------------------------------------
#  Created:            Monday, May 17, 2010
#  Original author:    Alexander Samoilov
#---------------------------------------------------------------------

#
#   Brief file description here
#

PWD   = $(shell pwd)
SDIR  = $(PWD)
ODIR  = $(SDIR)

SOURCES += $(wildcard $(SDIR)/*.cpp)
BASE_SOURCES += $(basename $(notdir $(SOURCES)))

# bison & flex combination
LEXERS = $(wildcard *.ll)
GRAMMARS = $(wildcard *.yy)
LEX_SOURCES += $(LEXERS:%.ll=$(SDIR)/%.cpp)
GRAMMAR_SOURCES += $(GRAMMARS:%.yy=$(SDIR)/%.cpp)

SOURCES += $(LEX_SOURCES)
SOURCES += $(GRAMMAR_SOURCES)

OBJS += $(BASE_SOURCES:%=$(ODIR)/%.o)
DEPS += $(BASE_SOURCES:%=$(ODIR)/%.d)

MY_ROOT = $(PWD)/../../

TARGET = test1

TASK1_IN = $(MY_ROOT)/task1.bin
TASK1_OUT = $(PWD)/task1.OUT

TASK2_IN = $(MY_ROOT)/task2.bin
TASK2_CMDS = $(PWD)/task2.cmds
TASK2_OUT = $(PWD)/task2.xml

$(ODIR)/%.o : %.cpp
	@echo Compiling $(<F)
	@$(CXX) -o $@ -c $< $(CXXFLAGS)

$(ODIR)/%.d : %.cpp
	@$(CXX) -MM $< $(CXXFLAGS) -MT $(@:.d=.o) -MF $@

$(SDIR)/%.cpp : %.ll
	@echo Generating $@ from lex file  $(<F)
	$(LEX) -o$@ $<

$(SDIR)/%.cpp : %.yy
	@echo Generating $@ from bison file  $(<F)
	$(BISON) -o $@ $<

CC = gcc
CXX = g++
CFLAGS = -g  $(DEFINES) $(INCLUDES)
ifeq ($(LIB),SHARED)
CFLAGS += -fPIC
endif

CXXFLAGS = $(CFLAGS) -Wno-deprecated

LEX = flex
LEX_FLAGS = 

BISON = bison
BISON_FLAGS = 

all: exec

.PHONY: all exec test tags

init:
	@mkdir -p $(ODIR)

dep: $(SOURCES)
	@for x in $(SOURCES); do\
		echo "Generating dependencies from $$x";\
		bname=`basename $$x .cpp`;\
		$(CXX) -MM $$x $(CXXFLAGS) -MT $(ODIR)/$$bname.o -MF $(ODIR)/$$bname.d;\
	done

exec: $(TARGET)

$(TARGET): $(OBJS)
	@echo Creating $(@F)
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

test:
	@./test_all

task1:
	@./$(TARGET) $(TASK1_IN) > $(TASK1_OUT)

task2:
	@./draw.pl > $(TASK2_CMDS)
	@./$(TARGET) $(TASK2_IN) < $(TASK2_CMDS) > $(TASK2_OUT)

tags:
	@ctags -R $(SOURCES)
	@ctags -a -R $(HEADERS_FOR_TAGS)

clean:
	@echo Cleaning...
	$(RM) $(ODIR)/*.o $(ODIR)/*.d $(LEX_SOURCES) $(GRAMMAR_SOURCES) $(TARGET) $(TASK1_OUT) tags

ifneq ($(MAKECMDGOALS),init)
-include $(DEPS)
endif
