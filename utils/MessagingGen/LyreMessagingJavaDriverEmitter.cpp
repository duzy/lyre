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
    return "String"; //"byte[]"; // UTF-8 bytes
  return S;
}

static inline std::string Msg(const std::string &S)
{
  return "Message." + S ;
}

static inline std::string Msg(Record *M)
{
  return Msg(M->getName());
}

static void emitFieldSizeExpr_deprecated(raw_ostream &OS, const std::string &S, const std::string &N)
{
  if (S == "Int8" || S == "Uint8")   OS << "1 /* m."<<N<<" */" ;
  if (S == "Int16" || S == "Uint16") OS << "2 /* m."<<N<<" */" ;
  if (S == "Int32" || S == "Uint32") OS << "4 /* m."<<N<<" */" ;
  if (S == "Int64" || S == "Uint64") OS << "8 /* m."<<N<<" */" ;
  if (S == "TinyString") OS << "1 + m."<<N<<".length" ;
  if (S == "ShortString") OS << "2 + m."<<N<<".length" ;
  if (S == "LongString") OS << "4 + m."<<N<<".length" ;
    
}

static void emitFieldEncodingCode_deprecated(raw_ostream &OS, const std::string &S, const std::string &N)
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
static void emitFieldEncodingCode_deprecated(raw_ostream &OS, Record *F)
{
  auto T = F->getSuperClasses().back();
  auto N = F->getValueAsString("NAME");
  auto S = T->getName();
  emitFieldEncodingCode_deprecated(OS, S, N);
}

static void emitFieldDecodingCode_deprecated(raw_ostream &OS, const std::string &S, const std::string &N)
{
  if (S == "Int8" || S == "Uint8") {
    OS << "        m."<<N<<" += data[off++];\n";
  }
  if (S == "Int16" || S == "Uint16") {
    OS << "        m."<<N<<" = (short)((data[off+0] << 8) + (data[off+1] << 0));\n";
    OS << "        off += 2;\n";
  }
  if (S == "Int32" || S == "Uint32") {
    OS << "        m."<<N<<" = 0\n";
    OS << "            + (int)(data[off++] << 24)\n";
    OS << "            + (int)(data[off++] << 16)\n";
    OS << "            + (int)(data[off++] << 8)\n";
    OS << "            + (int)(data[off++] << 0);\n";
    OS << "        off += 4;\n";
  }
  if (S == "Int64" || S == "Uint64") {
    OS << "        m."<<N<<" = 0\n";
    OS << "            + (long)(data[off+0] << 56)\n";
    OS << "            + (long)(data[off+1] << 48)\n";
    OS << "            + (long)(data[off+2] << 40)\n";
    OS << "            + (long)(data[off+3] << 32)\n";
    OS << "            + (long)(data[off+4] << 24)\n";
    OS << "            + (long)(data[off+5] << 16)\n";
    OS << "            + (long)(data[off+6] << 8)\n";
    OS << "            + (long)(data[off+7] << 0);\n";
    OS << "        off += 8;\n";
  }
  if (S == "TinyString") {
    OS << "        m."<<N<<" = new byte[data[off++]];\n";
    OS << "        System.arraycopy(data, off, m."<<N<<", 0, m."<<N<<".length);\n";
    OS << "        off += m."<<N<<".length;\n";
  }
  if (S == "ShortString") {
    OS << "        m."<<N<<" = new byte[(data[off+0] << 8) + (data[off+1] << 0)];\n";
    OS << "        off += 2;\n";
    OS << "        System.arraycopy(data, off, m."<<N<<", 0, m."<<N<<".length);\n";
    OS << "        off += m."<<N<<".length;\n";
  }
  if (S == "LongString") {
    OS << "        m."<<N<<" = new byte[(data[off+0] << 24) + (data[off+1] << 16)\n";
    OS << "            + (data[off+2] << 8) + (data[off+3] << 0)];\n";
    OS << "        off += 4;\n";
    OS << "        System.arraycopy(data, off, m."<<N<<", 0, m."<<N<<".length);\n";
    OS << "        off += m."<<N<<".length;\n";
  }
}

