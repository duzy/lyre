include ../../utils/build/common.mk

LLVM_LIBS := TableGen Support

CLEAN += TableGen

OBJECTS = TableGen.o NeonEmitter.o \
  LyreASTNodesEmitter.o \
  LyreDiagnosticsEmitter.o \

TableGen: $(OBJECTS)
	$(LINK) -o $@

.PHONY: 
