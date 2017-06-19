# -*- Makefile -*-


#---------------------------------------------------------------------
#  Created:            Friday, September 11, 2013
#  Original author:    Alexander Samoilov
#---------------------------------------------------------------------

#
#   Makefile for body_ax mini application derived from Pozrikidis' BEMLIB http://dehesa.freeshell.org/BEMLIB/
#

PWD   = $(shell pwd)
MKDIR = mkdir -p
SDIR  = $(PWD)/src
GOLDEN_DIR  = $(PWD)/golden
ODIR_BASE  = obj

SUFFIXES += .dep
NODEPS:=clean tags

DBG ?=0

#EIGEN_INCL = /usr/local/include/eigen3
EIGEN_INCL = /usr/include/eigen3

INCLUDES := -I$(SDIR)/ -I$(EIGEN_INCL)
LDLIBS = -lboost_program_options

CXX=g++
CXX11_FLAGS = -std=c++1z
DEBUG_CXXFLAGS = $(CXX11_FLAGS) -g -O0 $(INCLUDES)
RELEASE_CXXFLAGS = $(CXX11_FLAGS) -g -O2 -DNDEBUG=1 $(INCLUDES)


ifeq ($(DBG), 1)
    ODIR     = $(ODIR_BASE).debug
    CXXFLAGS = $(DEBUG_CXXFLAGS)
else
    ODIR     = $(ODIR_BASE).release
    CXXFLAGS = $(RELEASE_CXXFLAGS)
endif

DEPDIR = $(ODIR)

CPP_SOURCES  += $(wildcard $(SDIR)/*.cpp)
SOURCES      += $(CPP_SOURCES)

BASE_SOURCES += $(basename $(notdir $(SOURCES)))
OBJS         += $(BASE_SOURCES:%=$(ODIR)/%.o)
DEPS         += $(BASE_SOURCES:%=$(DEPDIR)/%.dep)

TARGET = $(ODIR)/body_ax

$(DEPDIR)/%.dep: $(SDIR)/%.cpp
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -MM -MT '$(ODIR)/$(basename $(notdir $<)).o' $< -MF $@

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPDIR)/%.dep
	@echo Compiling $(<F)
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

all: nice_pic
	
nice_pic: $(TARGET)

gl_print:
	$(CXX) $(CXXFLAGS) -DPRINT_ME $(SDIR)/gauss_legendre.cpp -o gauss_legendre_print 
	./gauss_legendre_print > gauss_legendre_print.out
	diff -u $(GOLDEN_DIR)/gauss_legendre_print.golden gauss_legendre_print.out

ell_print: $(SDIR)/elliptic_integral.cpp
	$(CXX) $(CXXFLAGS) -DPRINT_ME $(SDIR)/elliptic_integral.cpp -o ell_print 
	./ell_print > ell_print.out

gl_unit_test:
	$(CXX) -pthread $(CXXFLAGS) -DUNIT_TEST $(SDIR)/gauss_legendre.cpp -lgtest -o gauss_legendre_unit_test
	./gauss_legendre_unit_test

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDLIBS) -o $@ 

sample_run:
	obj.debug/body_ax --flow_type 50 --input_data data/sphere.dat --verbose
	obj.debug/body_ax --flow_type 51 --input_data data/torus_trgl.dat --verbose

nice_pic:
	obj.release/body_ax --flow_type 51 --input_data data/torus_trgl.dat.BIG
	asy -f pdf body_ax.asy

profile:
	valgrind --tool=callgrind obj.release/body_ax --flow_type 51 --input_data data/torus_trgl.dat.BIG

docs:
	#doxygen Doxyfile
	pandoc --include-in-header=fontoptions.tex -s README.md --highlight-style pygments --latex-engine=xelatex -o README.pdf


.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(DEPS)
endif
