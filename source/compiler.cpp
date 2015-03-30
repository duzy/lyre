#if LYRE_USING_MCJIT
#  include <llvm/ExecutionEngine/MCJIT.h>
#  include <llvm/ExecutionEngine/SectionMemoryManager.h>
#else
#  include <llvm/ExecutionEngine/Interpreter.h>
#endif
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include "compiler.h"

#define D(MSG)                                                          \
    std::clog << __FILE__ << ":" << __LINE__ << ": " << MSG << std::endl ;
    
#define DUMP_TY(MSG, TY)                                                \
    std::clog << __FILE__ << ":" << __LINE__ << ": " << MSG ;           \
    if (TY) { (TY)->dump(); } else { std::clog << "null" << std::endl; }

// TODO: http://llvm.org/docs/GarbageCollection.html

namespace lyre
{
    using namespace llvm;
}

#include "ntt.ipp"
#include "cc.ipp"

namespace lyre
{
    struct expr_compiler
    {
        typedef Value * result_type;

        compiler *comp;

        Value *compile(const ast::expr & v);
        Value *operator()(const ast::expr & v);
        Value *operator()(const ast::none & v);
        Value *operator()(const ast::identifier & v);
        Value *operator()(const ast::nodector & v);
        Value *operator()(const std::string & v);
        Value *operator()(ast::cv v);
        Value *operator()(int v);
        Value *operator()(unsigned int v);
        Value *operator()(float v);
        Value *operator()(double v);

