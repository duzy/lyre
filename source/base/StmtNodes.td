class AttrSubject;

class Stmt<bit abstract = 0> : AttrSubject {
  bit Abstract = abstract;
}

class DStmt<Stmt base, bit abstract = 0> : Stmt<abstract> {
  Stmt Base = base;
}

// Statements
def NullStmt : Stmt;
def CompoundStmt : Stmt;
def LabelStmt : Stmt;
def AttributedStmt : Stmt;
def IfStmt : Stmt;
def SwitchStmt : Stmt;
def WhileStmt : Stmt;
def DoStmt : Stmt;
def ForStmt : Stmt;
def GotoStmt : Stmt;
def IndirectGotoStmt : Stmt;
def ContinueStmt : Stmt;
def BreakStmt : Stmt;
def ReturnStmt : Stmt;
def DeclStmt  : Stmt;
def SwitchCase : Stmt<1>;
def CaseStmt : DStmt<SwitchCase>;
def DefaultStmt : DStmt<SwitchCase>;
def CapturedStmt : Stmt;
