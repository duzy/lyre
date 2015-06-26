def AbsoluteValue : DiagGroup<"absolute-value">;
def Absolute : DiagGroup<"absolute", [AbsoluteValue]>;

def DivZero : DiagGroup<"division-by-zero">;

def LiteralRange : DiagGroup<"literal-range">;

def Main : DiagGroup<"main">;
def MainReturnType : DiagGroup<"main-return-type">;

def NewlineEOF : DiagGroup<"newline-eof">;

// Backend warnings.
def BackendInlineAsm : DiagGroup<"inline-asm">;
def BackendFrameLargerThanEQ : DiagGroup<"frame-larger-than=">;
def BackendPlugin : DiagGroup<"backend-plugin">;
def RemarkBackendPlugin : DiagGroup<"remark-backend-plugin">;
def BackendOptimizationRemark : DiagGroup<"pass">;
def BackendOptimizationRemarkMissed : DiagGroup<"pass-missed">;
def BackendOptimizationRemarkAnalysis : DiagGroup<"pass-analysis">;
def BackendOptimizationFailure : DiagGroup<"pass-failed">;

// Issues with serialized diagnostics.
def SerializedDiagnostics : DiagGroup<"serialized-diagnostics">;

def UnknownWarningOption : DiagGroup<"unknown-warning-option">;

def IncompleteUmbrella : DiagGroup<"incomplete-umbrella">;

def ModuleBuild : DiagGroup<"module-build">;
def ModuleConflict : DiagGroup<"module-conflict">;
