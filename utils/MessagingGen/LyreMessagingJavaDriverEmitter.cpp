#include "MessagingGenBackends.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <cctype>
#include <map>
#include <set>
#include <string>

using namespace llvm;

namespace 
{
}

static std::string toMacroName(const std::string & S)
{
  std::string T(S);
  for (std::size_t I = 0, E = S.size(); I < E; ++I) {
    if (std::ispunct(T[I]) || std::isspace(T[I])) T[I] = '_';
    if (std::islower(T[I])) T[I] = std::toupper(T[I]);
  }
  return T;
}

static std::string TypeName(const std::string & S)
{
  if (S == "Int8" || S == "Uint8") return "byte";
  if (S == "Int16" || S == "Uint16") return "short";
  if (S == "Int32" || S == "Uint32") return "int";
  if (S == "Int64" || S == "Uint64") return "long";
  if (S == "TinyString" || S == "ShortString" || S == "LongString")
    return "byte[]"; // UTF-8 bytes
  return "unknown";
}

static std::string TypeSize(const std::string & S)
{
  if (S == "Int8" || S == "Uint8") return "1";
  if (S == "Int16" || S == "Uint16") return "2";
  if (S == "Int32" || S == "Uint32") return "4";
  if (S == "Int64" || S == "Uint64") return "8";
  if (S == "TinyString" || S == "ShortString" || S == "LongString")
    return S;
  return "0";
}

static void emitFieldSizeExpr(raw_ostream &OS, const std::string &S, const std::string &N)
{
  if (S == "Int8" || S == "Uint8")   OS << "1 /* m."<<N<<" */" ;
  if (S == "Int16" || S == "Uint16") OS << "2 /* m."<<N<<" */" ;
  if (S == "Int32" || S == "Uint32") OS << "4 /* m."<<N<<" */" ;
  if (S == "Int64" || S == "Uint64") OS << "8 /* m."<<N<<" */" ;
  if (S == "TinyString") OS << "1 + m."<<N<<".length" ;
  if (S == "ShortString") OS << "2 + m."<<N<<".length" ;
  if (S == "LongString") OS << "4 + m."<<N<<".length" ;
    
}

static void emitFieldEncodingCode(raw_ostream &OS, const std::string &S, const std::string &N)
{
  if (S == "Int8" || S == "Uint8") {
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 0);\n";
  }
  if (S == "Int16" || S == "Uint16") {
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 8);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 0);\n";
  }
  if (S == "Int32" || S == "Uint32") {
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 24);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 16);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 8);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 0);\n";
  }
  if (S == "Int64" || S == "Uint64") {
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 56);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 48);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 40);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 32);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 24);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 16);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 8);\n";
    OS << "        data[off++] = (byte)(m."<<N<<" >>> 0);\n";
  }
  if (S == "TinyString") {
    OS << "        data[off++] = (byte)(m."<<N<<".length >>> 0);\n";
    OS << "        System.arraycopy(m."<<N<<", 0, data, off, m."<<N<<".length);\n";
    OS << "        off += m."<<N<<".length;\n";
  }
  if (S == "ShortString") {
    OS << "        data[off++] = (byte)(m."<<N<<".length >>> 8);\n";
    OS << "        data[off++] = (byte)(m."<<N<<".length >>> 0);\n";
    OS << "        System.arraycopy(m."<<N<<", 0, data, off, m."<<N<<".length);\n";
    OS << "        off += m."<<N<<".length;\n";
  }
  if (S == "LongString") {
    OS << "        data[off++] = (byte)(m."<<N<<".length >>> 24);\n";
    OS << "        data[off++] = (byte)(m."<<N<<".length >>> 16);\n";
    OS << "        data[off++] = (byte)(m."<<N<<".length >>> 8);\n";
    OS << "        data[off++] = (byte)(m."<<N<<".length >>> 0);\n";
    OS << "        System.arraycopy(m."<<N<<", 0, data, off, m."<<N<<".length);\n";
    OS << "        off += m."<<N<<".length;\n";
  }
}
static void emitFieldEncodingCode(raw_ostream &OS, Record *F)
{
  auto T = F->getSuperClasses().back();
  auto N = F->getValueAsString("NAME");
  auto S = T->getName();
  emitFieldEncodingCode(OS, S, N);
}

