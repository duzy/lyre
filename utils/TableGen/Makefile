LLVMLIBS := TableGen Support

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

OBJECTS = TableGen.o LyreASTNodesEmitter.o

TableGen: $(OBJECTS)
	$(LINK.cc) -o $@ $^ $(LOADLIBS) $(LIBS)

%.o: %.cpp
	$(COMPILE.cc) $< -o $@

%.d: %.cpp
	$(CXX) -MM -MF $@ -MT $(@:%.d=%.o) $(CXXFLAGS) $<

-include $(OBJECTS:%.o=%.d)

.PHONY: 
