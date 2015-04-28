LYRE_USING_MCJIT := true

ifeq ($(LYRE_USING_MCJIT),true)
  #LLVMLIBS := core jit mcjit native irreader
  LLVMLIBS := core mcjit native
else
  LLVMLIBS := interpreter nativecodegen
endif

#$(info LLVMLIBS: $(LLVMLIBS))
#$(info ---------------------------)

LLVM := $(or $(wildcard /open/llvm),$(wildcard ~/Tools/llvm))
LLVM_CONFIG := $(LLVM)/Debug+Asserts/bin/llvm-config
LLVM_DIS := $(LLVM)/Debug+Asserts/bin/llvm-dis
LLI := $(LLVM)/Debug+Asserts/bin/lli

CXXFLAGS := -Isource \
  -DLYRE_USING_MCJIT=$(LYRE_USING_MCJIT) \
  $(shell $(LLVM_CONFIG) --cxxflags)

# -ltinfo
LIBS := \
  $(shell $(LLVM_CONFIG) --ldflags --libs $(LLVMLIBS)) \
  -lpthread -ldl -lm -lz

LOADLIBS := 

COMBINE = $(LD) -r -o $@ $^

OBJECTS = $(OBJECTS.lyre) $(OBJECTS.parse) $(OBJECTS.gc)

OBJECTS.lyre := \
  source/frontend.o \
  source/ast.o \
  source/parse.o \
  source/gc.o \

OBJECTS.frontend := \
  source/frontend/Compiler.o \
  source/frontend/main.o \

OBJECTS.ast := \
  source/ast/Stmt.o \
  source/ast/StmtList.o \
  source/ast/Expr.o \

OBJECTS.parse := \
  source/parse/metast.o \
  source/parse/convert.o \
  source/parse/parse.o \

OBJECTS.gc := \
  source/gc/lygc.o \

lyre: $(OBJECTS.lyre)
	$(LINK.cc) -o $@ $^ $(LOADLIBS) $(LIBS)

source/frontend.o: $(OBJECTS.frontend) ; $(COMBINE)
source/ast.o: $(OBJECTS.ast) ; $(COMBINE)
source/parse.o: $(OBJECTS.parse) ; $(COMBINE)
source/gc.o: $(OBJECTS.gc) ; $(COMBINE)

source/parse/parse.o: source/parse/parse.cpp
	$(CXX) -Isource -DLYRE_USING_MCJIT=$(LYRE_USING_MCJIT) -std=c++11 -fPIC -c $< -o $@

%.o: %.cpp
	$(COMPILE.cc) $< -o $@

%.d: %.cpp
	$(CXX) -MM -MF $@ -MT $(@:%.d=%.o) $(CXXFLAGS) $<

.PHONY: test lab

-include $(OBJECTS:%.o=%.d)

test: lyre ; @./lyre test/00.ly
lab: lab.ll ; @$(LLI) lab.ll