static void emitFieldDecodingCode(raw_ostream &OS, const std::string &S, const std::string &N)
{
  if (S == "Int8" || S == "Uint8") {
    OS << "        m."<<N<<" += data[off++];\n";
  }
  if (S == "Int16" || S == "Uint16") {
    OS << "        {\n";
    OS << "        short v = 0;\n";
    OS << "        v += (short)(data[off++] << 8);\n";
    OS << "        v += (short)(data[off++] << 0);\n";
    OS << "        m."<<N<<" = v;\n";
    OS << "        }\n";
  }
  if (S == "Int32" || S == "Uint32") {
    OS << "        {\n";
    OS << "        int v = 0;\n";
    OS << "        v += (int)(data[off++] << 24);\n";
    OS << "        v += (int)(data[off++] << 16);\n";
    OS << "        v += (int)(data[off++] << 8);\n";
    OS << "        v += (int)(data[off++] << 0);\n";
    OS << "        m."<<N<<" = v;\n";
    OS << "        }\n";
  }
  if (S == "Int64" || S == "Uint64") {
    OS << "        {\n";
    OS << "        long v = 0;\n";
    OS << "        v += (long)(data[off++] << 56);\n";
    OS << "        v += (long)(data[off++] << 48);\n";
    OS << "        v += (long)(data[off++] << 40);\n";
    OS << "        v += (long)(data[off++] << 32);\n";
    OS << "        v += (long)(data[off++] << 24);\n";
    OS << "        v += (long)(data[off++] << 16);\n";
    OS << "        v += (long)(data[off++] << 8);\n";
    OS << "        v += (long)(data[off++] << 0);\n";
    OS << "        m."<<N<<" = v;\n";
    OS << "        }\n";
  }
  if (S == "TinyString") {
    OS << "        m."<<N<<" = new byte[data[off++]];\n";
    OS << "        System.arraycopy(data, off, m."<<N<<", 0, m."<<N<<".length);\n";
    OS << "        off += m."<<N<<".length;\n";
  }
  if (S == "ShortString") {
    OS << "        {\n";
    OS << "        short v = 0;\n";
    OS << "        v += (short)(data[off++] << 8);\n";
    OS << "        v += (short)(data[off++] << 0);\n";
    OS << "        m."<<N<<" = new byte[v];\n";
    OS << "        System.arraycopy(data, off, m."<<N<<", 0, m."<<N<<".length);\n";
    OS << "        off += m."<<N<<".length;\n";
    OS << "        }\n";
  }
  if (S == "LongString") {
    OS << "        {\n";
    OS << "        int v = 0;\n";
    OS << "        v += (int)(data[off++] << 24);\n";
    OS << "        v += (int)(data[off++] << 16);\n";
    OS << "        v += (int)(data[off++] << 8);\n";
    OS << "        v += (int)(data[off++] << 0);\n";
    OS << "        m."<<N<<" = new byte[v];\n";
    OS << "        System.arraycopy(data, off, m."<<N<<", 0, m."<<N<<".length);\n";
    OS << "        off += m."<<N<<".length;\n";
    OS << "        }\n";
  }
}

static void emitFieldDecodingCode(raw_ostream &OS, Record *F)
{
  auto T = F->getSuperClasses().back();
  auto N = F->getValueAsString("NAME");
  auto S = T->getName();
  emitFieldDecodingCode(OS, S, N);
}

