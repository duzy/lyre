#include "metast.h"

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

namespace lyre
{
    namespace metast
    {
        template<class T>
        struct is
        {
            typedef bool result_type;
            bool operator()(const T &) { return true; }
            template<class A> bool operator()(const A &) { return false; }
        };

        /*
        // std::clog<<"stmts: "<<stmts.size()<<std::endl;
        stmt_dumper visit;
        for (auto stmt : stmts) {
            boost::apply_visitor(visit, stmt);
        }
        std::clog << std::string(12, '-') << std::endl;
        */
        struct stmt_dumper
        {
            typedef void result_type;

            int _indent;

            stmt_dumper() : _indent(0) {}

            std::string indent() const { return 0 < _indent ? std::string(_indent, ' ') : std::string(); }
            void indent(int n) { _indent += n; }

            void operator()(int v)
            {
                std::clog<<indent()<<"(int) "<<v<<std::endl;
            }

            void operator()(unsigned int v)
            {
                std::clog<<indent()<<"(unsigned int) "<<v<<std::endl;
            }

            void operator()(float v)
            {
                std::clog<<indent()<<"(float) "<<v<<std::endl;
            }

            void operator()(double v)
            {
                std::clog<<indent()<<"(double) "<<v<<std::endl;
            }

            void operator()(const std::string & v)
            {
                std::clog<<indent()<<"(string) "<<v<<std::endl;
            }

            void operator()(const lyre::metast::identifier & v)
            {
                std::clog<<indent()<<"(identifier) "<<v.string<<std::endl;
            }

            void operator()(const lyre::metast::expr & e)
            {
                is<lyre::metast::none> isNone;
                if (e.operators.size() == 0 && boost::apply_visitor(isNone, e.operand)) {
                    std::clog<<indent()<<"expr: none"<<std::endl;
                    return;
                }
                std::clog<<indent()<<"expr: ("<<e.operators.size()<<" ops)"<<std::endl;
                indent(4);
                boost::apply_visitor(*this, e.operand);
                for (auto op : e.operators) {
                    std::clog<<indent()<<"op: "<<int(op.opcode)<<std::endl;
                    indent(4);
                    boost::apply_visitor(*this, op.operand);
                    indent(-4);
                }
                indent(-4);
            }

            void operator()(const lyre::metast::none &)
            {
                std::clog<<indent()<<"none:"<<std::endl;
            }

            void operator()(const lyre::metast::decl & s)
            {
                std::clog<<indent()<<"decl: "<<std::endl;
            }

            void operator()(const lyre::metast::proc & s)
            {
                std::clog<<indent()<<"proc: "<<s.name.string<<std::endl;
            }

            void operator()(const lyre::metast::type & s)
            {
                std::clog<<indent()<<"type: "<<s.name.string<<std::endl;
            }

            void operator()(const lyre::metast::see & s)
            {
                std::clog<<indent()<<"see: "<<std::endl;
            }

            void operator()(const lyre::metast::with & s)
            {
                std::clog<<indent()<<"with: "<<std::endl;
            }

            void operator()(const lyre::metast::speak & s)
            {
                std::clog<<indent()<<"speak: "<<std::endl;
            }

            template <class T>
            void operator()(const T &)
            {
                std::clog<<indent()<<"stmt: ?"<<std::endl;
            }
        };
    }
}
