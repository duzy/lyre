#include "parse.h"
#include "metast.h"
#include "llvm/Support/raw_ostream.h"

namespace lyre
{
    namespace metast
    {
        stmts parse_file(const std::string & filename);
    }

    using ast::StmtResult;
    using ast::StmtError;
    
    struct converter
    {
        typedef StmtResult result_type;

        ast::Context & Context;
        ast::DeclContext *CurrentDeclContext;
        StmtResult Root;

        explicit converter(ast::Context & C, const metast::stmts & metaStmts)
            : Context(C), Root(true), CurrentDeclContext(nullptr)
        {
            std::vector<ast::Stmt*> Stmts;
            
            for (auto metaStmt : metaStmts) {
                auto Stmt = boost::apply_visitor(*this, metaStmt);
                if (Stmt.isInvalid()) {
                    llvm::errs() << "Invalid statement!\n";
                    //break;
                }
                Stmts.push_back(Stmt.get());
            }
            
            Root = new (C) ast::CompoundStmt(C, Stmts);
        }

        StmtResult operator()(const metast::none &);
        StmtResult operator()(const metast::expr & s);
        StmtResult operator()(const metast::decl & s);
        StmtResult operator()(const metast::proc & s);
        StmtResult operator()(const metast::type & s);
        StmtResult operator()(const metast::see & s);
        StmtResult operator()(const metast::with & s);
        StmtResult operator()(const metast::speak & s);
        StmtResult operator()(const metast::per & s);
        StmtResult operator()(const metast::ret & s);
    };

    StmtResult parse_file(ast::Context & context, const std::string & filename)
    {
        return converter(context, metast::parse_file(filename)).Root;
    }

    StmtResult converter::operator()(const metast::none &)
    {
        return StmtResult(new (Context) ast::NullStmt);
    }

    StmtResult converter::operator()(const metast::expr & s)
    {
        llvm::errs()<<"expr\n";
    }

    StmtResult converter::operator()(const metast::decl & decl)
    {
        std::vector<ast::Decl*> Decls;
        
        for (auto & sym : decl) {
            if (sym.id.string == "_") {
                llvm::errs() << "Using '_' as a symbol name!\n" ;
                return StmtError();
            }

            ast::VarDecl *VD = ast::VarDecl::Create(Context, CurrentDeclContext);

            llvm::errs() << "TODO: decl " << sym.id.string << "\n";
            
            Decls.push_back(VD);
        }

        auto DGRef = ast::DeclGroupRef::Create(Context, &Decls[0], Decls.size());
        auto DeclStmt(new (Context) ast::DeclStmt(DGRef));
        
        llvm::errs() << DeclStmt->getDeclGroup().getSingleDecl() <<"\n";
        
        return StmtResult(DeclStmt);
    }

    StmtResult converter::operator()(const metast::proc & s)
    {
        llvm::errs()<<"proc\n";
    }

    StmtResult converter::operator()(const metast::type & s)
    {
        llvm::errs()<<"type\n";
    }

    StmtResult converter::operator()(const metast::see & s)
    {
        llvm::errs()<<"see\n";
    }

    StmtResult converter::operator()(const metast::with & s)
    {
        llvm::errs()<<"with\n";
    }

    StmtResult converter::operator()(const metast::speak & s)
    {
        llvm::errs()<<"speak\n";
    }

    StmtResult converter::operator()(const metast::per & s)
    {
        llvm::errs()<<"per\n";
    }

    StmtResult converter::operator()(const metast::ret & s)
    {
        llvm::errs()<<"ret\n";
    }
}

/**
 *  AllocaInst - an instruction to allocate memory on the stack
 *  LoadInst - an instruction for reading from memory
 *  StoreInst - an instruction for storing to memory
 *  FenceInst - an instruction for ordering other memory operations
 *
 *  AtomicCmpXchgInst - an instruction that atomically checks whether a
 *      specified value is in a memory location, and, if it is, stores a new value
 *      there
 *  AtomicRMWInst - an instruction that atomically reads a memory location,
 *      combines it with another value, and then stores the result back
 *
 *  GetElementPtrInst - an instruction for type-safe pointer arithmetic to
 *      access elements of arrays and structs
 *
 *  ICmpInst - Represent an integer comparison operator.
 *  FCmpInst - Represents a floating point comparison operator.
 *
 *  CallInst - This class represents a function call, abstracting a target
 *      machine's calling convention
 *
 *  SelectInst - This class represents the LLVM 'select' instruction.
 *
 *  VAArgInst - This class represents the va_arg llvm instruction
 *
 *  ExtractElementInst - This instruction extracts a single (scalar)
 *      element from a VectorType value
 *  InsertElementInst - This instruction inserts a single (scalar)
 *      element into a VectorType value
 *
 *  ShuffleVectorInst - This instruction constructs a fixed permutation of two
 *      input vectors
 *
 *  ExtractValueInst - This instruction extracts a struct member or array
 *      element value from an aggregate value
 *  InsertValueInst - This instruction inserts a struct field of array element
 *      value into an aggregate value
 *
 *  PHINode - The PHINode class is used to represent the magical mystical PHI node
 *
 *  LandingPadInst - The landingpad instruction holds all of the information
 *      necessary to generate correct exception handling
 *
 *  ReturnInst - Return a value (possibly void), from a function
 *  BranchInst - Conditional or Unconditional Branch instruction
 *  SwitchInst - Multiway switch
 *  IndirectBrInst - Indirect Branch Instruction
 *
 *  InvokeInst - Invoke instruction
 *  ResumeInst - Resume the propagation of an exception
 *  UnreachableInst - This function has undefined behavior
 *
 *  TruncInst - represents a truncation of integer types
 *  ZExtInst - represents zero extension of integer types
 *  SExtInst - represents a sign extension of integer types
 *  FPTruncInst - represents a truncation of floating point types
 *  FPExtInst - represents an extension of floating point types
 *  UIToFPInst - represents a cast unsigned integer to floating point
 *  SIToFPInst - represents a cast from signed integer to floating point
 *  FPToUIInst - represents a cast from floating point to unsigned integer
 *  FPToSIInst - represents a cast from floating point to signed integer
 *  IntToPtrInst - represents a cast from an integer to a pointer
 *  PtrToIntInst - represents a cast from a pointer to an integer
 *  BitCastInst - represents a no-op cast from one type to another
 *  AddrSpaceCastInst - represents a conversion between pointers
 *      from one address space to another
 */