    private:
        Value *op_nil   (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_attr  (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_select(const ast::op & op, Value *operand1, Value *operand2);
        Value *op_call  (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_unary_plus (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_unary_minus(const ast::op & op, Value *operand1, Value *operand2);
        Value *op_unary_not  (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_unary_dot  (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_unary_arrow(const ast::op & op, Value *operand1, Value *operand2);
        Value *op_mul(const ast::op & op, Value *operand1, Value *operand2);
        Value *op_div(const ast::op & op, Value *operand1, Value *operand2);
        Value *op_add(const ast::op & op, Value *operand1, Value *operand2);
        Value *op_sub(const ast::op & op, Value *operand1, Value *operand2);
        Value *op_lt (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_le (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_gt (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_ge (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_eq (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_ne (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_and(const ast::op & op, Value *operand1, Value *operand2);
        Value *op_or (const ast::op & op, Value *operand1, Value *operand2);
        Value *op_xor(const ast::op & op, Value *operand1, Value *operand2);
        Value *op_set(const ast::op & op, Value *operand1, Value *operand2);

        Value *binary(Instruction::BinaryOps, Value *operand1, Value *operand2);
        Value *compare(CmpInst::Predicate, Value *operand1, Value *operand2);
    };

    Value *expr_compiler::compile(const ast::expr & expr)
    {
        auto operand1 = boost::apply_visitor(*this, expr.operand);
        if (expr.operators.empty()) {
            return operand1;
        }

        if (expr.operators.front().opcode == ast::opcode::comma) {
            std::vector<Metadata*> a{ ValueAsMetadata::get(operand1) };
            for (auto & op : expr.operators) {
                assert(op.opcode == ast::opcode::comma && "mixed comma with other operators");
                auto value = boost::apply_visitor(*this, op.operand);
                a.push_back( ValueAsMetadata::get(value) );
            }
            return MetadataAsValue::get(comp->context, MDNode::get(comp->context, a));
        }

        for (auto & op : expr.operators) {
            auto operand2 = boost::apply_visitor(*this, op.operand);
            switch (op.opcode) {
            case ast::opcode::nil:      operand1 = op_nil(op, operand1, operand2);   break;

            case ast::opcode::attr:     operand1 = op_attr(op, operand1, operand2);   break;
            case ast::opcode::select:   operand1 = op_select(op, operand1, operand2); break;
            case ast::opcode::call:     operand1 = op_call(op, operand1, operand2);   break;

            case ast::opcode::unary_plus:  operand1 = op_unary_plus(op, operand1, operand2);  break;
            case ast::opcode::unary_minus: operand1 = op_unary_minus(op, operand1, operand2); break;
            case ast::opcode::unary_not:   operand1 = op_unary_not(op, operand1, operand2);   break;
            case ast::opcode::unary_dot:   operand1 = op_unary_dot(op, operand1, operand2);   break;
            case ast::opcode::unary_arrow: operand1 = op_unary_arrow(op, operand1, operand2); break;

            case ast::opcode::mul:      operand1 = op_mul(op, operand1, operand2); break;
            case ast::opcode::div:      operand1 = op_div(op, operand1, operand2); break;
            case ast::opcode::add:      operand1 = op_add(op, operand1, operand2); break;
            case ast::opcode::sub:      operand1 = op_sub(op, operand1, operand2); break;

            case ast::opcode::lt:       operand1 = op_lt(op, operand1, operand2); break;
            case ast::opcode::le:       operand1 = op_le(op, operand1, operand2); break;
            case ast::opcode::gt:       operand1 = op_gt(op, operand1, operand2); break;
            case ast::opcode::ge:       operand1 = op_ge(op, operand1, operand2); break;

            case ast::opcode::eq:       operand1 = op_eq(op, operand1, operand2); break;
            case ast::opcode::ne:       operand1 = op_ne(op, operand1, operand2); break;

            case ast::opcode::a:        operand1 = op_and(op, operand1, operand2); break;
            case ast::opcode::o:        operand1 = op_or (op, operand1, operand2); break;
            case ast::opcode::xo:       operand1 = op_xor(op, operand1, operand2); break;

            case ast::opcode::set:      operand1 = op_set(op, operand1, operand2); break;

            default:
                D(__FUNCTION__
                    << ": TODO: expr: op = " << int(op.opcode) << ", "
                    //<< "operand1 = " << operand1 << ", "
                    //<< "operand2 = " << operand2
                  );
            }
        }

        return operand1;
    }

    Value *expr_compiler::operator()(const ast::expr & expr)
    {
        return compile(expr);
    }

    Value *expr_compiler::operator()(const ast::none & v)
    {
        // D(__FUNCTION__ << ": none");
        return nullptr;
    }

    Value *expr_compiler::operator()(const ast::identifier & id)
    {
        auto & name = id.string;

        //D("identifier: "<<name);

        auto sym = comp->builder->GetInsertBlock()->getValueSymbolTable()->lookup(name);
        if (sym) return sym;
        
        auto gv = comp->module->getGlobalVariable(name);
        if (gv) return gv;

        auto fun = comp->module->getFunction(name);
        if (fun) return fun;
        
        return nullptr;
    }

    Value *expr_compiler::operator()(const ast::nodector & nodector)
    {
        D("nodector");
        return comp->builder->CreateGlobalString("node", ".str");
    }

    Value *expr_compiler::operator()(const std::string & v)
    {
        //std::clog << __FUNCTION__ << ": string = " << v << std::endl;
        return comp->builder->CreateGlobalString(v, ".str");
    }

    Value *expr_compiler::operator()(ast::cv cv)
    {
        //std::clog << __FUNCTION__ << ": int = " << v << std::endl;
        switch (cv) {
        case ast::cv::null:
            break;
        case ast::cv::true_:
            return comp->builder->getInt1(1);
        case ast::cv::false_:
            return comp->builder->getInt1(0);
        }
        return nullptr;
    }

    Value *expr_compiler::operator()(int v)
    {
        //std::clog << __FUNCTION__ << ": int = " << v << std::endl;
        return comp->builder->getInt32(v);
    }

    Value *expr_compiler::operator()(unsigned int v)
    {
        return comp->builder->getInt32(v);
    }

    Value *expr_compiler::operator()(float v)
    {
        D(__FUNCTION__ << ": float = " << v);
        return nullptr;
    }

    Value *expr_compiler::operator()(double v)
    {
        //!< see <llvm/ADT/APFloat.h>
        return ConstantFP::get(comp->context, APFloat(v));
    }

    Value *expr_compiler::op_nil(const ast::op & op, Value *operand1, Value *operand2)
    {
        return nullptr;
    }

    Value *expr_compiler::op_attr(const ast::op & op, Value *operand1, Value *operand2)
    {
        D(__FUNCTION__);
        return operand1;
    }

    Value *expr_compiler::op_select(const ast::op & op, Value *operand1, Value *operand2)
    {
        D(__FUNCTION__);
        return operand1;
    }

    Value *expr_compiler::op_call(const ast::op & op, Value *operand1, Value *operand2)
    {
        if (!isa<Function>(operand1)) {
            errs()
                << "lyre: '" << operand1->getName().str() << "' is not a function"
                << "\n" ;
            return nullptr;
        }

        auto fun = cast<Function>(operand1);
        auto fty = fun->getFunctionType();

        //D("calling "<<fun->getName().str()<<", "<<fty->getNumParams());

        std::vector<Value*> args;

        if (operand2->getType()->isMetadataTy()) {
            auto metaArgs = cast<MDNode>(cast<MetadataAsValue>(operand2)->getMetadata());
            auto n = 0;
            for (auto & metaArg : metaArgs->operands()) {
                auto paramType = fty->getParamType(n++);                        //DUMP_TY("param-type: ", paramType);
                auto argMetadata = cast<ValueAsMetadata>(metaArg.get());        //DUMP_TY("arg-type: ", argMetadata->getValue()->getType());
                auto arg = comp->calling_cast(paramType, argMetadata->getValue());
                args.push_back(arg);
            }
        } else {
            //DUMP_TY("param-type: ", fty->getParamType(0));
            //DUMP_TY("arg-type: ", operand2->getType());
            args.push_back(comp->calling_cast(fty->getParamType(0), operand2));
        }

        if (!fty->isVarArg() && args.size() != fty->getNumParams()) {
            errs()
                << "lyre: '" << operand1->getName().str() << "' wrong number of arguments"
                << "\n" ;
            return nullptr;
        }

        for (auto n=0; n < fty->getNumParams(); ++n) {
            auto pty = fty->getParamType(n);
            auto aty = args[n]->getType();
            if (pty == aty) continue;
            if (aty->canLosslesslyBitCastTo(pty)) {
                if (aty->isPointerTy()) {
                    args[n] = comp->builder->CreatePointerCast(args[n], pty);
                } else if (aty->isIntegerTy()) {
                    args[n] = comp->builder->CreateIntCast(args[n], pty, true /* TODO: isSigned */);
                }
                continue;
            }
        }

        auto name = "res";
        if (fun->getReturnType()->isVoidTy()) {
            name = ""; // "Cannot assign a name to void values!"
        }
        return comp->builder->CreateCall(fun, args, name);
    }

    Value *expr_compiler::op_set(const ast::op & op, Value *operand1, Value *operand2)
    {
        auto var = operand1;
        if (isa<Argument>(var)) {
            auto arg = cast<Argument>(var);
            auto varName = arg->getName().str() + ".addr";
            if ((var = arg->getParent()->getValueSymbolTable().lookup(varName)) == nullptr) {
                errs()
                    << "lyre: argument#" << arg->getArgNo() << " " << arg->getName().str()
                    << " don't have an address"
                    << "\n" ;
                return nullptr;
            }
        }

        //DUMP_TY("value-type: ", operand2->getType());

        auto varTy = var->getType();

        //DUMP_TY("variable-type: ", varTy);

        if (!varTy->isPointerTy()) {
            errs()
                << "lyre: can't set value to a non-pointer value"
                << "\n" ;
            return nullptr;
        }

        auto val = operand2;
        auto varElementTy = varTy->getSequentialElementType();

        if (varElementTy == comp->variant) {
            val = comp->calling_cast(nullptr, val);

            //DUMP_TY("value-type: ", val->getType());

            if (val->getType() == comp->variant) {
                // ...
            }

            auto zero = comp->builder->getInt32(0);
            auto idxz = std::vector<Value*>{ zero, zero, zero };
            auto ptr1 = comp->builder->CreateGEP(var, idxz); // CreateStructGEP

            /**
             *  Convert the storage pointer and store 'val' to the variant
             */
            comp->builder->CreateStore(val,
                comp->builder->CreatePointerCast(ptr1, PointerType::getUnqual(val->getType())));

            /**
             *  Get vtable storage 
             */
            idxz = std::vector<Value*>{ zero, comp->builder->getInt32(1) };
            auto ptr2 = comp->builder->CreateGEP(var, idxz);
            //comp->builder->CreateStore(vtable, ptr2); ///< store vtable
        } else {
            val = comp->calling_cast(varElementTy, val);
            comp->builder->CreateStore(val, var); ///< store 'val' to the 'var'
        }

        return operand1;
    }

    Value *expr_compiler::op_unary_plus(const ast::op & op, Value *operand1, Value *operand2)
    {
        D(__FUNCTION__);
        return operand1;
    }

    Value *expr_compiler::op_unary_minus(const ast::op & op, Value *operand1, Value *operand2)
    {
        D(__FUNCTION__);
        return operand1;
    }

    Value *expr_compiler::op_unary_not(const ast::op & op, Value *operand1, Value *operand2)
    {
        D(__FUNCTION__);
        return operand1;
    }

    Value *expr_compiler::op_unary_dot(const ast::op & op, Value *operand1, Value *operand2)
    {
        D(__FUNCTION__);
        return operand1;
    }

    Value *expr_compiler::op_unary_arrow(const ast::op & op, Value *operand1, Value *operand2)
    {
        D(__FUNCTION__);
        return operand1;
    }

    Value *expr_compiler::op_mul(const ast::op & op, Value *operand1, Value *operand2)
    {
        return binary(Instruction::Mul,  operand1, operand2);
    }

    Value *expr_compiler::op_div(const ast::op & op, Value *operand1, Value *operand2)
    {
        return binary(Instruction::UDiv,  operand1, operand2);
    }

    Value *expr_compiler::op_add(const ast::op & op, Value *operand1, Value *operand2)
    {
        return binary(Instruction::Add,  operand1, operand2); // comp->builder->CreateAdd(operand1, operand2, "tmp");
    }

    Value *expr_compiler::op_sub(const ast::op & op, Value *operand1, Value *operand2)
    {
        return binary(Instruction::Sub,  operand1, operand2);
    }

    Value *expr_compiler::op_lt(const ast::op & op, Value *operand1, Value *operand2)
    {
        return compare(CmpInst::ICMP_SLT, operand1, operand2);
    }

    Value *expr_compiler::op_le(const ast::op & op, Value *operand1, Value *operand2)
    {
        return compare(CmpInst::ICMP_SLE, operand1, operand2);
    }

    Value *expr_compiler::op_gt(const ast::op & op, Value *operand1, Value *operand2)
    {
        return compare(CmpInst::ICMP_SGT, operand1, operand2);
    }

    Value *expr_compiler::op_ge(const ast::op & op, Value *operand1, Value *operand2)
    {
        return compare(CmpInst::ICMP_SGE, operand1, operand2);
    }

    Value *expr_compiler::op_eq(const ast::op & op, Value *operand1, Value *operand2)
    {
        return compare(CmpInst::ICMP_EQ, operand1, operand2);
    }

    Value *expr_compiler::op_ne(const ast::op & op, Value *operand1, Value *operand2)
    {
        return compare(CmpInst::ICMP_NE, operand1, operand2);
    }

    Value *expr_compiler::op_and(const ast::op & op, Value *operand1, Value *operand2)
    {
        return binary(Instruction::And,  operand1, operand2);
    }

    Value *expr_compiler::op_or(const ast::op & op, Value *operand1, Value *operand2)
    {
        return binary(Instruction::Or,  operand1, operand2);
    }

    Value *expr_compiler::op_xor(const ast::op & op, Value *operand1, Value *operand2)
    {
        return binary(Instruction::Xor,  operand1, operand2);
    }

    Value *expr_compiler::binary(Instruction::BinaryOps op, Value *operand1, Value *operand2)
    {
        auto ty1 = operand1->getType();
        auto ty2 = operand2->getType();

        if (ty1 == ty2) {
            if ((ty1 == comp->variant) || (ty1->isPointerTy() && ty1->getSequentialElementType() == comp->variant)) {
                errs()
                    << "lyre: can't perform binary operation on two variants"
                    << "\n" ;
                return nullptr;
            } else if (ty1->isPointerTy()) {
                operand1 = comp->builder->CreateLoad(operand1);
                operand2 = comp->builder->CreateLoad(operand2);
            }
        } else {
            if (ty1->isPointerTy()) {
                if (ty1->getSequentialElementType() == comp->variant) {
                    auto ty = ty2->isPointerTy() ? ty2->getSequentialElementType() : ty2;
                    operand1 = comp->calling_cast(ty, operand1);
                } else {
                    operand1 = comp->builder->CreateLoad(operand1);
                }
            }

            if (ty2->isPointerTy()) {
                if (ty2->getSequentialElementType() == comp->variant) {
                    auto ty = ty1->isPointerTy() ? ty1->getSequentialElementType() : ty1;
                    operand2 = comp->calling_cast(ty, operand2);
                } else {
                    operand2 = comp->builder->CreateLoad(operand2);
                }
            }
        }

        //DUMP_TY("binary-operand1: ", operand1->getType());
        //DUMP_TY("binary-operand2: ", operand2->getType());

        assert(operand1->getType() == operand2->getType() && "binary operator must have operands of the same type");

        auto binres = comp->builder->CreateBinOp(op, operand1, operand2, "binres");

        //std::clog << "\t"; binres->getType()->dump();

        return binres;
    }

    Value *expr_compiler::compare(CmpInst::Predicate predicate, Value *operand1, Value *operand2)
    {
        auto ty1 = operand1->getType();
        auto ty2 = operand2->getType();

        if (ty1->isIntegerTy() && ty2->isIntegerTy()) {
            if (ty1 != ty2) {
                if (ty1->getIntegerBitWidth() < ty2->getIntegerBitWidth()) {
                    operand1 = comp->builder->CreateIntCast(operand1, ty2, false);
                } else {
                    operand2 = comp->builder->CreateIntCast(operand2, ty1, false);
                }
            }
            return comp->builder->CreateICmpEQ(operand1, operand2);
        } else if (ty1->isFloatingPointTy() && ty2->isFloatingPointTy()) {
            if (ty1 != ty2) operand2 = comp->builder->CreateFPCast(operand2, ty1);
            return comp->builder->CreateFCmpOEQ(operand1, operand2);
        }

        DUMP_TY("compare: ", ty1);
        DUMP_TY("compare: ", ty2);
        errs()
            << "lyre: unsupported types for comparation"
            << "\n" ;
        return nullptr;
    }

    static bool llvm_init_done = false;

    void compiler::Init()
    {
        if (llvm_init_done) return;
        llvm_init_done = true;

        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();
    }

    void compiler::Shutdown()
    {
        llvm_shutdown();
    }

    static void say(const char * s, ...)
    {
        va_list ap;
        va_start(ap, s);
        vprintf(s, ap);
        va_end(ap);
        printf("\n");
    }

    static std::unordered_map<std::string, void*> LyreLazyFunctionMap = {
        std::pair<std::string, void*>("say", reinterpret_cast<void*>(&say))
    };

    // FIXME: http://llvm.org/PR5184 (thread safe issue)
    static void* LyreLazyFunctionCreator(const std::string & name)
    {
        auto i = LyreLazyFunctionMap.find(name);
        if (i != LyreLazyFunctionMap.end()) return i->second;
        return nullptr;
    }

    compiler::compiler()
        : context()
        , variant(
            StructType::create(
                "type.lyre.variant",
                ArrayType::get(Type::getInt8Ty(context), 8),
                PointerType::getUnqual(Type::getInt8Ty(context)),
                nullptr
            )
        )
        , nodetype(
            StructType::create(context, "type.lyre.node")
        )
        , typemap({
                //std::pair<std::string, Type*>("float16", Type::getHalfTy(context)),
                //std::pair<std::string, Type*>("float32", Type::getFloatTy(context)),
                //std::pair<std::string, Type*>("float64", Type::getDoubleTy(context)),
                std::pair<std::string, Type*>("float",  Type::getDoubleTy(context)),
                std::pair<std::string, Type*>("int",    IntegerType::get(context, 32)),
                std::pair<std::string, Type*>("bool",   Type::getInt1Ty(context)),
          })
        , error()
        , module(nullptr)
        , engine(nullptr)
        , builder(nullptr)
    {
        reinterpret_cast<StructType*>(nodetype)->setBody(
            Type::getInt8PtrTy(context),        // node (tag) name
            Type::getInt32Ty(context),          // number of children
            PointerType::getUnqual(nodetype),   // children
            PointerType::getUnqual(nodetype),   // parent
            nullptr
        );

        std::unique_ptr<Module> m(new Module("a", context));

        // Keep the reference to the module
        module = m.get();

        // Now we create the JIT.
        auto jit = EngineBuilder(std::move(m))
            .setErrorStr(&error)
#if LYRE_USING_MCJIT
            .setMCJITMemoryManager(std::unique_ptr<SectionMemoryManager>(new SectionMemoryManager))
#endif
            .create();

#if LYRE_USING_MCJIT
        jit->setProcessAllSections(true);
#endif

        jit->InstallLazyFunctionCreator(LyreLazyFunctionCreator);

        auto dataLayout = jit->getDataLayout();
        module->setDataLayout(*dataLayout);

        if (1) {
            std::vector<Type *> params = { Type::getInt8PtrTy(context) };
            FunctionType *FT = FunctionType::get(Type::getVoidTy(context), params, true);
            Function *F = Function::Create(FT, Function::ExternalLinkage, "say", module);
            F->arg_begin()->setName("s");
        }

        std::tuple<int,int,int> a;

        engine.reset(jit);
    }

    GenericValue compiler::eval(const ast::stmts & stmts)
    {
        GenericValue gv;

        if (!compile(stmts)) {
            std::clog << "lyre: failed to compile statements"  << std::endl;
            return gv;
        }

        // Print out all of the generated code.
#if 1
        module->dump();
#else
        outs() << "\n" << *module << "\n";
        outs().flush();
#endif

        auto start = module->getFunction("lyre.start"); // lyre·start
        if (!start) {
            std::clog << "no module start point"  << std::endl;
            return gv;
        }

        if (!engine) {
            errs() << "Could not create ExecutionEngine: " << error << "\n";
            return gv;
        }

        std::clog
            << "--------------------------------------\n"
            << "Running: " << start->getName().str() << "\n"
            << "--------------------------------------\n"
            << std::flush
            ;

        //engine->generateCodeForModule(module);

        // Ensure the module is fully processed and is usable. It has no effect for the interpeter.
        engine->finalizeObject();

        engine->runStaticConstructorsDestructors(/* isDtors = */false);

        // Call the `foo' function with no arguments:
#if 0
        std::vector<GenericValue> noargs;
        gv = engine->runFunction(start, noargs);
#else
        std::vector<std::string> argv = { "a" };
        const char * const * envp = nullptr;
        auto status = engine->runFunctionAsMain(start, argv, envp);
#endif

        engine->runStaticConstructorsDestructors(/* isDtors = */true);

        return gv;
    }

    Value* compiler::get_variant_storage(Value *value)
    {
        auto valueTy = value->getType();
        assert((valueTy == variant || valueTy->getSequentialElementType() == variant) && "value is not a variant");

        //DUMP_TY("value-type: ", valueTy);

        /**
         *  Get pointer to the variant storage.
         *
         *  %type.lyre.variant = type { [8 x i8], i8* }
         */
        auto zero = builder->getInt32(0);
        auto idxz = std::vector<Value*>{ zero, zero, zero };

        if (valueTy == variant) {
            ///< TODO: need a better way to arrange this temporary alloca.
            ///< a temporary 'variant' is required for getting the element pointer.
            auto alloca = builder->CreateAlloca(variant, builder->getInt32(0));

            ///< store a copy of the 'variant' instance
            builder->CreateStore(value, alloca);

            value = alloca; ///< redirect the 'value' to the new 'alloca'
        }

        value = builder->CreateGEP(value, idxz);

        //DUMP_TY("result-type: ", value->getType());
        return value;
    }

    Value* compiler::calling_cast(Type * destTy, Value * value)
    {
        if (isa<Argument>(value)) {
            auto arg = cast<Argument>(value);
            auto varName = arg->getName().str() + ".addr";
            if ((value = arg->getParent()->getValueSymbolTable().lookup(varName)) == nullptr) {
                errs()
                    << "lyre: argument#" << arg->getArgNo() << " " << arg->getName().str()
                    << " don't have an address"
                    << "\n" ;
                return nullptr;
            }
        }

        auto valueTy = value->getType();

        if (destTy == nullptr) {
            if (valueTy->isPointerTy()) {
                return builder->CreateLoad(value);
            }
            return value;
        }

        auto cc = CallingCast::getCallingCaster(destTy->getTypeID(), valueTy->getTypeID());

        //DUMP_TY("target-type: ", destTy);
        //DUMP_TY("value-type: ", valueTy);

        assert(cc && "no calling caster");

        return cc(this, destTy, value);
    }

    Type* compiler::find_type(const std::string & name)
    {
        // Also see:
        //      std::vector<StructType *> getIdentifiedStructTypes() const;
        auto structTy = module->getTypeByName("type.lyre." + name);
        if (structTy != nullptr) {
            // DUMP_TY("type: ", structTy);
            return structTy;
        }

        // Searching custom Lyre types:
        structTy = module->getTypeByName("type." + name);
        if (structTy != nullptr) {
            // DUMP_TY("type: ", structTy);
            return structTy;
        }

        // Searching builtin alias:
        auto t = typemap.find(name);
        if (t == typemap.end()) return nullptr;
        return t->second;
    }

    Value* compiler::compile(const ast::stmts & stmts)
    {
        // Create the ~start function entry and insert this entry into module M.
        // The '0' terminates the list of argument types.
        auto start = cast<Function>(
            module->getOrInsertFunction("lyre.start"
                , Type::getInt32Ty(context)
                , Type::getInt32Ty(context)
                , PointerType::getUnqual(Type::getInt8PtrTy(context))
                , static_cast<Type*>(nullptr)
            )
        );

        //start->getFunctionType()->getParamType(0)->setName("argc");
        //start->getFunctionType()->getParamType(1)->setName("argv");
        start->arg_begin()->setName("argc");
        (++start->arg_begin())->setName("argv");

        start->setGC("lygc");

        auto entry = BasicBlock::Create(context, "entry", start);
        builder = std::unique_ptr<IRBuilder<>>(new IRBuilder<>(entry));

        if (stmts.empty()) {
            builder->CreateRet(builder->getInt32(0));
            return start;
        }

        auto block = BasicBlock::Create(context, "top", start);
        builder->CreateBr(block);
        builder->SetInsertPoint(block);

        Value *last = nullptr;
        for (auto & stmt : stmts) {
            if (!(last = boost::apply_visitor(*this, stmt))) {
                builder->CreateRet(builder->getInt32(0));
                return nullptr;
            }
        }

        // If the block is well formed, we don't need to add 'ret'.
        if (builder->GetInsertBlock()->getTerminator()) return start;

#if 0
        if (last == nullptr) {
            builder->CreateRet(builder->getInt32(0));
        } else if (last->getType()->isPointerTy()) {
            last = builder->CreateLoad(last, "res");
        }

        if (last->getType()->isIntegerTy()) {
            builder->CreateRet(builder->CreateLoad(last, "res"));
        } else {
            builder->CreateRet(builder->getInt32(0));
        }
#else
        builder->CreateRet(builder->getInt32(0));
#endif

        return start;
    }

    Value* compiler::create_alloca(Type *Ty, Value *ArraySize, const std::string &Name)
    {
        auto & entry = builder->GetInsertBlock()->getParent()->getEntryBlock();
        IRBuilder<> allocaBuilder(&entry, entry.begin());
        return allocaBuilder.CreateAlloca(Ty, ArraySize, Name.c_str());
    }

    Value* compiler::compile_expr(const ast::expr & expr)
    {
        expr_compiler excomp{ this };
        auto value = excomp.compile(expr);
        return value;
    }

    Value* compiler::operator()(const ast::expr & expr)
    {
        // TODO: accepts invocation only...
        return compile_expr(expr);
    }

    Value* compiler::operator()(const ast::none &)
    {
        D("none: ");
        return nullptr;
    }

    Value* compiler::operator()(const ast::decl & decl)
    {
        auto & entry = builder->GetInsertBlock()->getParent()->getEntryBlock();
        auto allocaBuilder = IRBuilder<>{ &entry, entry.begin() };
        Value* lastAlloca = nullptr;
        for (auto & sym : decl) {
            if (sym.id.string == "_") {
                errs()
                    << "lyre: symbol '_' is reserved"
                    << "\n"
                    ;
                return nullptr;
            }

            auto type = variant; ///< The default type is 'variant'.

            if (sym.type) {
                auto typeName = boost::get<ast::identifier>(sym.type).string;
                if ((type = find_type(typeName)) == nullptr) {
                    errs()
                        << "lyre: decl " << sym.id.string << " as unknown type '" << typeName << "'"
                        << "\n" ;
                    return nullptr;
                }
            }

            Value* value = nullptr;
            if (sym.expr) {
                if ((value = compile_expr(sym.expr))) {
                    /**
                     *  Use the value type if no explicit type specified.
                     */
                    if (!sym.type) type = value->getType();
                }
            }

            /**
             *  Get a PointerTy of new alloca.
             */
            auto alloca = allocaBuilder.CreateAlloca(type, nullptr, sym.id.string.c_str());
            if (value) builder->CreateStore(value, alloca);

            lastAlloca = alloca;
        }
        return lastAlloca;
    }

    Value* compiler::operator()(const ast::proc & proc)
    {
        auto rty = Type::getVoidTy(context);
        if (proc.type) {
            auto id = boost::get<ast::identifier>(proc.type);
            if ((rty = find_type(id.string)) == nullptr) {
                errs()
                    << "lyre: " << proc.name.string << ": unknown return type '" << id.string << "'"
                    << "\n" ;
                return nullptr;
            }
        }

        std::vector<Type*> params;
        for (auto & param : proc.params) {
            auto type = variant;
            if (param.type) {
                auto & id = boost::get<ast::identifier>(param.type);
                if ((type = find_type(id.string)) == nullptr) {
                    errs()
                        << "lyre: " << proc.name.string << "used an unknown parameter type '"
                        << id.string << "' referenced by '" << param.name.string << "'"
                        << "\n" ;
                    return nullptr;
                }
            }
            if (type->isStructTy()) {
                params.push_back(PointerType::getUnqual(type));
            } else {
                params.push_back(type);
            }
        }

        auto fty = FunctionType::get(rty, params, false);

        // ExternalLinkage, InternalLinkage, PrivateLinkage
        auto fun = Function::Create(fty, Function::PrivateLinkage, proc.name.string, module);
        auto arg = fun->arg_begin();
        for (auto & param : proc.params) {
            assert (arg != fun->arg_end());
            arg->setName(param.name.string);
            ++arg;
        }

        fun->setGC("lygc");

        auto savedInsertBlock = builder->GetInsertBlock();

        if (nullptr == this->compile_body(fun, proc.block.stmts)) {
            // No function body, remove function.
            fun->eraseFromParent();
            return nullptr;
        }

        builder->SetInsertPoint(savedInsertBlock);
        return fun;
    }

    Value* compiler::compile_body(Function * fun, const ast::stmts & stmts)
    {
        auto rty = fun->getReturnType();

        auto entry = BasicBlock::Create(context, "entry", fun);
        builder->SetInsertPoint(entry);

        /**
         *  Create allocas on the frame for arguments. 
         */
        for (auto arg = fun->arg_begin(); arg != fun->arg_end(); ++arg) {
            auto name = arg->getName().str() + ".addr";
            auto argAddr = builder->CreateAlloca(arg->getType(), builder->getInt32(0), name.c_str());
            builder->CreateStore(arg, argAddr);
        }

        if (stmts.empty()) {
            return builder->CreateRetVoid();
        }

        auto block = BasicBlock::Create(context, "block", fun);
        builder->CreateBr(block);
        builder->SetInsertPoint(block);

        Value *last = nullptr;
        for (auto & stmt : stmts) {
            if (!(last = boost::apply_visitor(*this, stmt))) {
                builder->CreateRetVoid();
                return nullptr;
            }
        }

        if (builder->GetInsertBlock()->getTerminator()) return last;

        if (rty->isVoidTy()) return builder->CreateRetVoid();
        if (last) {
            auto fty = fun->getFunctionType();
            auto lty = last->getType();
            if (lty->isPointerTy() && fty->isValidReturnType(lty->getPointerElementType()))
                return builder->CreateRet(builder->CreateLoad(last, "res"));
            if (fty->isValidReturnType(lty))
                return builder->CreateRet(last);
        }
        return builder->CreateRetVoid();
    }

    Value* compiler::operator()(const ast::type & s)
    {
        std::clog << "type: " << std::endl;
        return nullptr;
    }

    Value* compiler::operator()(const ast::see & s)
    {
        auto seeValue = compile_expr(s.expr);
        if (seeValue == nullptr) {
            errs()
                << "lyre: sees an invalid expression"
                << "\n" ;
            return nullptr;
        }

        auto bbOuter = builder->GetInsertBlock();
        auto fun = bbOuter->getParent();

        ///< Put all blocks in a list
        auto astBlocks = std::vector<const ast::xblock*>{ &s.block0 };
        for (auto & b : s.blocks) { astBlocks.push_back(&b); }

        auto bbMerge = BasicBlock::Create(context, "saw.stop");

        auto numBlocks = astBlocks.size();
        auto blocks = std::vector<BasicBlock*>{};
        for (auto n = 0; n < numBlocks; ++n) {
            auto bbSaw = BasicBlock::Create(context, "saw");
            auto bbCont = BasicBlock::Create(context, "saw.cont");
            auto astBlock = astBlocks[n];

            blocks.push_back(bbSaw);

            Value *caseValue = nullptr;
            if (astBlock->expr) {
                auto & expr = boost::get<ast::expr>(astBlock->expr);
                if ((caseValue = compile_expr(expr)) == nullptr && 0 < expr.operators.size()) {
                    errs()
                        << "lyre: invalid expression "
                        << ", operators = " << expr.operators.size()
                        << "\n" ;
                    return nullptr;
                }
            }

            if (caseValue == nullptr) {
                auto state = 0;
                switch (n) {
                case 0:  state = 1;  break;
                case 1:  state = 0;  break;
                default: state = -1; break;
                }
                caseValue = ConstantInt::get(seeValue->getType(), state);
            }

            ///< Remove pointers
            seeValue = calling_cast(nullptr, seeValue);
            caseValue = calling_cast(nullptr, caseValue);

            auto seeValueTy = seeValue->getType();
            auto caseValueTy = caseValue->getType();
            Value *cond = nullptr;

            if (seeValueTy->isIntegerTy() && caseValueTy->isIntegerTy()) {
                if (seeValueTy != caseValueTy)
                    caseValue = builder->CreateIntCast(caseValue, seeValueTy, false);
                cond = builder->CreateICmpEQ(seeValue, caseValue);
            } else if (seeValueTy->isFloatingPointTy() && caseValueTy->isFloatingPointTy()) {
                if (seeValueTy != caseValueTy)
                    caseValue = builder->CreateFPCast(caseValue, seeValueTy);
                cond = builder->CreateFCmpOEQ(seeValue, caseValue);
            } else {
                DUMP_TY("saw: ", seeValueTy);
                DUMP_TY("saw: ", caseValueTy);
                errs()
                    << "lyre: unsupported see types"
                    << "\n" ;
                return nullptr;
            }

            builder->CreateCondBr(cond, bbSaw, bbCont);

            ///< Emit the block statements
            fun->getBasicBlockList().push_back(bbSaw);
            builder->SetInsertPoint(bbSaw);
            for (auto & stmt : astBlock->stmts) {
                auto v = boost::apply_visitor(*this, stmt);
            }
            builder->CreateBr(bbMerge);

            ///< update for PHI (the block might be changed)
            blocks[n] = builder->GetInsertBlock();

            ///< Emit the continual saw block
            fun->getBasicBlockList().push_back(bbCont);
            builder->SetInsertPoint(bbCont);
        }

        ///< Terminate the final continual block
        builder->CreateBr(bbMerge);

        ///< Emit the merge block
        fun->getBasicBlockList().push_back(bbMerge);
        builder->SetInsertPoint(bbMerge);

        /*
       ///< Emit PHI node in the merge block
       Value *bbSawV = builder->getInt1(1);
       PHINode *pn = builder->CreatePHI(bbSawV->getType(), 2, "see.tmp");
       pn->addIncoming(bbSawV, bbOuter);
       pn->addIncoming(bbSawV, bbSaw);
        */

        return blocks[0];
    }

    Value* compiler::operator()(const ast::with & s)
    {
        std::clog << "with: " << std::endl;
        return nullptr;
    }

    Value* compiler::operator()(const ast::speak & s)
    {
        std::clog << "speak: " << std::endl;
        return nullptr;
    }

    Value* compiler::operator()(const ast::per & s)
    {
        return nullptr;
    }

    Value* compiler::operator()(const ast::ret & ret)
    {
        auto fun = builder->GetInsertBlock()->getParent();
        auto rty = fun->getReturnType();
        if (rty->isVoidTy()) {
            if (ret.expr) {
                errs()
                    << "lyre: procedure '" << fun->getName().str() << "' returns 'void' only"
                    << "\n" ;
                return nullptr;
            }
            return builder->CreateRetVoid();
        }

        auto value = compile_expr(ret.expr);
        if (!value) {
            errs()
                << "lyre: proc " << fun->getName().str() << " must return a value"
                << "\n" ;
            return nullptr;
        }

        value = calling_cast(rty, value);

        //DUMP_TY("return-type: ", rty);
        //DUMP_TY("return-value-type: ", value->getType());

        assert(value->getType() == rty && "return type mismatched");

        return builder->CreateRet(value);
    }
}
