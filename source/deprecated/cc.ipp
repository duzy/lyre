/** -*- c++ -*-
 *  
 *  CallingCast
 *  
 */
namespace lyre
{
    typedef llvm::Value *(*CC)(compiler *comp, llvm::Type *target, llvm::Value *value);

    struct CallingCast
    {
        /*
        static llvm::Value *Void_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Void_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Half_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Float_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Double_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_FP80_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *FP128_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *PPC_FP128_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Label_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Metadata_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *X86_MMX_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Integer_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Function_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Struct_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Array_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Pointer_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Void(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Half(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Float(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Double(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Label(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Integer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Function(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Struct(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Array(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value);
        static llvm::Value *Vector_Vector(compiler *comp, llvm::Type *target, llvm::Value *value);
        */

        static llvm::Value *identical(compiler */*comp*/, llvm::Type *target, llvm::Value *value)
        {
            assert(value->getType() == target && "value type and target type is not identical");
            return value;
        }

        static llvm::Value *impossible(compiler */*comp*/, llvm::Type *target, llvm::Value *value)
        {
            assert(false && "value type and target type is not convertible");
            return value;
        }

        static llvm::Value *Void_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isVoidTy() && "value type is not Void");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Void_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVoidTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Half_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isHalfTy() && "target type is not Half");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Float_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFloatTy() && "target type is not Float");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isVoidTy() && "value type is not Void");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Double_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isDoubleTy() && "target type is not Double");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isVoidTy() && "value type is not Void");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_FP80_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_FP80Ty() && "target type is not X86_FP80");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isVoidTy() && "value type is not Void");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *FP128_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFP128Ty() && "target type is not FP128");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isVoidTy() && "value type is not Void");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *PPC_FP128_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPPC_FP128Ty() && "target type is not PPC_FP128");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isVoidTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Label_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isLabelTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Metadata_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isMetadataTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *X86_MMX_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isX86_MMXTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");

            auto valueTy = value->getType();

            //DUMP_TY("value-type: ", valueTy);

            if (valueTy == comp->variant) {
                //DUMP_TY("target-type: ", target);
                auto ptr = comp->get_variant_storage(value);
                //DUMP_TY("pointer-type: ", ptr->getType());
                ptr = comp->builder->CreatePointerCast(ptr, PointerType::getUnqual(target));
                //DUMP_TY("pointer-type: ", ptr->getType());
                return comp->builder->CreateLoad(ptr);
            }

            return value;
        }

        static llvm::Value *Integer_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Integer_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            //DUMP_TY("target-type: ", target);
            //DUMP_TY("value-type: ", value->getType());

            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");

            auto pointeeTy = value->getType()->getSequentialElementType();

            if (pointeeTy == target) {
                return comp->builder->CreateLoad(value);
            }

            //DUMP_TY("pointee-type: ", pointeeTy);

            if (pointeeTy == comp->variant) {
                auto ptr = comp->builder->CreatePointerCast(
                    comp->get_variant_storage(value), PointerType::getUnqual(target));
                //DUMP_TY("storage: ", ptr->getType());
                return comp->builder->CreateLoad(ptr);
            }

            return value;
        }

        static llvm::Value *Integer_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isIntegerTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Function_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isFunctionTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);

            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");

            DUMP_TY("target-type: ", target);
            DUMP_TY("value-type: ", value->getType());

            if (target == comp->variant) {
                D("variant");
            }

            return value;
        }

        static llvm::Value *Struct_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Struct_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");

            if (value->getType()->getSequentialElementType() == target) {
                return comp->builder->CreateLoad(value);
            }

            return value;
        }

        static llvm::Value *Struct_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isStructTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);

            auto valueTy = value->getType();

            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");

            return impossible(comp, target, value);
        }

        static llvm::Value *Array_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isArrayTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);

            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");

            return impossible(comp, target, value);
        }

        static llvm::Value *Pointer_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);

            //DUMP_TY("target-type: ", target);

            auto valueTy = value->getType();
            //DUMP_TY("value-type: ", valueTy);

            assert(valueTy->isPointerTy() && "value type is not Pointer");
            assert(target->isPointerTy() && "target type is not Void");

            auto pointeeTy = valueTy->getSequentialElementType();
            //DUMP_TY("pointee-type: ", pointeeTy);

            if (pointeeTy == comp->variant) {
                D("TODO: variant casting");
            }