static void emitMessageDefines(const std::vector<Record*> &Messages, raw_ostream &OS)
{
  OS << "\n";

  bool HasUserDefinedErrorMessage = false;

  OS << "    public static abstract class Message {\n" ;
  OS << "    }\n\n" ;

  // Define message structs.
  for (auto M : Messages) {
    if (M->getName() == "error") HasUserDefinedErrorMessage = true;

    OS << "    public static final class " << M->getName() << " extends Message {\n" ;

    auto Fields = M->getValueAsListOfDefs("FIELDS");
    for (auto F : Fields) {
      auto T = F->getSuperClasses().back();
      OS << "        public " << TypeName(T->getName())
         << " " << F->getValueAsString("NAME")
         << ";\n";
    }
      
    OS << "    }\n\n" ;
  }
  if (!HasUserDefinedErrorMessage) {
    OS << "    public static final class error extends Message {\n" ;
    OS << "        public "<<TypeName("Uint16")<<" code;\n" ;
    OS << "        public "<<TypeName("TinyString")<<" text;\n" ;
    OS << "    }\n\n" ;
  }

  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto Fields = M->getValueAsListOfDefs("FIELDS");
    OS << "    public static int getMessageSize(" << M->getName() << " m) {\n" ;
    if (Fields.empty()) {
      OS << "        return 0;\n" ;
    } else {
      for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
        auto F = Fields[FI];
        auto T = F->getSuperClasses().back();
        auto N = F->getValueAsString("NAME");
        auto S = T->getName();
        OS << "        " ;
        OS << (FI == 0 ? "return " : "    +  ") ;
        emitFieldSizeExpr(OS, S, N);
        OS << (FI + 1 == FE ? ";\n" : "\n") ;
      }
    }
    OS << "    }\n" ;
    OS << "\n" ;
    OS << "    public static int encodeMessage(final byte[] data, int off, final " << M->getName() << " m) {\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      emitFieldEncodingCode(OS, Fields[FI]);
    }
    OS << "        return off;\n" ;
    OS << "    }\n" ;
    OS << "    public static int decodeMessage(final byte[] data, int off, final " << M->getName() << " m) {\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      emitFieldDecodingCode(OS, Fields[FI]);
    }
    OS << "        return off;\n" ;
    OS << "    }\n" ;
    OS << "\n" ;
  }
  if (!HasUserDefinedErrorMessage) {
    OS << "    public static int getMessageSize(error m) {\n" ;
    OS << "        return " ; emitFieldSizeExpr(OS, "Uint16", "code"); OS << "\n" ;
    OS << "            +  " ; emitFieldSizeExpr(OS, "TinyString", "text"); OS << ";\n" ;
    OS << "    }\n" ;
    OS << "\n" ;
    OS << "    public static void encodeMessage(byte[] data, int off, error m) {\n" ;
    emitFieldEncodingCode(OS, "Uint16", "code");
    emitFieldEncodingCode(OS, "TinyString", "text");
    OS << "    }\n" ;
    OS << "\n" ;
    OS << "    public static void decodeMessage(byte[] data, int off, error m) {\n" ;
    emitFieldDecodingCode(OS, "Uint16", "code");
    emitFieldDecodingCode(OS, "TinyString", "text");
    OS << "    }\n" ;
    OS << "\n" ;
  }
  
  
}

