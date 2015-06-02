class Decl<bit abstract = 0> {
  bit Abstract = abstract;
}

class DerivedDecl<Decl base, bit abstract = 0> : Decl<abstract> {
  Decl Base = base;  
}

class DeclContext {}

def TranslationUnit : Decl, DeclContext;
def Named : Decl<1>;
    def Type : DerivedDecl<Named, 1>;
        def Tag : DerivedDecl<Type, 1>, DeclContext; // struct, class, etc.
            def Class : DerivedDecl<Tag>;
    def Value : DerivedDecl<Named, 1>;
        def Declarator : DerivedDecl<Value, 1>;
            def Var : DerivedDecl<Declarator>;
            def Field : DerivedDecl<Declarator>;
            def Proc : DerivedDecl<Declarator>, DeclContext; // function, method
                def Method : DerivedDecl<Proc>;
                    def Constructor : DerivedDecl<Method>;
                    def Destructor : DerivedDecl<Method>;
                    def Conversion : DerivedDecl<Method>;
