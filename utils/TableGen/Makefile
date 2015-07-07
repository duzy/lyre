include ../../utils/build/common.mk

LLVMLIBS := TableGen Support

CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)

LIBS := \
  $(shell $(LLVM_CONFIG) --ldflags --libs $(LLVMLIBS)) \
  -lpthread -ldl -lm -lz $(EXTRA_LIBS)

LOADLIBS := 

OBJECTS = TableGen.o NeonEmitter.o \
  LyreASTNodesEmitter.o \
  LyreDiagnosticsEmitter.o \

TableGen: $(OBJECTS)
	$(LINK.cc) -o $@ $^ $(LOADLIBS) $(LIBS)

%.o: %.cpp
	$(COMPILE.cc) $< -o $@

%.d: %.cpp
	$(CXX) -MM -MF $@ -MT $(@:%.d=%.o) $(CXXFLAGS) $<

-include $(OBJECTS:%.o=%.d)

.PHONY: 
