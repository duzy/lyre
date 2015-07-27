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
std::string toMacroName(const std::string & S)
{
  std::string T(S);
  for (std::size_t I = 0, E = S.size(); I < E; ++I) {
    if (std::ispunct(T[I]) || std::isspace(T[I])) T[I] = '_';
    if (std::islower(T[I])) T[I] = std::toupper(T[I]);
  }
  return T;
}

void emitBasicTypedefs(raw_ostream &OS)
{
  OS << "typedef std::int8_t     Int8;\n";
  OS << "typedef std::int16_t    Int16;\n";
  OS << "typedef std::int32_t    Int32;\n";
  OS << "typedef std::int64_t    Int64;\n";
  OS << "typedef std::uint8_t    Uint8;\n";
  OS << "typedef std::uint16_t   Uint16;\n";
  OS << "typedef std::uint32_t   Uint32;\n";
  OS << "typedef std::uint64_t   Uint64;\n";
  OS << "struct TinyString : std::string {\n";
  OS << "  using std::string::string;\n";
  OS << "};\n";
  OS << "struct ShortString : std::string {\n";
  OS << "  using std::string::string;\n";
  OS << "};\n";
  OS << "struct LongString : std::string {\n";
  OS << "  using std::string::string;\n";
  OS << "};\n";
}

void emitMessageStructs(const std::vector<Record*> &Messages, raw_ostream &OS)
{
  emitBasicTypedefs(OS);
  
  OS << "\n";

  bool HasUserDefinedErrorMessage = false;

  // Define message structs.
  for (auto M : Messages) {
    if (M->getName() == "error") HasUserDefinedErrorMessage = true;

    OS << "struct " << M->getName() << " {\n" ;

    auto Fields = M->getValueAsListOfDefs("FIELDS");
    for (auto F : Fields) {
      auto T = F->getSuperClasses().back();
      OS << "  " << T->getName() ;
      OS << " " << F->getValueAsString("NAME") << ";\n";
    }
      
    OS << "};\n\n" ;
  }

  if (!HasUserDefinedErrorMessage) {
    OS << "struct error {\n" ;
    OS << "  Uint16 code;\n" ;
    OS << "  TinyString text;\n" ;
    OS << "};\n\n" ;
  }
}

void emitProtocols(const std::vector<Record*> &Protocols,
    const std::vector<Record*> &Messages, raw_ostream &OS)
{
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
  OS << "struct protocol : base_protocol<protocol, codec>\n" ;
  OS << "{\n" ;
  OS << "  explicit protocol(int type) : base_protocol(type) {}\n" ;
  OS << "}; // end struct protocol\n\n" ;

  // The "request_processor" definition.
  OS << "struct request_processor : base_processor<request_processor, codec>\n" ;
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
  OS << "struct reply_processor : base_processor<reply_processor, codec>\n" ;
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

} // end anonymous namespace

static inline void sortMessages(std::vector<Record*> &Messages)
{
  std::sort(Messages.begin(), Messages.end(), [](Record *A, Record *B){
      return A->getValueAsInt("ID") < B->getValueAsInt("ID");
    });
}

namespace lyre
{
  void EmitMessagingDriverCC(RecordKeeper &Records, raw_ostream &OS)
  {
    std::vector<Record*> Protocols = Records.getAllDerivedDefinitions("Protocol");
    std::vector<Record*> Machines = Records.getAllDerivedDefinitions("StateMachine");
    std::vector<Record*> Messages = Records.getAllDerivedDefinitions("Message");
    std::vector<Record*> States = Records.getAllDerivedDefinitions("State");
    std::vector<Record*> Events = Records.getAllDerivedDefinitions("Event");

    emitSourceFileHeader("The Protocol Engine.", OS);
    
    sortMessages(Messages);
    
    auto & Namespace = getOptNamespace();
    auto & SharedHeader = getOptSharedHeader();

    if (!SharedHeader.empty())
      OS << "#include \"" << SharedHeader << "\"\n" ;

    if (!Namespace.empty())
      OS << "using namespace " << Namespace << ";\n" ;
    OS << "\n" ;
    OS << "#include <boost/algorithm/string/split.hpp>\n" ;
    OS << "#include <boost/ptr_container/ptr_list.hpp>\n" ;
    OS << "#include <boost/noncopyable.hpp>\n" ;
    OS << "#include <cstdint>\n" ;
    OS << "#include <thread>\n" ;
    OS << "#include <chrono>\n" ;
    OS << "#include <memory>\n" ;
    OS << "#include <zmq.h>\n" ;
    OS << "#include <mutex>\n" ;
    OS << "#include <iostream>\n" ;
    OS << "\n" ;
    OS << "namespace { static std::mutex logmux; }\n" ;
    OS << "#define DBG(x)                                            \\\n" ;
    OS << "    do {                                                  \\\n" ;
    OS << "        std::lock_guard <std::mutex> lock (logmux);       \\\n" ;
    OS << "        std::clog << __FILE__ << \":\" << __LINE__ << \": \"  \\\n" ;
    OS << "                  << x << std::endl;                      \\\n" ;
    OS << "    } while (false);\n" ;
    OS << "\n" ;
    OS << "namespace meta = boost::mpl;\n" ;
    OS << "\n" ;
    OS << "namespace\n" ;
    OS << "{\n" ;
    OS << "static const struct zmq_wrapper\n" ;
    OS << "{\n" ;
    OS << "  zmq_wrapper() : the_context( zmq_ctx_new () ) {}\n" ;
    OS << "  ~zmq_wrapper() { zmq_ctx_term (the_context); }\n" ;
    OS << "  inline void *context() const { return the_context; }\n" ;
    OS << "private:\n" ;
    OS << "  void * the_context;\n" ;
    OS << "} zmq;\n" ;
    OS << "\n" ;
    OS << "#include \"messaging.inc\"\n" ;
    OS << "\n" ;

    if (SharedHeader.empty())
      emitMessageStructs(Messages, OS);    
    
    emitProtocols(Protocols, Messages, OS);
    
    OS << "} // end anonymous namespace\n" ;
  }

  void EmitMessagingDriverHH(RecordKeeper &Records, raw_ostream &OS)
  {
    std::vector<Record*> Messages = Records.getAllDerivedDefinitions("Message");
    std::vector<Record*> States = Records.getAllDerivedDefinitions("State");
    std::vector<Record*> Events = Records.getAllDerivedDefinitions("Event");

    emitSourceFileHeader("The Protocol Engine.", OS);

    sortMessages(Messages);
    
    auto & Namespace = getOptNamespace();
    auto OutputFilename = getOutputFilename();

    OS << "#ifndef __"<<toMacroName(OutputFilename)<<"__\n" ;
    OS << "#define __"<<toMacroName(OutputFilename)<<"__\n" ;
    OS << "#include <cstdint>\n" ;
    OS << "#include <string>\n" ;
    OS << "\n" ;
    
    if (!Namespace.empty())
      OS << "namespace " << Namespace << "\n{\n" ;
    
    emitMessageStructs(Messages, OS);
    
    if (!Namespace.empty())
      OS << "} // end namespace " << Namespace << "\n";

    OS << "#endif//__"<<toMacroName(OutputFilename)<<"__\n" ;
  }
} // end namespace lyre