static void emitFieldDecodingCode_deprecated(raw_ostream &OS, Record *F)
{
  auto T = F->getSuperClasses().back();
  auto N = F->getValueAsString("NAME");
  auto S = T->getName();
  emitFieldDecodingCode_deprecated(OS, S, N);
}

static void emitMessageDefines_deprecated(const std::vector<Record*> &Messages, raw_ostream &OS)
{
  bool HasUserDefinedErrorMessage = false;

  // Define message structs.
  OS << "    public static abstract class Message {\n" ;
  for (auto M : Messages) {
    if (M->getName() == "error") HasUserDefinedErrorMessage = true;

    OS << "        public static final class " << M->getName() << " extends Message {\n" ;

    auto Fields = M->getValueAsListOfDefs("FIELDS");
    for (auto F : Fields) {
      auto T = F->getSuperClasses().back();
      OS << "            public " << TypeName(T->getName())
         << " " << F->getValueAsString("NAME")
         << ";\n";
    }
      
    OS << "        }\n" ;
  }
  if (!HasUserDefinedErrorMessage) {
    OS << "        public static final class error extends Message {\n" ;
    OS << "            public "<<TypeName("Uint16")<<" code;\n" ;
    OS << "            public "<<TypeName("TinyString")<<" text;\n" ;
    OS << "        }\n" ;
  }
  OS << "    } // end class Message\n\n" ;
  
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto Fields = M->getValueAsListOfDefs("FIELDS");
    OS << "    public static int getMessageSize(" << Msg(M) << " m) {\n" ;
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
        emitFieldSizeExpr_deprecated(OS, S, N);
        OS << (FI + 1 == FE ? ";\n" : "\n") ;
      }
    }
    OS << "    }\n" ;
    OS << "    public static int encodeMessage(final byte[] data, int off, final " << Msg(M) << " m) {\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      emitFieldEncodingCode_deprecated(OS, Fields[FI]);
    }
    OS << "        return off;\n" ;
    OS << "    }\n" ;
    OS << "    public static int decodeMessage(final byte[] data, int off, final " << Msg(M) << " m) {\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      emitFieldDecodingCode_deprecated(OS, Fields[FI]);
    }
    OS << "        return off;\n" ;
    OS << "    }\n" ;
    OS << "\n" ;
  }
  if (!HasUserDefinedErrorMessage) {
    OS << "    public static int getMessageSize("<<Msg("error")<<" m) {\n" ;
    OS << "        return " ; emitFieldSizeExpr_deprecated(OS, "Uint16", "code"); OS << "\n" ;
    OS << "            +  " ; emitFieldSizeExpr_deprecated(OS, "TinyString", "text"); OS << ";\n" ;
    OS << "    }\n" ;
    OS << "    public static void encodeMessage(byte[] data, int off, "<<Msg("error")<<" m) {\n" ;
    emitFieldEncodingCode_deprecated(OS, "Uint16", "code");
    emitFieldEncodingCode_deprecated(OS, "TinyString", "text");
    OS << "    }\n" ;
    OS << "    public static void decodeMessage(byte[] data, int off, "<<Msg("error")<<" m) {\n" ;
    emitFieldDecodingCode_deprecated(OS, "Uint16", "code");
    emitFieldDecodingCode_deprecated(OS, "TinyString", "text");
    OS << "    }\n" ;
    OS << "\n" ;
  }
  
  
}

