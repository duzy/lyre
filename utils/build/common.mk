LLVM_ROOT := $(or $(wildcard /open/llvm),$(wildcard ~/Tools/llvm))
LLVM := $(LLVM_ROOT)/build
LLVM_CONFIG := $(LLVM)/Debug+Asserts/bin/llvm-config
LLVM_DIS := $(LLVM)/Debug+Asserts/bin/llvm-dis
LLVMTableGen := $(LLVM)/Debug+Asserts/bin/llvm-tblgen -I$(LLVM_ROOT)/include
LLI := $(LLVM)/Debug+Asserts/bin/lli

