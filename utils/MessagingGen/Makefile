include ../../utils/build/common.mk

LLVMLIBS := TableGen Support

CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)

LIBS := \
  $(shell $(LLVM_CONFIG) --ldflags --libs $(LLVMLIBS)) \
  -lpthread -ldl -lm -lz $(EXTRA_LIBS)

LOADLIBS := 

OBJECTS = MessagingGen.o \
  LyreMessagingDriverEmitter.o \

MessagingGen: $(OBJECTS)
	$(LINK.cc) -o $@ $^ $(LOADLIBS) $(LIBS)

%.o: %.cpp
	$(COMPILE.cc) $< -o $@

%.d: %.cpp
	$(CXX) -MM -MF $@ -MT $(@:%.d=%.o) $(CXXFLAGS) $<

-include $(OBJECTS:%.o=%.d)

test:
	@$(LLVMTableGen) -print-records example.td

example: example.cc
#	$(CXX) -std=c++1y -I$(BOOST_ROOT) -o $@ $^
	$(CXX) -std=c++1y -o $@ $^ -lboost-system

example.cc: example.td MessagingGen
	./MessagingGen -o $@ $<

.PHONY: 
