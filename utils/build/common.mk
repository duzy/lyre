LLVM_ROOT := $(or $(wildcard /open/llvm),$(wildcard ~/Tools/llvm))
LLVM := $(LLVM_ROOT)/build
LLVM_CONFIG := $(LLVM)/Debug+Asserts/bin/llvm-config
LLVM_DIS := $(LLVM)/Debug+Asserts/bin/llvm-dis
LLVMTableGen := $(LLVM)/Debug+Asserts/bin/llvm-tblgen -I$(LLVM_ROOT)/include
LLI := $(LLVM)/Debug+Asserts/bin/lli

BOOST_ROOT := $(or $(wildcard /open/boost_1_58_0),$(wildcard ~/Tools/boost_1_58_0))

CXXSTD := -std=c++11 -std=c++1y

EXTRA_LIBS :=

ifeq ($(shell pkg-config --exists tinfo && echo ok),ok)
  EXTRA_LIBS += -ltinfo
endif