static void emitProtocols(const std::vector<Record*> &Protocols,
    const std::vector<Record*> &Messages, raw_ostream &OS)
{
  return;
  
  auto TagBase = Messages.size() < 256 ? "Uint8" : "Uint16";

  OS << "// Protocols: " << Protocols.size() << "\n" ;
  for (auto P : Protocols) {
    auto Req = P->getValueAsDef("REQ");
    auto Rep = P->getValueAsDef("REP");
    OS << "//    " << P->getName() << ": "
       << Req->getName() << " -> " << Rep->getName()
       << "\n" ;
  }

  bool HasUserDefinedErrorMessage = false;
  auto ErrorID = 0;
  
  // Define message tag.
  OS << "enum class tag : " << TagBase << "\n" ;
  OS << "{\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto ID = M->getValueAsInt("ID");
    OS << "  " << M->getName() << " = " << ID << ", \n";
    if (M->getName() == "error") HasUserDefinedErrorMessage = true;
    if (ID == ErrorID) ErrorID += 1;
  }
  if (!HasUserDefinedErrorMessage)
    OS << "  error = " << ErrorID << "\n";
  OS << "};\n\n" ;

  OS << "using ERROR = struct error;\n\n" ;
  OS << "constexpr std::size_t tag_size = sizeof(tag);\n\n" ;

  // Define message codec.
  OS << "template <class P, class accessor>\n" ;
  OS << "struct codec\n" ;
  OS << "{\n" ;
  OS << "  typedef " << TagBase << " tag_value_t;\n";
  OS << "  typedef ::tag tag_t;\n\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto MSG = M->getName();

    // A comment line for the message.
    OS << "  // Message: "<<MSG<<"\n";

    // static tag_t t(const MESSAGE &);
    OS << "  static tag_t t(const "<<MSG<<"&) { return tag::"<<MSG<<"; }\n" ;
    
    // static std::size_t size(P *p, const MESSAGE &m);
    OS << "  static std::size_t size(P *p, const "<<MSG<<" &m)\n" ;
    OS << "  {\n" ;
    auto Fields = M->getValueAsListOfDefs("FIELDS");
    if (Fields.empty()) {
      OS << "    return 0;\n";
    } else {
      for (std::size_t I = 0, S = Fields.size(); I < S; ++I) {
        auto F = Fields[I]->getValueAsString("NAME");
        OS << (I == 0 ? "    return " : "      +    ") ;
        OS << "accessor::field_size(p, m." << F << ")" ;
        OS << (I + 1 == S ? ";\n" : "\n") ;
      }
    }
    OS << "  }\n" ;

    // static void encode (P *p, const MESSAGE & m);
    OS << "  static std::size_t encode(P *p, const "<<MSG<<" &m)\n";
    OS << "  {\n" ;
    for (std::size_t I = 0, S = Fields.size(); I < S; ++I) {
      auto F = Fields[I]->getValueAsString("NAME");
      OS << "    accessor::put(p, m." << F << ");\n" ;
    }
    OS << "  }\n" ;

    // static void decode (P *p, const MESSAGE & m);
    OS << "  static std::size_t decode(P *p, "<<MSG<<" &m)\n";
    OS << "  {\n" ;
    for (std::size_t I = 0, S = Fields.size(); I < S; ++I) {
      auto F = Fields[I]->getValueAsString("NAME");
      OS << "    accessor::get(p, m." << F << ");\n" ;
    }
    OS << "  }\n\n" ;
  }
  if (!HasUserDefinedErrorMessage) {
    OS << "  // Message: error\n";
    OS << "  static tag_t t(const ERROR&) { return tag::error; }\n" ;
    OS << "  static std::size_t size(P *p, const ERROR &m)\n" ;
    OS << "  {\n" ;
    OS << "    return accessor::field_size(p, m.code)\n" ;
    OS << "      +    accessor::field_size(p, m.text);\n" ;
    OS << "  }\n" ;
    OS << "  static std::size_t encode(P *p, const ERROR &m)\n";
    OS << "  {\n" ;
    OS << "    accessor::put(p, m.code);\n" ;
    OS << "    accessor::put(p, m.text);\n" ;
    OS << "  }\n" ;
    OS << "  static std::size_t decode(P *p, ERROR &m)\n";
    OS << "  {\n" ;
    OS << "    accessor::get(p, m.code);\n" ;
    OS << "    accessor::get(p, m.text);\n" ;
    OS << "  }\n\n" ;
  }
  
  // static void parse (P *p, Context *ctx)
  OS << "  template < class Message, class Context >\n" ;
  OS << "  static void parse(P *p, Context *ctx)\n" ;
  OS << "  {\n" ;
  OS << "    Message m;\n" ;
  OS << "    decode(p, m);\n" ;
  OS << "    p->process_message(ctx, m);\n" ;
  OS << "  }\n\n" ;

  // static bool parse (P *p, Context *ctx, tag t)
  OS << "  template < class Context >\n" ;
  OS << "  static bool parse(P *p, Context *ctx, tag t)\n" ;
  OS << "  {\n" ;
  OS << "    switch (t) {\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto S = M->getName();
    OS << "    case tag::"<<S<<": parse<"<<S<<">(p, ctx); return true; \n";
  }
  if (!HasUserDefinedErrorMessage)
    OS << "    case tag::error: parse<ERROR>(p, ctx); return true; \n";
  OS << "    }\n" ;
  OS << "    return false;\n" ;
  OS << "  }\n" ;
  OS << "\n" ;
  OS << "private:\n" ;
  OS << "  codec() = delete;\n" ;
  OS << "  ~codec() = delete;\n" ;
  OS << "  void operator=(const codec &) = delete;\n" ;
  OS << "}; // end struct codec\n\n" ;

  // The "protocol" definition.
  OS << "struct protocol : messaging::base_protocol<protocol, codec>\n" ;
  OS << "{\n" ;
  OS << "  explicit protocol(int type) : base_protocol(type) {}\n" ;
  OS << "}; // end struct protocol\n\n" ;

  // The "request_processor" definition.
  OS << "struct request_processor : messaging::base_processor<request_processor, codec>\n" ;
  OS << "{\n" ;
  OS << "  explicit request_processor(int type) : base_processor(type) {}\n" ;
  OS << "\n" ;
  OS << "  template <class Context>\n" ;
  OS << "  bool wait_process_request(Context *C) {\n" ;
  OS << "    auto okay = receive_and_process(C);\n" ;
  OS << "    if (!okay) { /*...*/ }\n" ;
  OS << "    return okay;\n" ;
  OS << "  }\n" ;
  OS << "\n" ;
  OS << "protected:\n" ;
  for (auto P : Protocols) {
    auto Req = P->getValueAsDef("REQ");
    auto Rep = P->getValueAsDef("REP");
    OS << "  virtual void on_request(const "<<Req->getName()<<" &Req, "
       << Rep->getName() << " &Rep) {}\n" ;
  }
  OS << "\n" ;
  OS << "  virtual void on_bad_request() {}\n" ;
  OS << "\n" ;
  OS << "  ERROR make_error(Uint16 n, const char *s) {\n" ;
  if (!HasUserDefinedErrorMessage) {
    OS << "    return ERROR{ n, s };\n" ;
  } else {
    OS << "    ERROR E;\n" ;
    OS << "    // TODO: init E;\n" ;
    OS << "    return E;\n" ;
  }
  OS << "  }\n" ;
  OS << "\n" ;
  OS << "private:\n" ;
  OS << "  friend codec;\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto S = M->getName();
    OS << "  template<class C>" ;
    OS << " void process_message(C*, const "<<S<<" &Q) {\n" ;
    auto C = 0;
    for (auto P : Protocols) {
      auto Req = P->getValueAsDef("REQ");
      auto Rep = P->getValueAsDef("REP");
      if (Req != M && Rep != M) continue;
      if (Req == M) {
        OS << "    {\n" ;
        OS << "      "<<Rep->getName()<<" P;\n" ;
        OS << "      on_request(Q, P);\n" ;
        OS << "      base_processor::send(P);\n" ;
        OS << "    }\n" ;
        C += 1;
      }
    }
    if (C == 0) {
      OS << "    {\n" ;
      OS << "      ERROR P = make_error(-1, \"bad\");\n" ;
      OS << "      on_bad_request();\n" ;
      OS << "      base_processor::send(P);\n" ;
      OS << "    }\n" ;
    }
    OS << "  }\n" ;
  }
  if (!HasUserDefinedErrorMessage) {
    OS << "  template<class C>" ;
    OS << " void process_message(C*, const ERROR &E) {\n" ;
    OS << "      ERROR P = make_error(-2, \"bad\");\n" ;
    OS << "      on_bad_request();\n" ;
    OS << "      base_processor::send(P);\n" ;
    OS << "  }\n" ;
  }
  OS << "}; // end struct request_processor\n\n" ;

  // The "reply_processor" definition.
  OS << "struct reply_processor : messaging::base_processor<reply_processor, codec>\n" ;
  OS << "{\n" ;
  OS << "  explicit reply_processor(int type) : base_processor(type) {}\n" ;
  OS << "\n" ;
  for (auto P : Protocols) {
    auto Req = P->getValueAsDef("REQ");
    OS << "  auto send(const "<<Req->getName()<<"&Q) {" ;
    OS << " return base_processor::send(Q); }\n" ;
  }
  OS << "\n" ;
  OS << "  template <class Context>\n" ;
  OS << "  bool wait_process_reply(Context *C) {\n" ;
  OS << "    auto okay = receive_and_process(C);\n" ;
  OS << "    if (!okay) { /*...*/ }\n" ;
  OS << "    return okay;\n" ;
  OS << "  }\n" ;
  OS << "\n" ;
  OS << "protected:\n" ;
  for (auto P : Protocols) {
    auto Req = P->getValueAsDef("REQ");
    auto Rep = P->getValueAsDef("REP");
    OS << "  virtual void on_reply(const "<<Rep->getName()<<" &Rep) {}\n" ;
  }
  if (true /*!HasUserDefinedErrorMessage*/) {
    OS << "  virtual void on_reply(const ERROR &E) {}\n" ;
  }
  OS << "\n" ;
  OS << "  virtual void on_bad_reply() {}\n" ;
  OS << "\n" ;
  OS << "private:\n" ;
  OS << "  friend codec;\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto S = M->getName();
    OS << "  template<class C>" ;
    OS << " void process_message(C*, const "<<S<<" &P) {\n" ;
    auto C = 0;
    for (auto P : Protocols) {
      auto Req = P->getValueAsDef("REQ");
      auto Rep = P->getValueAsDef("REP");
      if (Req != M && Rep != M) continue;
      if (Rep == M) {
        OS << "    on_reply(P);\n" ;
        C += 1;
      }
    }
    if (C == 0) {
      OS << "    on_bad_reply();\n" ;
    }
    OS << "  }\n" ;
  }
  if (true /*!HasUserDefinedErrorMessage*/) {
    OS << "  template<class C>" ;
    OS << " void process_message(C*, const ERROR &E) {\n" ;
    OS << "    on_reply(E);\n" ;
    OS << "  }\n" ;
  }
  OS << "}; // end struct reply_processor\n\n" ;
}