#if 0
            /**
             *  CreatePointerCast can also convert a '[n x i8]*' into 'i8*', so we don't need
             *  to do this specially.
             */
            if (pointeeTy->isArrayTy()) {
                auto arrayElementTy = pointeeTy->getSequentialElementType();
                if  (arrayElementTy == target->getSequentialElementType()) {
                    /**
                     *  %0 = i8* getelementptr inbounds ([11 x i8]* @str1, i32 0, i32 0)
                     */
                    auto zero = comp->builder->getInt32(0);
                    auto idxs = std::vector<llvm::Value*>{ zero, zero };
                    return comp->builder->CreateGEP(value, idxs);
                }
            }
#endif

            /**
             *  Note that 'CreatePointerCast' can also convert '[n x i8]*' into 'i8*'.
             */
            auto result = comp->builder->CreatePointerCast(value, target);
            //DUMP_TY("result-type: ", result->getType());

            return result;
        }

        static llvm::Value *Pointer_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isPointerTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Void(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Half(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isHalfTy() && "value type is not Half");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Float(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isFloatTy() && "value type is not Float");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Double(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isDoubleTy() && "value type is not Double");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_X86_FP80(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isX86_FP80Ty() && "value type is not X86_FP80");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isFP128Ty() && "value type is not FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_PPC_FP128(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isPPC_FP128Ty() && "value type is not PPC_FP128");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Label(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isLabelTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Metadata(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isMetadataTy() && "value type is not Label");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_X86_MMX(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isX86_MMXTy() && "value type is not X86_MMX");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Integer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isIntegerTy() && "value type is not Integer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Function(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isFunctionTy() && "value type is not Function");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Struct(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isStructTy() && "value type is not Struct");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Array(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isArrayTy() && "value type is not Array");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Pointer(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isPointerTy() && "value type is not Pointer");
            return impossible(comp, target, value);
        }

        static llvm::Value *Vector_Vector(compiler *comp, llvm::Type *target, llvm::Value *value)
        {
            D(__FUNCTION__);
            assert(target->isVectorTy() && "target type is not Void");
            assert(value->getType()->isVectorTy() && "value type is not Vector");
            return impossible(comp, target, value);
        }

        static CC getCallingCaster(llvm::Type::TypeID target, llvm::Type::TypeID dest)
        {
            static CC CastMap[16][16] = { { nullptr } };
            if (CastMap[0][0] == nullptr) {
                std::memset(CastMap, 0, sizeof(CC) * 16 * 16);
                CastMap[llvm::Type::VoidTyID][llvm::Type::VoidTyID]             = &Void_Void;
                CastMap[llvm::Type::VoidTyID][llvm::Type::HalfTyID]             = &Void_Half;
                CastMap[llvm::Type::VoidTyID][llvm::Type::FloatTyID]            = &Void_Float;
                CastMap[llvm::Type::VoidTyID][llvm::Type::DoubleTyID]           = &Void_Double;
                CastMap[llvm::Type::VoidTyID][llvm::Type::X86_FP80TyID]         = &Void_X86_FP80;
                CastMap[llvm::Type::VoidTyID][llvm::Type::FP128TyID]            = &Void_FP128;
                CastMap[llvm::Type::VoidTyID][llvm::Type::PPC_FP128TyID]        = &Void_PPC_FP128;
                CastMap[llvm::Type::VoidTyID][llvm::Type::LabelTyID]            = &Void_Label;
                CastMap[llvm::Type::VoidTyID][llvm::Type::MetadataTyID]         = &Void_Metadata;
                CastMap[llvm::Type::VoidTyID][llvm::Type::X86_MMXTyID]          = &Void_X86_MMX;
                CastMap[llvm::Type::VoidTyID][llvm::Type::IntegerTyID]          = &Void_Integer;
                CastMap[llvm::Type::VoidTyID][llvm::Type::FunctionTyID]         = &Void_Function;
                CastMap[llvm::Type::VoidTyID][llvm::Type::StructTyID]           = &Void_Struct;
                CastMap[llvm::Type::VoidTyID][llvm::Type::ArrayTyID]            = &Void_Array;
                CastMap[llvm::Type::VoidTyID][llvm::Type::PointerTyID]          = &Void_Pointer;
                CastMap[llvm::Type::VoidTyID][llvm::Type::VectorTyID]           = &Void_Vector;

                CastMap[llvm::Type::HalfTyID][llvm::Type::VoidTyID]             = &Half_Void;
                CastMap[llvm::Type::HalfTyID][llvm::Type::HalfTyID]             = &Half_Half;
                CastMap[llvm::Type::HalfTyID][llvm::Type::FloatTyID]            = &Half_Float;
                CastMap[llvm::Type::HalfTyID][llvm::Type::DoubleTyID]           = &Half_Double;
                CastMap[llvm::Type::HalfTyID][llvm::Type::X86_FP80TyID]         = &Half_X86_FP80;
                CastMap[llvm::Type::HalfTyID][llvm::Type::FP128TyID]            = &Half_FP128;
                CastMap[llvm::Type::HalfTyID][llvm::Type::PPC_FP128TyID]        = &Half_PPC_FP128;
                CastMap[llvm::Type::HalfTyID][llvm::Type::LabelTyID]            = &Half_Label;
                CastMap[llvm::Type::HalfTyID][llvm::Type::MetadataTyID]         = &Half_Metadata;
                CastMap[llvm::Type::HalfTyID][llvm::Type::X86_MMXTyID]          = &Half_X86_MMX;
                CastMap[llvm::Type::HalfTyID][llvm::Type::IntegerTyID]          = &Half_Integer;
                CastMap[llvm::Type::HalfTyID][llvm::Type::FunctionTyID]         = &Half_Function;
                CastMap[llvm::Type::HalfTyID][llvm::Type::StructTyID]           = &Half_Struct;
                CastMap[llvm::Type::HalfTyID][llvm::Type::ArrayTyID]            = &Half_Array;
                CastMap[llvm::Type::HalfTyID][llvm::Type::PointerTyID]          = &Half_Pointer;
                CastMap[llvm::Type::HalfTyID][llvm::Type::VectorTyID]           = &Half_Vector;

                CastMap[llvm::Type::FloatTyID][llvm::Type::VoidTyID]            = &Float_Void;
                CastMap[llvm::Type::FloatTyID][llvm::Type::HalfTyID]            = &Float_Half;
                CastMap[llvm::Type::FloatTyID][llvm::Type::FloatTyID]           = &Float_Float;
                CastMap[llvm::Type::FloatTyID][llvm::Type::DoubleTyID]          = &Float_Double;
                CastMap[llvm::Type::FloatTyID][llvm::Type::X86_FP80TyID]        = &Float_X86_FP80;
                CastMap[llvm::Type::FloatTyID][llvm::Type::FP128TyID]           = &Float_FP128;
                CastMap[llvm::Type::FloatTyID][llvm::Type::PPC_FP128TyID]       = &Float_PPC_FP128;
                CastMap[llvm::Type::FloatTyID][llvm::Type::LabelTyID]           = &Float_Label;
                CastMap[llvm::Type::FloatTyID][llvm::Type::MetadataTyID]        = &Float_Metadata;
                CastMap[llvm::Type::FloatTyID][llvm::Type::X86_MMXTyID]         = &Float_X86_MMX;
                CastMap[llvm::Type::FloatTyID][llvm::Type::IntegerTyID]         = &Float_Integer;
                CastMap[llvm::Type::FloatTyID][llvm::Type::FunctionTyID]        = &Float_Function;
                CastMap[llvm::Type::FloatTyID][llvm::Type::StructTyID]          = &Float_Struct;
                CastMap[llvm::Type::FloatTyID][llvm::Type::ArrayTyID]           = &Float_Array;
                CastMap[llvm::Type::FloatTyID][llvm::Type::PointerTyID]         = &Float_Pointer;
                CastMap[llvm::Type::FloatTyID][llvm::Type::VectorTyID]          = &Float_Vector;

                CastMap[llvm::Type::DoubleTyID][llvm::Type::VoidTyID]           = &Double_Void;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::HalfTyID]           = &Double_Half;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::FloatTyID]          = &Double_Float;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::DoubleTyID]         = &identical;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::X86_FP80TyID]       = &Double_X86_FP80;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::FP128TyID]          = &Double_FP128;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::PPC_FP128TyID]      = &Double_PPC_FP128;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::LabelTyID]          = &Double_Label;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::MetadataTyID]       = &Double_Metadata;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::X86_MMXTyID]        = &Double_X86_MMX;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::IntegerTyID]        = &Double_Integer;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::FunctionTyID]       = &Double_Function;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::StructTyID]         = &Double_Struct;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::ArrayTyID]          = &Double_Array;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::PointerTyID]        = &Double_Pointer;
                CastMap[llvm::Type::DoubleTyID][llvm::Type::VectorTyID]         = &Double_Vector;

                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::VoidTyID]         = &X86_FP80_Void;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::HalfTyID]         = &X86_FP80_Half;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::FloatTyID]        = &X86_FP80_Float;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::DoubleTyID]       = &X86_FP80_Double;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::X86_FP80TyID]     = &X86_FP80_X86_FP80;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::FP128TyID]        = &X86_FP80_FP128;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::PPC_FP128TyID]    = &X86_FP80_PPC_FP128;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::LabelTyID]        = &X86_FP80_Label;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::MetadataTyID]     = &X86_FP80_Metadata;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::X86_MMXTyID]      = &X86_FP80_X86_MMX;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::IntegerTyID]      = &X86_FP80_Integer;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::FunctionTyID]     = &X86_FP80_Function;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::StructTyID]       = &X86_FP80_Struct;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::ArrayTyID]        = &X86_FP80_Array;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::PointerTyID]      = &X86_FP80_Pointer;
                CastMap[llvm::Type::X86_FP80TyID][llvm::Type::VectorTyID]       = &X86_FP80_Vector;

                CastMap[llvm::Type::FP128TyID][llvm::Type::VoidTyID]            = &FP128_Void;
                CastMap[llvm::Type::FP128TyID][llvm::Type::HalfTyID]            = &FP128_Half;
                CastMap[llvm::Type::FP128TyID][llvm::Type::FloatTyID]           = &FP128_Float;
                CastMap[llvm::Type::FP128TyID][llvm::Type::DoubleTyID]          = &FP128_Double;
                CastMap[llvm::Type::FP128TyID][llvm::Type::X86_FP80TyID]        = &FP128_X86_FP80;
                CastMap[llvm::Type::FP128TyID][llvm::Type::FP128TyID]           = &identical;
                CastMap[llvm::Type::FP128TyID][llvm::Type::PPC_FP128TyID]       = &FP128_PPC_FP128;
                CastMap[llvm::Type::FP128TyID][llvm::Type::LabelTyID]           = &FP128_Label;
                CastMap[llvm::Type::FP128TyID][llvm::Type::MetadataTyID]        = &FP128_Metadata;
                CastMap[llvm::Type::FP128TyID][llvm::Type::X86_MMXTyID]         = &FP128_X86_MMX;
                CastMap[llvm::Type::FP128TyID][llvm::Type::IntegerTyID]         = &FP128_Integer;
                CastMap[llvm::Type::FP128TyID][llvm::Type::FunctionTyID]        = &FP128_Function;
                CastMap[llvm::Type::FP128TyID][llvm::Type::StructTyID]          = &FP128_Struct;
                CastMap[llvm::Type::FP128TyID][llvm::Type::ArrayTyID]           = &FP128_Array;
                CastMap[llvm::Type::FP128TyID][llvm::Type::PointerTyID]         = &FP128_Pointer;
                CastMap[llvm::Type::FP128TyID][llvm::Type::VectorTyID]          = &FP128_Vector;

                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::VoidTyID]        = &PPC_FP128_Void;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::HalfTyID]        = &PPC_FP128_Half;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::FloatTyID]       = &PPC_FP128_Float;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::DoubleTyID]      = &PPC_FP128_Double;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::X86_FP80TyID]    = &PPC_FP128_X86_FP80;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::FP128TyID]       = &PPC_FP128_FP128;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::PPC_FP128TyID]   = &PPC_FP128_PPC_FP128;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::LabelTyID]       = &PPC_FP128_Label;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::MetadataTyID]    = &PPC_FP128_Metadata;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::X86_MMXTyID]     = &PPC_FP128_X86_MMX;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::IntegerTyID]     = &PPC_FP128_Integer;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::FunctionTyID]    = &PPC_FP128_Function;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::StructTyID]      = &PPC_FP128_Struct;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::ArrayTyID]       = &PPC_FP128_Array;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::PointerTyID]     = &PPC_FP128_Pointer;
                CastMap[llvm::Type::PPC_FP128TyID][llvm::Type::VectorTyID]      = &PPC_FP128_Vector;
            
                CastMap[llvm::Type::LabelTyID][llvm::Type::VoidTyID]            = &Label_Void;
                CastMap[llvm::Type::LabelTyID][llvm::Type::HalfTyID]            = &Label_Half;
                CastMap[llvm::Type::LabelTyID][llvm::Type::FloatTyID]           = &Label_Float;
                CastMap[llvm::Type::LabelTyID][llvm::Type::DoubleTyID]          = &Label_Double;
                CastMap[llvm::Type::LabelTyID][llvm::Type::X86_FP80TyID]        = &Label_X86_FP80;
                CastMap[llvm::Type::LabelTyID][llvm::Type::FP128TyID]           = &Label_FP128;
                CastMap[llvm::Type::LabelTyID][llvm::Type::PPC_FP128TyID]       = &Label_PPC_FP128;
                CastMap[llvm::Type::LabelTyID][llvm::Type::LabelTyID]           = &Label_Label;
                CastMap[llvm::Type::LabelTyID][llvm::Type::MetadataTyID]        = &Label_Metadata;
                CastMap[llvm::Type::LabelTyID][llvm::Type::X86_MMXTyID]         = &Label_X86_MMX;
                CastMap[llvm::Type::LabelTyID][llvm::Type::IntegerTyID]         = &Label_Integer;
                CastMap[llvm::Type::LabelTyID][llvm::Type::FunctionTyID]        = &Label_Function;
                CastMap[llvm::Type::LabelTyID][llvm::Type::StructTyID]          = &Label_Struct;
                CastMap[llvm::Type::LabelTyID][llvm::Type::ArrayTyID]           = &Label_Array;
                CastMap[llvm::Type::LabelTyID][llvm::Type::PointerTyID]         = &Label_Pointer;
                CastMap[llvm::Type::LabelTyID][llvm::Type::VectorTyID]          = &Label_Vector;

                CastMap[llvm::Type::MetadataTyID][llvm::Type::VoidTyID]         = &Metadata_Void;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::HalfTyID]         = &Metadata_Half;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::FloatTyID]        = &Metadata_Float;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::DoubleTyID]       = &Metadata_Double;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::X86_FP80TyID]     = &Metadata_X86_FP80;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::FP128TyID]        = &Metadata_FP128;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::PPC_FP128TyID]    = &Metadata_PPC_FP128;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::LabelTyID]        = &Metadata_Label;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::MetadataTyID]     = &Metadata_Metadata;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::X86_MMXTyID]      = &Metadata_X86_MMX;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::IntegerTyID]      = &Metadata_Integer;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::FunctionTyID]     = &Metadata_Function;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::StructTyID]       = &Metadata_Struct;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::ArrayTyID]        = &Metadata_Array;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::PointerTyID]      = &Metadata_Pointer;
                CastMap[llvm::Type::MetadataTyID][llvm::Type::VectorTyID]       = &Metadata_Vector;

                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::VoidTyID]          = &X86_MMX_Void;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::HalfTyID]          = &X86_MMX_Half;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::FloatTyID]         = &X86_MMX_Float;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::DoubleTyID]        = &X86_MMX_Double;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::X86_FP80TyID]      = &X86_MMX_X86_FP80;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::FP128TyID]         = &X86_MMX_FP128;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::PPC_FP128TyID]     = &X86_MMX_PPC_FP128;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::LabelTyID]         = &X86_MMX_Label;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::MetadataTyID]      = &X86_MMX_Metadata;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::X86_MMXTyID]       = &X86_MMX_X86_MMX;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::IntegerTyID]       = &X86_MMX_Integer;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::FunctionTyID]      = &X86_MMX_Function;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::StructTyID]        = &X86_MMX_Struct;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::ArrayTyID]         = &X86_MMX_Array;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::PointerTyID]       = &X86_MMX_Pointer;
                CastMap[llvm::Type::X86_MMXTyID][llvm::Type::VectorTyID]        = &X86_MMX_Vector;

                CastMap[llvm::Type::IntegerTyID][llvm::Type::VoidTyID]          = &Integer_Void;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::HalfTyID]          = &Integer_Half;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::FloatTyID]         = &Integer_Float;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::DoubleTyID]        = &Integer_Double;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::X86_FP80TyID]      = &Integer_X86_FP80;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::FP128TyID]         = &Integer_FP128;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::PPC_FP128TyID]     = &Integer_PPC_FP128;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::LabelTyID]         = &Integer_Label;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::MetadataTyID]      = &Integer_Metadata;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::X86_MMXTyID]       = &Integer_X86_MMX;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::IntegerTyID]       = &identical;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::FunctionTyID]      = &Integer_Function;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::StructTyID]        = &Integer_Struct;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::ArrayTyID]         = &Integer_Array;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::PointerTyID]       = &Integer_Pointer;
                CastMap[llvm::Type::IntegerTyID][llvm::Type::VectorTyID]        = &Integer_Vector;

                CastMap[llvm::Type::FunctionTyID][llvm::Type::VoidTyID]         = &Function_Void;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::HalfTyID]         = &Function_Half;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::FloatTyID]        = &Function_Float;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::DoubleTyID]       = &Function_Double;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::X86_FP80TyID]     = &Function_X86_FP80;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::FP128TyID]        = &Function_FP128;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::PPC_FP128TyID]    = &Function_PPC_FP128;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::LabelTyID]        = &Function_Label;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::MetadataTyID]     = &Function_Metadata;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::X86_MMXTyID]      = &Function_X86_MMX;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::IntegerTyID]      = &Function_Integer;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::FunctionTyID]     = &identical;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::StructTyID]       = &Function_Struct;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::ArrayTyID]        = &Function_Array;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::PointerTyID]      = &Function_Pointer;
                CastMap[llvm::Type::FunctionTyID][llvm::Type::VectorTyID]       = &Function_Vector;

                CastMap[llvm::Type::StructTyID][llvm::Type::VoidTyID]           = &Struct_Void;
                CastMap[llvm::Type::StructTyID][llvm::Type::HalfTyID]           = &Struct_Half;
                CastMap[llvm::Type::StructTyID][llvm::Type::FloatTyID]          = &Struct_Float;
                CastMap[llvm::Type::StructTyID][llvm::Type::DoubleTyID]         = &Struct_Double;
                CastMap[llvm::Type::StructTyID][llvm::Type::X86_FP80TyID]       = &Struct_X86_FP80;
                CastMap[llvm::Type::StructTyID][llvm::Type::FP128TyID]          = &Struct_FP128;
                CastMap[llvm::Type::StructTyID][llvm::Type::PPC_FP128TyID]      = &Struct_PPC_FP128;
                CastMap[llvm::Type::StructTyID][llvm::Type::LabelTyID]          = &Struct_Label;
                CastMap[llvm::Type::StructTyID][llvm::Type::MetadataTyID]       = &Struct_Metadata;
                CastMap[llvm::Type::StructTyID][llvm::Type::X86_MMXTyID]        = &Struct_X86_MMX;
                CastMap[llvm::Type::StructTyID][llvm::Type::IntegerTyID]        = &Struct_Integer;
                CastMap[llvm::Type::StructTyID][llvm::Type::FunctionTyID]       = &Struct_Function;
                CastMap[llvm::Type::StructTyID][llvm::Type::StructTyID]         = &identical;
                CastMap[llvm::Type::StructTyID][llvm::Type::ArrayTyID]          = &Struct_Array;
                CastMap[llvm::Type::StructTyID][llvm::Type::PointerTyID]        = &Struct_Pointer;
                CastMap[llvm::Type::StructTyID][llvm::Type::VectorTyID]         = &Struct_Vector;

                CastMap[llvm::Type::ArrayTyID][llvm::Type::VoidTyID]            = &Array_Void;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::HalfTyID]            = &Array_Half;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::FloatTyID]           = &Array_Float;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::DoubleTyID]          = &Array_Double;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::X86_FP80TyID]        = &Array_X86_FP80;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::FP128TyID]           = &Array_FP128;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::PPC_FP128TyID]       = &Array_PPC_FP128;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::LabelTyID]           = &Array_Label;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::MetadataTyID]        = &Array_Metadata;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::X86_MMXTyID]         = &Array_X86_MMX;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::IntegerTyID]         = &Array_Integer;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::FunctionTyID]        = &Array_Function;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::StructTyID]          = &Array_Struct;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::ArrayTyID]           = &Array_Array;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::PointerTyID]         = &Array_Pointer;
                CastMap[llvm::Type::ArrayTyID][llvm::Type::VectorTyID]          = &Array_Vector;

                CastMap[llvm::Type::PointerTyID][llvm::Type::VoidTyID]          = &Pointer_Void;
                CastMap[llvm::Type::PointerTyID][llvm::Type::HalfTyID]          = &Pointer_Half;
                CastMap[llvm::Type::PointerTyID][llvm::Type::FloatTyID]         = &Pointer_Float;
                CastMap[llvm::Type::PointerTyID][llvm::Type::DoubleTyID]        = &Pointer_Double;
                CastMap[llvm::Type::PointerTyID][llvm::Type::X86_FP80TyID]      = &Pointer_X86_FP80;
                CastMap[llvm::Type::PointerTyID][llvm::Type::FP128TyID]         = &Pointer_FP128;
                CastMap[llvm::Type::PointerTyID][llvm::Type::PPC_FP128TyID]     = &Pointer_PPC_FP128;
                CastMap[llvm::Type::PointerTyID][llvm::Type::LabelTyID]         = &Pointer_Label;
                CastMap[llvm::Type::PointerTyID][llvm::Type::MetadataTyID]      = &Pointer_Metadata;
                CastMap[llvm::Type::PointerTyID][llvm::Type::X86_MMXTyID]       = &Pointer_X86_MMX;
                CastMap[llvm::Type::PointerTyID][llvm::Type::IntegerTyID]       = &Pointer_Integer;
                CastMap[llvm::Type::PointerTyID][llvm::Type::FunctionTyID]      = &Pointer_Function;
                CastMap[llvm::Type::PointerTyID][llvm::Type::StructTyID]        = &Pointer_Struct;
                CastMap[llvm::Type::PointerTyID][llvm::Type::ArrayTyID]         = &Pointer_Array;
                CastMap[llvm::Type::PointerTyID][llvm::Type::PointerTyID]       = &Pointer_Pointer;
                CastMap[llvm::Type::PointerTyID][llvm::Type::VectorTyID]        = &Pointer_Vector;

                CastMap[llvm::Type::VectorTyID][llvm::Type::VoidTyID]           = &Vector_Void;
                CastMap[llvm::Type::VectorTyID][llvm::Type::HalfTyID]           = &Vector_Half;
                CastMap[llvm::Type::VectorTyID][llvm::Type::FloatTyID]          = &Vector_Float;
                CastMap[llvm::Type::VectorTyID][llvm::Type::DoubleTyID]         = &Vector_Double;
                CastMap[llvm::Type::VectorTyID][llvm::Type::X86_FP80TyID]       = &Vector_X86_FP80;
                CastMap[llvm::Type::VectorTyID][llvm::Type::FP128TyID]          = &Vector_FP128;
                CastMap[llvm::Type::VectorTyID][llvm::Type::PPC_FP128TyID]      = &Vector_PPC_FP128;
                CastMap[llvm::Type::VectorTyID][llvm::Type::LabelTyID]          = &Vector_Label;
                CastMap[llvm::Type::VectorTyID][llvm::Type::MetadataTyID]       = &Vector_Metadata;
                CastMap[llvm::Type::VectorTyID][llvm::Type::X86_MMXTyID]        = &Vector_X86_MMX;
                CastMap[llvm::Type::VectorTyID][llvm::Type::IntegerTyID]        = &Vector_Integer;
                CastMap[llvm::Type::VectorTyID][llvm::Type::FunctionTyID]       = &Vector_Function;
                CastMap[llvm::Type::VectorTyID][llvm::Type::StructTyID]         = &Vector_Struct;
                CastMap[llvm::Type::VectorTyID][llvm::Type::ArrayTyID]          = &Vector_Array;
                CastMap[llvm::Type::VectorTyID][llvm::Type::PointerTyID]        = &Vector_Pointer;
                CastMap[llvm::Type::VectorTyID][llvm::Type::VectorTyID]         = &Vector_Vector;
            }
            return CastMap[target][dest];
        }
    };
}