static void emitFieldEncodingCode(raw_ostream &OS, const std::string &S, const std::string &N)
{
  if (S == "Int8" || S == "Uint8") {
    OS << "                putNumber1(m."<<N<<");\n";
  }
  if (S == "Int16" || S == "Uint16") {
    OS << "                putNumber2(m."<<N<<");\n";
  }
  if (S == "Int32" || S == "Uint32") {
    OS << "                putNumber4(m."<<N<<");\n";
  }
  if (S == "Int64" || S == "Uint64") {
    OS << "                putNumber8(m."<<N<<");\n";
  }
  if (S == "TinyString") {
    OS << "                putTinyString(m."<<N<<");\n";
  }
  if (S == "ShortString") {
    OS << "                putTinyString(m."<<N<<");\n";
  }
  if (S == "LongString") {
    OS << "                putTinyString(m."<<N<<");\n";
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
    OS << "                    m."<<N<<" = getNumber1();\n";
  }
  if (S == "Int16" || S == "Uint16") {
    OS << "                    m."<<N<<" = getNumber2();\n";
  }
  if (S == "Int32" || S == "Uint32") {
    OS << "                    m."<<N<<" = getNumber4();\n";
  }
  if (S == "Int64" || S == "Uint64") {
    OS << "                    m."<<N<<" = getNumber8();\n";
  }
  if (S == "TinyString") {
    OS << "                    m."<<N<<" = getTinyString();\n";
  }
  if (S == "ShortString") {
    OS << "                    m."<<N<<" = getShortString();\n";
  }
  if (S == "LongString") {
    OS << "                    m."<<N<<" = getLongString();\n";
  }
}

static void emitFieldDecodingCode(raw_ostream &OS, Record *F)
{
  auto T = F->getSuperClasses().back();
  auto N = F->getValueAsString("NAME");
  auto S = T->getName();
  emitFieldDecodingCode(OS, S, N);
}

static void emitFieldSizeExpr(raw_ostream &OS, const std::string &S, const std::string &N)
{
  if (S == "Int8" || S == "Uint8")   OS << "1 /* m."<<N<<" */" ;
  if (S == "Int16" || S == "Uint16") OS << "2 /* m."<<N<<" */" ;
  if (S == "Int32" || S == "Uint32") OS << "4 /* m."<<N<<" */" ;
  if (S == "Int64" || S == "Uint64") OS << "8 /* m."<<N<<" */" ;
  if (S == "TinyString") OS << "1 + m."<<N<<".getBytes(CHARSET).length" ;
  if (S == "ShortString") OS << "2 + m."<<N<<".getBytes(CHARSET).length" ;
  if (S == "LongString") OS << "4 + m."<<N<<".getBytes(CHARSET).length" ;
}

static void emitFieldSizeExpr(raw_ostream &OS, Record *F)
{
  auto T = F->getSuperClasses().back();
  auto N = F->getValueAsString("NAME");
  auto S = T->getName();
  emitFieldSizeExpr(OS, S, N);
}

static void emitMessageDefines(const std::vector<Record*> &Messages, raw_ostream &OS)
{
  bool HasUserDefinedErrorMessage = false;
  auto ErrorID = 0;
  auto TagValueType = Messages.size() < 256 ? "Uint8" : "Uint16";
  auto TagValueSize = Messages.size() < 256 ? 1 : 2;

  // Define message structs.
  OS << "    public static abstract class Message\n" ;
  OS << "    {\n" ;
  for (auto M : Messages) {
    auto ID = M->getValueAsInt("ID");
    OS << "        public static final int tag_"
       << M->getName()  << " = " << ID << ";\n" ;
    if (M->getName() == "error") HasUserDefinedErrorMessage = true;
    if (ID == ErrorID) ErrorID += 1;
  }
  if (!HasUserDefinedErrorMessage) {
    OS << "        public static final int tag_error = " << ErrorID << ";\n" ;
  }
  OS << "\n" ;

  for (auto M : Messages) {
    OS << "        public static final class " << M->getName() << " extends Message {\n" ;

    auto Fields = M->getValueAsListOfDefs("FIELDS");
    for (auto F : Fields) {
      auto T = F->getSuperClasses().back();
      OS << "            public " << TypeName(T->getName())
         << " " << F->getValueAsString("NAME")
         << ";\n";
    }
      
    OS << "        }\n" ;
  }
  if (!HasUserDefinedErrorMessage) {
    OS << "        public static final class error extends Message {\n" ;
    OS << "            public "<<TypeName("Uint16")<<" code;\n" ;
    OS << "            public "<<TypeName("TinyString")<<" text;\n" ;
    OS << "        }\n" ;
  }
  OS << "    } // end class Message\n\n" ;
}

