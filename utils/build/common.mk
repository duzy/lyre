#
#  
#  OBJECTS:
#	Object (.o) files to be generated for the project.
#  
#  CLEAN:
#	Extra clean files (excluding $(OBJECTS)).
#
#
#

## FIXME: Using CURDIR instead?
PRJDIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

## High level predictors.
IS_MAKE_CLEAN := $(if $(findstring clean,$(MAKECMDGOALS)),y,)
IS_MAKE_TEST := $(if $(findstring test,$(MAKECMDGOALS)),y,)

ifeq ($(IS_MAKE_TEST),y)
  ifeq ($(word 1,$(MAKECMDGOALS)),test)
    TEST_GOAL_PREFIX :=
    TEST_GOAL_SUFFIX := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  else
    ## Doing bash substituations:
    ##     ${s/%test */}
    ##     ${s/#$(TEST_GOAL_PREFIX) test/}
    TEST_GOAL_PREFIX := $(shell bash -c 's="$(MAKECMDGOALS)"; echo $${s/%test */}')
    TEST_GOAL_SUFFIX := $(shell bash -c 's="$(MAKECMDGOALS)"; echo $${s/\#$(TEST_GOAL_PREFIX) test/}')
  endif
  MAKECMDGOALS := $(TEST_GOAL_PREFIX) test
endif # IS_MAKE_TEST == y

## High level comman variables
BOOST := $(wildcard /open/boost_1_59_0)
LLVM := $(wildcard /open/llvm)

## Dump build configs
ifeq (y,n)
$(info --------------------------------------------------)
$(info   CURDIR: $(CURDIR))
$(info   PRJDIR: $(PRJDIR))
$(info --------------------------------------------------)
endif

ifndef LLVM
  $(error lyre: you need 'llvm' at '/open/llvm')
endif

ifndef BOOST
  $(error lyre: you need 'boost 1.59' at '/open/boost_1_59_0')
endif

LLVM_BUILD := $(LLVM)/build
LLVM_CONFIG := $(LLVM_BUILD)/bin/llvm-config
LLVM_DIS := $(LLVM_BUILD)/bin/llvm-dis
LLVM_TBLGEN := $(LLVM_BUILD)/bin/llvm-tblgen
LLC := $(LLVM_BUILD)/bin/llc
LLI := $(LLVM_BUILD)/bin/lli

LLVM_INC := $(shell $(LLVM_CONFIG) --includedir)
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)

ARCHIVE = $(AR) $(ARFLAGS) $@ $^
COMBINE = $(LD) -r $^ -o $@
LINK = $(LINK.cc) $^ $(LOADLIBS) $(LIBS)

## Debug
ARFLAGS = rv

## Release
ARFLAGS = crs

CXXSTD := -std=c++11 -std=c++1y

CXXFLAGS = -g -ggdb
CXXFLAGS += $(LLVM_CXXFLAGS)

LOADLIBS = 

LIBS = \
  $(shell $(LLVM_CONFIG) --ldflags --libs $(LLVM_LIBS)) \
  -lpthread -ldl -lm -lz $(EXTRA_LIBS)

EXTRA_LIBS =

ifeq ($(shell pkg-config --exists tinfo && echo ok),ok)
  EXTRA_LIBS += -ltinfo
endif

OBJECTS =
CLEAN =

TESTFLAGS =

%.o: %.cpp
	$(COMPILE.cc) $< -o $@

%.d: %.cpp
	$(CXX) -MM -MF $@ -MT $(@:%.d=%.o) $(CXXFLAGS) $<

clean::
	@$(RM) -v $(OBJECTS) $(OBJECTS:%.o=%.d) $(CLEAN)

ifneq ($(wildcard $(CURDIR)/test/run),)
  test:: $(CURDIR)/test/run ; @cd $(<D) && bash $(<F) $(TESTFLAGS) $(TEST_GOAL_SUFFIX)
else
  test::; @echo No test defined (test/run).
endif

ifneq ($(IS_MAKE_CLEAN),y)
  -include $(OBJECTS:%.o=%.d)
endif # IS_MAKE_CLEAN != y

## Setup phony goals
.PHONY: clean test

## Reset the default goal
.DEFAULT_GOAL :=
