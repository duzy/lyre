class Decl<bit abstract = 0> {
  bit Abstract = abstract;
}

class DerivedDecl<Decl base, bit abstract = 0> : Decl<abstract> {
  Decl Base = base;  
}

class DeclContext {}

def Named : Decl<1>;
  def Type : DerivedDecl<Named, 1>;
    def Tag : DerivedDecl<Type, 1>, DeclContext;
      def Enum : DerivedDecl<Tag>;
      def Record : DerivedDecl<Tag>;
  def Value : DerivedDecl<Named, 1>;
    def Declarator : DerivedDecl<Value, 1>;
      def Field : DerivedDecl<Declarator>;
      def Function : DerivedDecl<Declarator>, DeclContext;
        def Method : DerivedDecl<Function>;
          def Constructor : DerivedDecl<Method>;
          def Destructor : DerivedDecl<Method>;
          def Conversion : DerivedDecl<Method>;
      def Var : DerivedDecl<Declarator>;