static void emitProtocols(const std::vector<Record*> &Protocols,
    const std::vector<Record*> &Messages, raw_ostream &OS)
{
  auto TagValueType = Messages.size() < 256 ? "Uint8" : "Uint16";
  auto TagValueSize = Messages.size() < 256 ? 1 : 2;
  OS << "\n" ;
  OS << "    public static class Protocol\n" ;
  OS << "    {\n" ;
  OS << "        ZFrame routing; // routing_id from ROUTER, if any\n" ;
  OS << "        ByteBuffer buffer; // Read/write pointer for serialization\n" ;
  OS << "\n" ;
  OS << "        // Put a 1-byte number to the frame\n" ;
  OS << "        private void putNumber1(int value)\n" ;
  OS << "        {\n" ;
  OS << "            buffer.put((byte) value);\n" ;
  OS << "        }\n" ;
  OS << "        // Get a 1-byte number from the frame then make it unsigned\n" ;
  OS << "        private int getNumber1()\n" ;
  OS << "        {\n" ;
  OS << "            int value = buffer.get();\n" ;
  OS << "            if (value < 0) value = (0XFF) & value;\n" ;
  OS << "            return value;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Put a 2-byte number to the frame\n" ;
  OS << "        private void putNumber2(int value)\n" ;
  OS << "        {\n" ;
  OS << "            buffer.putShort((short) value);\n" ;
  OS << "        }\n" ;
  OS << "        // Get a 2-byte number from the frame then make it unsigned\n" ;
  OS << "        private int getNumber2()\n" ;
  OS << "        {\n" ;
  OS << "            int value = buffer.getShort();\n" ;
  OS << "            if (value < 0) value = (0XFFFF) & value;\n" ;
  OS << "            return value;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Put a 4-byte number to the frame\n" ;
  OS << "        private void putNumber4(int value)\n" ;
  OS << "        {\n" ;
  OS << "            buffer.putInt((int) value);\n" ;
  OS << "        }\n" ;
  OS << "        // Get a 4-byte number from the frame then make it unsigned\n" ;
  OS << "        private int getNumber4()\n" ;
  OS << "        {\n" ;
  OS << "            int value = buffer.getShort();\n" ;
  OS << "            if (value < 0) value = (0XFFFFFFFF) & value;\n" ;
  OS << "            return value;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Put a 8-byte number to the frame\n" ;
  OS << "        private void putNumber8(long value)\n" ;
  OS << "        {\n" ;
  OS << "            buffer.putLong(value);\n" ;
  OS << "        }\n" ;
  OS << "        // Get a 8-byte number from the frame\n" ;
  OS << "        private long getNumber8()\n" ;
  OS << "        {\n" ;
  OS << "            return buffer.getLong();\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Put a tiny-string to the frame\n" ;
  OS << "        private void putTinyString(String s)\n" ;
  OS << "        {\n" ;
  OS << "            byte[] a = s.getBytes(CHARSET);\n" ;
  OS << "            buffer.put((byte) a.length);\n" ;
  OS << "            buffer.put(ByteBuffer.wrap(a, 0, Math.min(a.length, 0xFF))/*.slice()*/);\n" ;
  OS << "        }\n" ;
  OS << "        // Get a tiny-string number from the frame\n" ;
  OS << "        private String getTinyString()\n" ;
  OS << "        {\n" ;
  OS << "            int size = getNumber1();\n" ;
  OS << "            byte[] a = new byte[size];\n" ;
  OS << "            buffer.get(a);\n" ;
  OS << "            return new String(a, CHARSET);\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Put a short-string to the frame\n" ;
  OS << "        private void putShortString(String s)\n" ;
  OS << "        {\n" ;
  OS << "            byte[] a = s.getBytes(CHARSET);\n" ;
  OS << "            buffer.putShort((short) a.length);\n" ;
  OS << "            buffer.put(ByteBuffer.wrap(a, 0, Math.min(a.length, 0xFFFF)));\n" ;
  OS << "        }\n" ;
  OS << "        // Get a short-string number from the frame\n" ;
  OS << "        private String getShortString()\n" ;
  OS << "        {\n" ;
  OS << "            int size = getNumber2();\n" ;
  OS << "            byte[] a = new byte[size];\n" ;
  OS << "            buffer.get(a);\n" ;
  OS << "            return new String(a, CHARSET);\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Put a long-string to the frame\n" ;
  OS << "        private void putLongString(String s)\n" ;
  OS << "        {\n" ;
  OS << "            byte[] a = s.getBytes(CHARSET);\n" ;
  OS << "            buffer.putInt((int) a.length);\n" ;
  OS << "            buffer.put(ByteBuffer.wrap(a, 0, Math.min(a.length, 0xFFFFFFFF)));\n" ;
  OS << "        }\n" ;
  OS << "        // Get a long-string number from the frame\n" ;
  OS << "        private String getLongString()\n" ;
  OS << "        {\n" ;
  OS << "            int size = getNumber4();\n" ;
  OS << "            byte[] a = new byte[size];\n" ;
  OS << "            buffer.get(a);\n" ;
  OS << "            return new String(a, CHARSET);\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        public Message recv(Socket input)\n" ;
  OS << "        {\n" ;
  OS << "            assert( input != null );\n" ;
  OS << "            Message msg = null;\n" ;
  OS << "            ZFrame frame = null;\n" ;
  OS << "            int tag = -1;\n" ;
  OS << "            \n" ;
  OS << "            try {\n" ;
  OS << "                while (true) {\n" ;
  OS << "                    if (input.getType() == ZMQ.ROUTER) {\n" ;
  OS << "                        routing = ZFrame.recvFrame(input);\n" ;
  OS << "                        if (routing == null) return null; // Interrupted\n" ;
  OS << "                        if (!routing.hasData()) return null; // Empty Frame (eg recv-timeout)\n" ;
  OS << "                        if (!input.hasReceiveMore ()) throw new IllegalArgumentException ();\n" ;
  OS << "                    }\n" ;
  OS << "\n" ;
  OS << "                    frame = ZFrame.recvFrame(input);\n" ;
  OS << "                    if (frame == null) return null; // Interrupted\n" ;
  OS << "\n" ;
  OS << "                    // Get and check protocol signature\n" ;
  OS << "                    buffer = ByteBuffer.wrap( frame.getData() );\n" ;
  OS << "                    //int signature = getNumber2();\n" ;
  OS << "                    //if (signature == (0xAAA0 | 0)) break; // Valid signature\n" ;
  OS << "                    if (buffer != null) break;\n" ;
  OS << "\n" ;
  OS << "                    // Protocol assertion, drop message\n" ;
  OS << "                    while (input.hasReceiveMore()) {\n" ;
  OS << "                        frame.destroy();\n" ;
  OS << "                        frame = ZFrame.recvFrame(input);\n" ;
  OS << "                    }\n" ;
  OS << "                    frame.destroy();\n" ;
  OS << "                }\n" ;
  OS << "\n" ;
  OS << "                switch ((tag = getNumber"<<TagValueSize<<"())) {\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto Fields = M->getValueAsListOfDefs("FIELDS");
    OS << "                case Message.tag_"<<M->getName()<<": {\n" ;
    OS << "                    "<<Msg(M)<<" m = new "<<Msg(M)<<"();\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      emitFieldDecodingCode(OS, Fields[FI]);
    }
    OS << "                    msg = m;\n" ;
    OS << "                } break;\n" ;
    OS << "\n" ;
  }
  OS << "                default:\n" ;
  OS << "                    throw new IllegalArgumentException();\n" ;
  OS << "                }\n" ;
  OS << "                return msg;\n" ;
  OS << "            } catch (Exception e) {\n" ;
  OS << "                System.out.printf(\"E: malformed message '%d'\\n\", tag);\n" ;
  OS << "                return null;\n" ;
  OS << "            } finally {\n" ;
  OS << "                if (frame != null) frame.destroy();\n" ;
  OS << "            }\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        public boolean send(Socket output, Message am)\n" ;
  OS << "        {\n" ;
  OS << "            assert( output != null );\n" ;
  OS << "\n" ;
  OS << "            ZMsg msg = new ZMsg();\n" ;
  OS << "            // If we're sending to a ROUTER, send the 'routing' first\n" ;
  OS << "            if (output.getType() == ZMQ.ROUTER) {\n" ;
  OS << "                msg.add(routing);\n" ;
  OS << "            }\n" ;
  OS << "            \n" ;
  OS << "            int frameSize = "<<TagValueSize<<";\n" ;
  OS << "            int tag = -1;\n" ;
  OS << "            \n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto Fields = M->getValueAsListOfDefs("FIELDS");
    if (MI == 0)
      OS << "            if (am instanceof "<<Msg(M)<<") {\n" ;
    else 
      OS << "            else if (am instanceof "<<Msg(M)<<") {\n" ;
    OS << "                "<<Msg(M)<<" m = ("<<Msg(M)<<") am;\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      OS << "                frameSize += " ;
      emitFieldSizeExpr(OS, Fields[FI]);
      OS <<";\n" ;
    }
    OS << "                tag = Message.tag_"<<M->getName()<<";\n" ;
    OS << "            }\n" ;
  }
  OS << "\n" ;
  OS << "            ZFrame frame = new ZFrame(new byte[frameSize]);\n" ;
  OS << "            buffer = ByteBuffer.wrap( frame.getData() );\n" ;
  OS << "\n" ;
  OS << "            putNumber"<<TagValueSize<<"(tag);\n" ;
  OS << "\n" ;
  OS << "            switch (tag) {\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto Fields = M->getValueAsListOfDefs("FIELDS");
    OS << "            case Message.tag_"<<M->getName()<<":\n" ;
    OS << "              {\n" ;
    OS << "                "<<Msg(M)<<" m = ("<<Msg(M)<<") am;\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      emitFieldEncodingCode(OS, Fields[FI]);
    }
    OS << "              } break;\n" ;
  }
  OS << "            }\n" ;
  OS << "            msg.add(frame);\n" ;
  OS << "            msg.send(output);\n" ;
  OS << "            return true;\n" ;
  OS << "        }\n" ;
  OS << "    } // end class Protocol\n" ;
  OS << "\n" ;
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
    OS << "    // " << M->getName() << ", " << DirectSuper->getName() << "\n";
    for (auto super : M->getSuperClasses())
      OS << "    //     " << super->getName() << "\n";
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
    OS << "import java.nio.charset.Charset;\n" ;
    OS << "import org.zeromq.ZFrame;\n" ;
    OS << "import org.zeromq.ZMsg;\n" ;
    OS << "import org.zeromq.ZMQ;\n" ;
    OS << "import org.zeromq.ZMQ.Socket;\n" ;
    
    OS << "\n" ;
    OS << "public class messaging\n" ;
    OS << "{\n" ;
    OS << "    public static final Charset CHARSET = Charset.forName(\"UTF-8\");\n" ;
    OS << "\n" ;

    emitMessageDefines(Messages, OS);
    emitProtocols(Protocols, Messages, OS);
    emitStateMachines(Machines, States, Events, OS);
    
    OS << "}\n" ;
  }
} // end namespace lyre
