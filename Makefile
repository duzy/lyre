LYRE_USING_MCJIT := true

ifeq ($(LYRE_USING_MCJIT),true)
  #LLVMLIBS := core jit mcjit native irreader
  LLVMLIBS := core mcjit native option
else
  LLVMLIBS := interpreter nativecodegen option
endif

LLVM_ROOT := $(or $(wildcard /open/llvm),$(wildcard ~/Tools/llvm))
LLVM := $(LLVM_ROOT)/build
LLVM_CONFIG := $(LLVM)/Debug+Asserts/bin/llvm-config
LLVM_DIS := $(LLVM)/Debug+Asserts/bin/llvm-dis
LLVMTableGen := $(LLVM)/Debug+Asserts/bin/llvm-tblgen -I$(LLVM_ROOT)/include
LLI := $(LLVM)/Debug+Asserts/bin/lli

TableGen := utils/TableGen/TableGen

CXXFLAGS := -Iinclude -Isource \
  -DLYRE_USING_MCJIT=$(LYRE_USING_MCJIT) \
  $(shell $(LLVM_CONFIG) --cxxflags)

# -ltinfo
LIBS := \
  $(shell $(LLVM_CONFIG) --ldflags --libs $(LLVMLIBS)) \
  -lpthread -ldl -lm -lz

LOADLIBS := 

COMBINE = $(LD) -r -o $@ $^

OBJECTS = \
  $(OBJECTS.lyre) \
  $(OBJECTS.base) \
  $(OBJECTS.ast) \
  $(OBJECTS.codegen) \
  $(OBJECTS.parse) \
  $(OBJECTS.gc) \
  $(OBJECTS.frontend) \

OBJECTS.lyre := \
  source/base.o \
  source/gc.o \
  source/ast.o \
  source/parse.o \
  source/codegen.o \
  source/frontend.o \

OBJECTS.base := \
  source/base/CharInfo.o \
  source/base/Diagnostic.o \
  source/base/DiagnosticIDs.o \
  source/base/FileManager.o \
  source/base/FileSystemStatCache.o \
  source/base/VirtualFileSystem.o \
  source/base/SourceLocation.o \
  source/base/SourceManager.o \

OBJECTS.frontend := \
  source/frontend/Compiler.o \
  source/frontend/CompilerInvocation.o \
  source/frontend/FrontendAction.o \
  source/frontend/DiagnosticRenderer.o \
  source/frontend/Options.o \
  source/frontend/TextDiagnostic.o \
  source/frontend/TextDiagnosticBuffer.o \
  source/frontend/TextDiagnosticPrinter.o \

OBJECTS.ast := \
  source/ast/Context.o \
  source/ast/Decl.o \
  source/ast/DeclKinds.o \
  source/ast/DeclGroup.o \
  source/ast/Stmt.o \
  source/ast/Expr.o \

OBJECTS.codegen := \
  source/codegen/CodeGenAction.o \
  source/codegen/CodeGenBackend.o \

OBJECTS.parse := \
  source/parse/metast.o \
  source/parse/parse.o \

OBJECTS.gc := \
  source/gc/lygc.o \

lyre: source/main.o | liblyre.a
	$(LINK.cc) -o $@ $^ liblyre.a $(LOADLIBS) $(LIBS)

liblyre.a: $(OBJECTS.lyre) ; $(AR) crs $@ $^

source/frontend.o: $(OBJECTS.frontend) ; $(COMBINE)
source/codegen.o: $(OBJECTS.codegen) ; $(COMBINE)
source/parse.o: $(OBJECTS.parse) ; $(COMBINE)
source/base.o: $(OBJECTS.base) ; $(COMBINE)
source/ast.o: $(OBJECTS.ast) ; $(COMBINE)
source/gc.o: $(OBJECTS.gc) ; $(COMBINE)

source/parse/metast.o: source/parse/metast.cpp
	$(CXX) -Isource -DLYRE_USING_MCJIT=$(LYRE_USING_MCJIT) -std=c++11 -fPIC -c $< -o $@

include/lyre/base/DiagnosticGroups.inc: include/lyre/base/Diagnostic.td $(TableGen) \
    include/lyre/base/DiagnosticGroups.td \
    include/lyre/base/DiagnosticSemaKinds.td \
    include/lyre/base/DiagnosticCategories.td
	$(TableGen) -Iinclude/lyre/base -gen-lyre-diag-groups -o=$@ $<

include/lyre/base/DiagnosticDefs.inc: include/lyre/base/Diagnostic.td $(TableGen) \
    include/lyre/base/DiagnosticCommonKinds.td \
    include/lyre/base/DiagnosticDriverKinds.td \
    include/lyre/base/DiagnosticSemaKinds.td \
    include/lyre/base/DiagnosticCategories.td
	$(TableGen) -Iinclude/lyre/base -gen-lyre-diag-defs -o=$@ $<

include/lyre/ast/DeclNodes.inc: include/lyre/base/DeclNodes.td $(TableGen)
	$(TableGen) -gen-lyre-decl-nodes -o=$@ $<

include/lyre/ast/StmtNodes.inc: include/lyre/base/StmtNodes.td $(TableGen)
	$(TableGen) -gen-lyre-stmt-nodes -o=$@ $<

include/lyre/frontend/Options.inc: include/lyre/frontend/Options.td
	$(LLVMTableGen) -gen-opt-parser-defs -o $@ $<

$(TableGen): \
    utils/TableGen/TableGen.cpp \
    utils/TableGen/TableGenBackends.h \
    utils/TableGen/LyreASTNodesEmitter.cpp \
    utils/TableGen/LyreDiagnosticsEmitter.cpp \
    utils/TableGen/Makefile
	cd $(@D) && $(MAKE) && test $(@F)

%.o: %.cpp
	$(COMPILE.cc) $< -o $@

%.d: %.cpp
	$(CXX) -MM -MF $@ -MT $(@:%.d=%.o) $(CXXFLAGS) $<

ifeq ($(findstring $(MAKECMDGOALS),clean),)
  source/base/DiagnosticIDs.d: \
    include/lyre/base/DiagnosticGroups.inc \
    include/lyre/base/DiagnosticDefs.inc \

  source/frontend/FrontendAction.d: \
    include/lyre/ast/DeclNodes.inc \
    include/lyre/ast/StmtNodes.inc \

  source/frontend/CompilerInvocation.d: \
    include/lyre/base/DiagnosticDefs.inc \

  -include $(OBJECTS:%.o=%.d)
endif

clean: ; @rm -vf lyre liblyre.a $(OBJECTS) $(OBJECTS:%.o=%.d)

test: lyre ; @./lyre test/00.ly
lab: lab.ll ; @$(LLI) lab.ll

.PHONY: test lab clean

