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
  OS << "typedef std::string     String;\n";
}

void emitMessageStructs(const std::vector<Record*> &Messages, raw_ostream &OS)
{
  emitBasicTypedefs(OS);
  
  OS << "\n";

  // Define message structs.
  for (auto M : Messages) {
    OS << "struct " << M->getName() << " {\n" ;

    auto Fields = M->getValueAsListOfDefs("FIELDS");
    for (auto F : Fields) {
      auto T = F->getSuperClasses().back();
      OS << "  " << T->getName() ;
      OS << " " << F->getValueAsString("NAME") << ";\n";
    }
      
    OS << "};\n\n" ;
  }
}
  
void emitProtocol(const std::vector<Record*> &Messages, raw_ostream &OS)
{
  // Define message tag.
  OS << "enum class tag : " << (Messages.size() < 256 ? "Uint8" : "Uint16") << "\n" ;
  OS << "{\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    OS << "  " << M->getName() << " = " << M->getValueAsInt("ID") << ", \n";
  }
  OS << "};\n\n" ;

  OS << "constexpr std::size_t tag_size = sizeof(tag);\n\n" ;

  // Define message codec.
  OS << "template <class P, class accessor>\n" ;
  OS << "struct codec\n" ;
  OS << "{\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto MSG = M->getName();

    // A comment line for the message.
    OS << "  // Message: "<<MSG<<"\n";

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
    OS << "  static std::size_t decode(P *p, const "<<MSG<<" &m)\n";
    OS << "  {\n" ;
    for (std::size_t I = 0, S = Fields.size(); I < S; ++I) {
      auto F = Fields[I]->getValueAsString("NAME");
      OS << "    accessor::get(p, m." << F << ");\n" ;
    }
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
  OS << "\n" ;
  OS << "  template<class C, class M>\n" ;
  OS << "  void process_message(C *c, M &m) { c->process_message(m); }\n" ;
  OS << "\n";
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto S = M->getName();
    OS << "  template<class C>" ;
    OS << " void process_message(C*, "<<S<<" &m) {}\n" ;
  }
  OS << "}; // end struct protocol\n\n" ;
}

void emitStateMachine(const std::vector<Record*> &States, 
    const std::vector<Record*> &Events, raw_ostream &OS)
{
  for (std::size_t EI = 0, EE = Events.size(); EI < EE; ++EI) {
    auto E = Events[EI];
    auto S = E->getName();
    OS << "struct event_"<<S<<" : sc::event<event_"<<S<<">\n" ;
    OS << "{\n" ;
    
    OS << "};\n" ;
    OS << "\n" ;
  }
}
}

namespace lyre
{
  void EmitMessagingDriverCC(RecordKeeper &Records, raw_ostream &OS)
  {
    std::vector<Record*> Messages = Records.getAllDerivedDefinitions("Message");
    std::vector<Record*> States = Records.getAllDerivedDefinitions("State");
    std::vector<Record*> Events = Records.getAllDerivedDefinitions("Event");

    emitSourceFileHeader("The Protocol Engine.", OS);

    auto & Namespace = getOptNamespace();
    auto & SharedHeader = getOptSharedHeader();

    if (!SharedHeader.empty())
      OS << "#include \"" << SharedHeader << "\"\n" ;
    OS << "#include \"messaging.inc\"\n\n" ;

    if (!Namespace.empty())
      OS << "using namespace " << Namespace << ";\n" ;
    
    OS << "\n" ;
    OS << "namespace\n" ;
    OS << "{\n" ;

    if (SharedHeader.empty())
      emitMessageStructs(Messages, OS);    
    
    emitProtocol(Messages, OS);
    emitStateMachine(States, Events, OS);
    
    OS << "} // end anonymous namespace\n" ;
  }

  void EmitMessagingDriverHH(RecordKeeper &Records, raw_ostream &OS)
  {
    std::vector<Record*> Messages = Records.getAllDerivedDefinitions("Message");
    std::vector<Record*> States = Records.getAllDerivedDefinitions("State");
    std::vector<Record*> Events = Records.getAllDerivedDefinitions("Event");

    emitSourceFileHeader("The Protocol Engine.", OS);

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
