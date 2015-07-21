class Decl<bit abstract = 0> {
  bit Abstract = abstract;
}

class DerivedDecl<Decl base, bit abstract = 0> : Decl<abstract> {
  Decl Base = base;  
}

class DeclContext {}

def TranslationUnit : Decl, DeclContext;
def Named : Decl<1>;
    def Namespace : DerivedDecl<Named, 1>, DeclContext;
    def Type : DerivedDecl<Named, 1>;
        def Language : DerivedDecl<Type>; // new language definition
        def Semantics : DerivedDecl<Type>; // new semantics definition
        def Tag : DerivedDecl<Type, 1>, DeclContext; // struct, class, etc.
            def Class : DerivedDecl<Tag>;
    def Value : DerivedDecl<Named, 1>;
        def Declarator : DerivedDecl<Value, 1>;
            def Variable : DerivedDecl<Declarator>;
            def Field : DerivedDecl<Declarator>;
            def Procedure : DerivedDecl<Declarator>, DeclContext; // function, method
                def Method : DerivedDecl<Procedure>;
                    def Constructor : DerivedDecl<Method>;
                    def Destructor : DerivedDecl<Method>;
                    def Conversion : DerivedDecl<Method>;
