#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <cctype>
#include <map>
#include <set>
#include <string>

using namespace llvm;

namespace 
{
void emitMessageCodec(const std::vector<Record*> &Messages, raw_ostream &OS)
{
  if (Messages.size() < 256)
    OS << "typedef Uint8 tag_value_base;" ;
  else
    OS << "typedef Uint16 tag_value_base;" ;

  OS << "\n\n";

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
  OS << "private:\n" ;
  OS << "  codec() = delete;\n" ;
  OS << "  ~codec() = delete;\n" ;
  OS << "  void operator=(const codec &) = delete;\n" ;
  OS << "}; // end struct codec\n\n" ;
}
}

namespace lyre
{
  void EmitMessagingDriver(RecordKeeper &Records, raw_ostream &OS)
  {
    std::vector<Record*> Messages = Records.getAllDerivedDefinitions("Message");
    std::vector<Record*> States = Records.getAllDerivedDefinitions("State");
    std::vector<Record*> Events = Records.getAllDerivedDefinitions("Event");

    emitSourceFileHeader("The Protocol Engine.", OS);

    OS << "#include \"messaging.inc\"\n\n" ;
    OS << "namespace\n" ;
    OS << "{\n" ;

    emitMessageCodec(Messages, OS);
    // TODO: emitStateMachine(States, Events, OS);
    
    OS << "} // end anonymous namespace\n" ;
  }
} // end namespace lyre
