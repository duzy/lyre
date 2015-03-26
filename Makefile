LYRE_USING_MCJIT := true

ifeq ($(LYRE_USING_MCJIT),true)
  #LLVMLIBS := core jit mcjit native irreader
  LLVMLIBS := core mcjit native
else
  LLVMLIBS := interpreter nativecodegen
endif

$(info LLVMLIBS: $(LLVMLIBS))
$(info ---------------------------)

LLVM := $(or $(wildcard /open/llvm),$(wildcard ~/tools/ll/llvm))
LLVM_CONFIG := $(LLVM)/Debug+Asserts/bin/llvm-config
LLVM_DIS := $(LLVM)/Debug+Asserts/bin/llvm-dis
LLI := $(LLVM)/Debug+Asserts/bin/lli

CXXFLAGS := \
  -DLYRE_USING_MCJIT=$(LYRE_USING_MCJIT) \
  $(shell $(LLVM_CONFIG) --cxxflags)
LIBS := \
  $(shell $(LLVM_CONFIG) --ldflags --libs $(LLVMLIBS)) \
  -lpthread -ltinfo -ldl -lm -lz
LOADLIBS := 

lyre: source/main.o source/compiler.o source/parse.o source/gc/lygc.o
	$(LINK.cc) -o $@ $^ $(LOADLIBS) $(LIBS)

source/main.o: source/main.cpp source/parse.h source/ast.h source/compiler.h
source/compiler.o: source/compiler.cpp source/ast.h source/compiler.h
source/parse.o: source/parse.cpp source/parse.h source/grammar.h source/ast.h
source/gc/lygc.o: source/gc/lygc.cpp

source/compiler.cpp: source/cc.ipp

source/parse.o:
	$(CXX) -DLYRE_USING_MCJIT=$(LYRE_USING_MCJIT) -std=c++11 -fPIC -c $< -o $@

%.o: %.cpp
	$(COMPILE.cc) $< -o $@

test: lyre ; @./lyre test/00.ly
lab: lab.ll ; @$(LLI) lab.ll

.PHONY: test lab