static void emitStateMachines(
    const std::vector<Record*> &Machines,
    const std::vector<Record*> &States,
    const std::vector<Record*> &Events,
    raw_ostream &OS)
{
  for (std::size_t EI = 0, EE = Events.size(); EI < EE; ++EI) {
    auto E = Events[EI];
    auto N = E->getName();
    OS << "    // event: " <<N<< "\n" ;
  }
  OS << "\n" ;

  for (std::size_t SI = 0, SE = States.size(); SI < SE; ++SI) {
    auto S = States[SI];
    auto N = S->getName();
    OS << "    // state: "<<N<<";\n" ;
  }  
  OS << "\n" ;

  for (std::size_t MI = 0, ME = Machines.size(); MI < ME; ++MI) {
    auto M = Machines[MI];
    auto DirectSuper = M->getSuperClasses().back();
    OS << "// " << M->getName() << ", " << DirectSuper->getName() << "\n";
    for (auto super : M->getSuperClasses())
      OS << "//     " << super->getName() << "\n";
  }
  OS << "\n" ;
}

static inline void sortMessages(std::vector<Record*> &Messages)
{
  std::sort(Messages.begin(), Messages.end(), [](Record *A, Record *B){
      return A->getValueAsInt("ID") < B->getValueAsInt("ID");
    });
}

