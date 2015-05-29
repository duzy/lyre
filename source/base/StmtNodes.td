class Stmt<bit abstract = 0> {
  bit Abstract = abstract;
}

class DerivedStmt<Stmt base, bit abstract = 0> : Stmt<abstract> {
  Stmt Base = base;  
}

// Statements
def NullStmt : Stmt;
def CompoundStmt : Stmt;
def DeclStmt : Stmt; // decl, proc, type, etc
def SeeStmt : Stmt;
def WithStmt : Stmt;
def SpeakStmt : Stmt;
def PerStmt : Stmt;
def ReturnStmt : Stmt;
def SeeForkStmt : Stmt<1>;
def BareForkStmt : DerivedStmt<SeeForkStmt>;
def ValueForkStmt : DerivedStmt<SeeForkStmt>;

// Expressions
def Expr : Stmt<1>;
def IntegerLiteral : DerivedStmt<Expr>;
def FloatingLiteral : DerivedStmt<Expr>;
def StringLiteral : DerivedStmt<Expr>;
def CharacterLiteral : DerivedStmt<Expr>;
def NodeConstructionExpr : DerivedStmt<Expr>;
def ListExpr : DerivedStmt<Expr>;
