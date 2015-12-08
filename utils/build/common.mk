LLVM_ROOT := $(wildcard /open/llvm)
LLVM := $(LLVM_ROOT)/build
LLVM_BUILD := $(LLVM_ROOT)/build
LLVM_CONFIG := $(LLVM_BUILD)/bin/llvm-config
LLVM_DIS := $(LLVM_BUILD)/bin/llvm-dis
LLVMTableGen := $(LLVM_BUILD)/bin/llvm-tblgen -I$(LLVM_ROOT)/include
LLI := $(LLVM_BUILD)/bin/lli

BOOST_ROOT := $(wildcard /open/boost_1_59_0)

CXXSTD := -std=c++11 -std=c++1y

EXTRA_LIBS :=

ifeq ($(shell pkg-config --exists tinfo && echo ok),ok)
  EXTRA_LIBS += -ltinfo
endif