namespace lyre
{
  void EmitMessagingDriverJ(RecordKeeper &Records, raw_ostream &OS)
  {
    std::vector<Record*> Protocols = Records.getAllDerivedDefinitions("Protocol");
    std::vector<Record*> Machines = Records.getAllDerivedDefinitions("StateMachine");
    std::vector<Record*> Messages = Records.getAllDerivedDefinitions("Message");
    std::vector<Record*> States = Records.getAllDerivedDefinitions("State");
    std::vector<Record*> Events = Records.getAllDerivedDefinitions("Event");

    sortMessages(Messages);
    
    auto & Namespace = getOptNamespace();

    if (Namespace.empty())
      OS << "package lyre;\n" ;
    else
      OS << "package " << Namespace << ";\n" ;

    OS << "\n" ;
    OS << "import java.nio.ByteBuffer;\n" ;
    OS << "import java.nio.channels.SelectableChannel;\n" ;
    OS << "import java.nio.charset.Charset;\n" ;
    
    OS << "\n" ;
    OS << "public class messaging\n" ;
    OS << "{\n" ;
    OS << "    private messaging() {}\n" ;
    OS << "\n" ;
    OS << "    public static final Charset CHARSET = Charset.forName(\"UTF-8\");\n" ;

    emitMessageDefines(Messages, OS);
    emitProtocols(Protocols, Messages, OS);
    emitStateMachines(Machines, States, Events, OS);
    
    OS << "}\n" ;
  }
} // end namespace lyre
