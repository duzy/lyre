def AbsoluteValue : DiagGroup<"absolute-value">;
def Absolute : DiagGroup<"absolute", [AbsoluteValue]>;

def DivZero : DiagGroup<"division-by-zero">;

def LiteralRange : DiagGroup<"literal-range">;

def Main : DiagGroup<"main">;
def MainReturnType : DiagGroup<"main-return-type">;

def NewlineEOF : DiagGroup<"newline-eof">;
