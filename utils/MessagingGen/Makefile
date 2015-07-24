include ../../utils/build/common.mk

LLVMLIBS := TableGen Support

CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)

LIBS := \
  $(shell $(LLVM_CONFIG) --ldflags --libs $(LLVMLIBS)) \
  -lpthread -ldl -lm -lz $(EXTRA_LIBS)

LOADLIBS := 

OBJECTS = MessagingGen.o \
  LyreMessagingDriverEmitter.o \
  LyreMessagingJavaDriverEmitter.o \

JEROMQ = $(HOME)/Tools/jeromq/src/main/java

MessagingGen: $(OBJECTS)
	$(LINK.cc) -o $@ $^ $(LOADLIBS) $(LIBS)

%.o: %.cpp
	$(COMPILE.cc) $< -o $@

%.d: %.cpp
	$(CXX) -MM -MF $@ -MT $(@:%.d=%.o) $(CXXFLAGS) $<

-include $(OBJECTS:%.o=%.d)

test:
	@$(LLVMTableGen) -print-records example.td

example: example_main.cpp example.cc messaging.inc
	$(CXX) -std=c++1y -I$(BOOST_ROOT) -DBOOST_ERROR_CODE_HEADER_ONLY -o $@ $< -lzmq -lpthread

com/lyrecode/example.class: example.java messaging.java
	@mkdir -p classes
	javac -d classes -classpath "$(JEROMQ)" $^

com/lyrecode/messaging.class: messaging.java
	@mkdir -p classes
	javac -d classes -classpath "$(JEROMQ)" $<

run: example com/lyrecode/example.class
	./example & (P=$$! && java -classpath classes com.lyrecode.example && echo "waiting $$P..." && (wait $$P; true))

example_main.cpp: example.hh example.cc
example.hh: example.td MessagingGen
	./MessagingGen -gen-messaging-driver-hh -ns=example -o $@ $<
example.cc: example.td MessagingGen example.hh
	./MessagingGen -gen-messaging-driver-cc -ns=example -hh=example.hh -o $@ $<
messaging.java: example.td MessagingGen
	./MessagingGen -gen-messaging-driver-j -ns=com.lyrecode -o $@ $<

.PHONY: 
