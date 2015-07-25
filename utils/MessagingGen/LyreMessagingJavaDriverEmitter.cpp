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

static void emitFieldEncodingCode(raw_ostream &OS, const std::string &S, const std::string &N)
{
  if (S == "Int8" || S == "Uint8") {
    OS << "            putNumber1(m."<<N<<");\n";
  }
  if (S == "Int16" || S == "Uint16") {
    OS << "            putNumber2(m."<<N<<");\n";
  }
  if (S == "Int32" || S == "Uint32") {
    OS << "            putNumber4(m."<<N<<");\n";
  }
  if (S == "Int64" || S == "Uint64") {
    OS << "            putNumber8(m."<<N<<");\n";
  }
  if (S == "TinyString") {
    OS << "            putTinyString(m."<<N<<");\n";
  }
  if (S == "ShortString") {
    OS << "            putShortString(m."<<N<<");\n";
  }
  if (S == "LongString") {
    OS << "            putLongString(m."<<N<<");\n";
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
    OS << "            m."<<N<<" = getNumber1();\n";
  }
  if (S == "Int16" || S == "Uint16") {
    OS << "            m."<<N<<" = getNumber2();\n";
  }
  if (S == "Int32" || S == "Uint32") {
    OS << "            m."<<N<<" = getNumber4();\n";
  }
  if (S == "Int64" || S == "Uint64") {
    OS << "            m."<<N<<" = getNumber8();\n";
  }
  if (S == "TinyString") {
    OS << "            m."<<N<<" = getTinyString();\n";
  }
  if (S == "ShortString") {
    OS << "            m."<<N<<" = getShortString();\n";
  }
  if (S == "LongString") {
    OS << "            m."<<N<<" = getLongString();\n";
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
  if (S == "TinyString") {
    OS << "1 + (m."<<N<<" == null ? 0 : m."<<N<<".getBytes(CHARSET).length)" ;
  }
  if (S == "ShortString") {
    OS << "2 + (m."<<N<<" == null ? 0 : m."<<N<<".getBytes(CHARSET).length)" ;
  }
  if (S == "LongString") {
    OS << "4 + (m."<<N<<" == null ? 0 : m."<<N<<".getBytes(CHARSET).length)" ;
  }
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
  Record *UserDefinedErrorMessage = nullptr;
  auto TagValueType = Messages.size() < 256 ? "Uint8" : "Uint16";
  auto TagValueSize = Messages.size() < 256 ? 1 : 2;
  auto ErrorID = 0;

  // Define message structs.
  OS << "    public static abstract class Message\n" ;
  OS << "    {\n" ;
  for (auto M : Messages) {
    auto ID = M->getValueAsInt("ID");
    OS << "        public static final int tag_"
       << M->getName()  << " = " << ID << ";\n" ;
    if (M->getName() == "error") UserDefinedErrorMessage = M;
    if (ID == ErrorID) ErrorID += 1;
  }
  if (!UserDefinedErrorMessage) {
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
  if (!UserDefinedErrorMessage) {
    OS << "        public static final class error extends Message {\n" ;
    OS << "            public "<<TypeName("Uint16")<<" code;\n" ;
    OS << "            public "<<TypeName("TinyString")<<" text;\n" ;
    OS << "        }\n" ;
  }
  OS << "        public static final error makeError(int code, String text) {\n" ;
  OS << "            error e = new error();\n" ;
  OS << "            e.code = (short) code;\n" ;
  OS << "            e.text = text;\n" ;
  OS << "            return e;\n" ;
  OS << "        }\n" ;
  OS << "    } // end class Message\n\n" ;
}

static void emitProtocols(const std::vector<Record*> &Protocols,
    const std::vector<Record*> &Messages, raw_ostream &OS)
{
  Record *UserDefinedErrorMessage = nullptr;
  auto TagValueType = Messages.size() < 256 ? "Uint8" : "Uint16";
  auto TagValueSize = Messages.size() < 256 ? 1 : 2;
  auto Signature = "0xABC0 | 0";
  auto ErrorID = 0;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    if (M->getName() == "error") UserDefinedErrorMessage = M;
    if (M->getValueAsInt("ID") == ErrorID) ErrorID += 1;
  }
  OS << "\n" ;
  OS << "    public static class Protocol implements AutoCloseable\n" ;
  OS << "    {\n" ;
  OS << "        private ZFrame routing = null; // routing_id from ROUTER, if any\n" ;
  OS << "        private ByteBuffer buffer = null; // Read/write pointer for serialization\n" ;
  OS << "\n" ;
  OS << "        @Override\n" ;
  OS << "        public void close() {\n" ;
  OS << "            if (routing != null) routing.destroy(); \n" ;
  OS << "            //if (buffer != null) buffer.destroy(); \n" ;
  OS << "            routing = null;\n" ;
  OS << "            buffer = null;\n" ;
  OS << "        }\n" ;
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
  OS << "            if (s == null) {\n" ;
  OS << "                buffer.put((byte) 0);\n" ;
  OS << "            } else {\n" ;
  OS << "                byte[] a = s.getBytes(CHARSET);\n" ;
  OS << "                buffer.put((byte) a.length);\n" ;
  OS << "                buffer.put(ByteBuffer.wrap(a, 0, Math.min(a.length, 0xFF))/*.slice()*/);\n" ;
  OS << "            }\n" ;
  OS << "        }\n" ;
  OS << "        // Get a tiny-string number from the frame\n" ;
  OS << "        private String getTinyString()\n" ;
  OS << "        {\n" ;
  OS << "            int size = getNumber1();\n" ;
  OS << "            if (size <= 0) return null;\n" ;
  OS << "            byte[] a = new byte[size];\n" ;
  OS << "            buffer.get(a);\n" ;
  OS << "            return new String(a, CHARSET);\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Put a short-string to the frame\n" ;
  OS << "        private void putShortString(String s)\n" ;
  OS << "        {\n" ;
  OS << "            if (s == null) {\n" ;
  OS << "                buffer.putShort((short) 0);\n" ;
  OS << "            } else {\n" ;
  OS << "                byte[] a = s.getBytes(CHARSET);\n" ;
  OS << "                buffer.putShort((short) a.length);\n" ;
  OS << "                buffer.put(ByteBuffer.wrap(a, 0, Math.min(a.length, 0xFFFF)));\n" ;
  OS << "            }\n" ;
  OS << "        }\n" ;
  OS << "        // Get a short-string number from the frame\n" ;
  OS << "        private String getShortString()\n" ;
  OS << "        {\n" ;
  OS << "            int size = getNumber2();\n" ;
  OS << "            if (size <= 0) return null;\n" ;
  OS << "            byte[] a = new byte[size];\n" ;
  OS << "            buffer.get(a);\n" ;
  OS << "            return new String(a, CHARSET);\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Put a long-string to the frame\n" ;
  OS << "        private void putLongString(String s)\n" ;
  OS << "        {\n" ;
  OS << "            if (s == null) {\n" ;
  OS << "                buffer.putInt((int) 0);\n" ;
  OS << "            } else {\n" ;
  OS << "                byte[] a = s.getBytes(CHARSET);\n" ;
  OS << "                buffer.putInt((int) a.length);\n" ;
  OS << "                buffer.put(ByteBuffer.wrap(a, 0, Math.min(a.length, 0xEFFFFFFF)));\n" ;
  OS << "            }\n" ;
  OS << "        }\n" ;
  OS << "        // Get a long-string number from the frame\n" ;
  OS << "        private String getLongString()\n" ;
  OS << "        {\n" ;
  OS << "            int size = getNumber4();\n" ;
  OS << "            if (size <= 0) return null;\n" ;
  OS << "            byte[] a = new byte[size];\n" ;
  OS << "            buffer.get(a);\n" ;
  OS << "            return new String(a, CHARSET);\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Receive message to the buffer without decoding it.\n" ;
  OS << "        private final int recvToBuffer(Socket input)\n" ;
  OS << "        {\n" ;
  OS << "            ZFrame frame = null;\n" ;
  OS << "            \n" ;
  OS << "            assert( input != null );\n" ;
  OS << "            \n" ;
  OS << "            try {\n" ;
  OS << "                while (true) {\n" ;
  OS << "                    if (input.getType() == ZMQ.ROUTER) {\n" ;
  OS << "                        if (routing != null) routing.destroy();\n" ;
  OS << "                        routing = ZFrame.recvFrame(input);\n" ;
  OS << "                        if (routing == null) return -1; // Interrupted\n" ;
  OS << "                        if (!routing.hasData()) return -1; // Empty Frame (eg recv-timeout)\n" ;
  OS << "                        if (!input.hasReceiveMore())\n" ;
  OS << "                            throw new IllegalArgumentException ();\n" ;
  OS << "                    }\n" ;
  OS << "\n" ;
  OS << "                    frame = ZFrame.recvFrame(input);\n" ;
  OS << "                    if (frame == null) return -1; // Interrupted\n" ;
  OS << "\n" ;
  OS << "                    // Get and check protocol signature\n" ;
  OS << "                    buffer = ByteBuffer.wrap( frame.getData() );\n" ;
  OS << "                    int signature = getNumber2();\n" ;
  OS << "                    if (signature == ("<<Signature<<"))\n" ;
  OS << "                        break; // Valid signature\n" ;
  OS << "\n" ;
  OS << "                    // Protocol assertion, drop message\n" ;
  OS << "                    while (input.hasReceiveMore()) {\n" ;
  OS << "                        frame.destroy();\n" ;
  OS << "                        frame = ZFrame.recvFrame(input);\n" ;
  OS << "                    }\n" ;
  OS << "                    if (frame != null) frame.destroy();\n" ;
  OS << "                }\n" ;
  OS << "                int tag = getNumber"<<TagValueSize<<"();\n" ;
  OS << "                //System.out.println(\"tag: \"+tag);\n" ;
  OS << "                return tag;\n" ;
  OS << "            } catch (Exception e) {\n" ;
  OS << "                // e.printStackTrace();\n" ;
  OS << "                System.out.println(\"E: \"+e);\n" ;
  OS << "                return -1;\n" ;
  OS << "            } finally {\n" ;
  OS << "                if (frame != null) frame.destroy();\n" ;
  OS << "            }\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        private final Message decodeMessage(int tag)\n" ;
  OS << "        {\n" ;
  OS << "            switch (tag) {\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    OS << "            case Message.tag_"<<M->getName()<<": {\n" ;
    OS << "                "<<Msg(M)<<" m = new "<<Msg(M)<<"();\n" ;
    OS << "                decodeMessage(m);\n" ;
    OS << "                return m;\n" ;
    OS << "            }\n" ;
  }
  OS << "            default:\n" ;
  OS << "                throw new IllegalArgumentException();\n" ;
  OS << "            }\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto Fields = M->getValueAsListOfDefs("FIELDS");
    OS << "        private final void decodeMessage("<<Msg(M)<<" m)\n" ;
    OS << "        {\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      emitFieldDecodingCode(OS, Fields[FI]);
    }
    OS << "        }\n" ;
  }
  OS << "\n" ;
  OS << "        public final Message recv(Socket input)\n" ;
  OS << "        {\n" ;
  OS << "            return decodeMessage(recvToBuffer(input));\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        private final ZMsg newOutputMsg(Socket output) {\n" ;
  OS << "            ZMsg msg = new ZMsg();\n" ;
  OS << "            // If we're sending to a ROUTER, send the 'routing' first\n" ;
  OS << "            if (output.getType() == ZMQ.ROUTER && routing != null) {\n" ;
  OS << "                msg.add(routing);\n" ;
  OS << "            }\n" ;
  OS << "            return msg;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        private final ZFrame newMsgFrame(int tag, int msgSize) {\n" ;
  OS << "            int frameSize = 2 + "<<TagValueSize<<" + msgSize;\n" ;
  OS << "            ZFrame frame = new ZFrame(new byte[frameSize]);\n" ;
  OS << "            buffer = ByteBuffer.wrap( frame.getData() );\n" ;
  OS << "            putNumber2("<<Signature<<"); // signature\n" ;
  OS << "            putNumber"<<TagValueSize<<"(tag);\n" ;
  OS << "            return frame;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    auto Fields = M->getValueAsListOfDefs("FIELDS");
    OS << "        public final boolean send(Socket output, "<<Msg(M)<<" m)\n" ;
    OS << "        {\n" ;
    OS << "            ZMsg msg = newOutputMsg(output);\n" ;
    OS << "            \n" ;
    OS << "            int tag = Message.tag_"<<M->getName()<<";\n" ;
    OS << "            int msgSize = 0;\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      OS << "            msgSize += " ;
      emitFieldSizeExpr(OS, Fields[FI]);
      OS <<";\n" ;
    }
    OS << "\n" ;
    OS << "            ZFrame frame = newMsgFrame(tag, msgSize);\n" ;
    for (std::size_t FI = 0, FE = Fields.size(); FI < FE; ++FI) {
      emitFieldEncodingCode(OS, Fields[FI]);
    }
    OS << "\n" ;
    OS << "            msg.add(frame);\n" ;
    OS << "            return msg.send(output);\n" ;
    OS << "        }\n" ;
  }
  if (!UserDefinedErrorMessage) {
    OS << "        public final boolean send(Socket output, "<<Msg("error")<<" m)\n" ;
    OS << "        {\n" ;
    OS << "            ZMsg msg = newOutputMsg(output);\n" ;
    OS << "            \n" ;
    OS << "            int tag = Message.tag_error;\n" ;
    OS << "            int msgSize = 0;\n" ;
    OS << "            msgSize += "; emitFieldSizeExpr(OS, "Uint16", "code"); OS << ";\n" ;
    OS << "            msgSize += "; emitFieldSizeExpr(OS, "TinyString", "text"); OS << ";\n" ;
    OS << "\n" ;
    OS << "            ZFrame frame = newMsgFrame(tag, msgSize);\n" ;
    emitFieldEncodingCode(OS, "Uint16", "code");
    emitFieldEncodingCode(OS, "TinyString", "text");
    OS << "\n" ;
    OS << "            msg.add(frame);\n" ;
    OS << "            return msg.send(output);\n" ;
    OS << "        }\n" ;
  }
  OS << "    } // end class Protocol\n" ;
  OS << "\n" ;
  OS << "    public static abstract class RequestProcessor implements AutoCloseable\n" ;
  OS << "    {\n" ;
  OS << "        private Protocol protocol = null;\n" ;
  OS << "        private Socket socket = null;\n" ;
  OS << "\n" ;
  OS << "        public RequestProcessor(Socket s)\n" ;
  OS << "        {\n" ;
  OS << "            protocol = new Protocol();\n" ;
  OS << "            socket = s;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        public final Socket getSocket() { return socket; }\n" ;
  OS << "\n" ;
  OS << "        @Override\n" ;
  OS << "        public void close()\n" ;
  OS << "        {\n" ;
  OS << "            if (protocol != null) protocol.close();\n" ;
  OS << "            protocol = null;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        // Wait and process a request.\n" ;
  OS << "        public final boolean waitProcessRequest()\n" ;
  OS << "        {\n" ;
  OS << "            assert(protocol != null);\n" ;
  OS << "            assert(socket != null);\n" ;
  OS << "            final int tag = protocol.recvToBuffer(socket);\n" ;
  OS << "            switch (tag) {\n" ;
  std::vector<Record*> BadRequests;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    OS << "            case Message.tag_"<<M->getName()<<": {\n" ;
    auto C = 0;
    for (auto P : Protocols) {
      auto Req = P->getValueAsDef("REQ");
      auto Rep = P->getValueAsDef("REP");
      if (Req != M && Rep != M) continue;
      if (Req == M) {
        OS << "                "<<Msg(Req)<<" Q = new "<<Msg(Req)<<"();\n" ;
        OS << "                "<<Msg(Rep)<<" P = new "<<Msg(Rep)<<"();\n" ;
        OS << "                protocol.decodeMessage(Q);\n" ;
        OS << "                try { onRequest(Q, P); } catch(Exception e) {\n" ;
        OS << "                    System.out.println(\"onRequest: \"+e);\n" ;
        OS << "                }\n" ;
        OS << "                return protocol.send(socket, P);\n" ;
        C += 1;
      }
    }
    if (C == 0) {
      OS << "                "<<Msg("error")<<" P = Message.makeError(-1, \"bad\");\n" ;
      OS << "                try { onBadRequest(tag); } catch(Exception e) {\n" ;
      OS << "                    System.out.println(\"onRequest: \"+e);\n" ;
      OS << "                }\n" ;
      OS << "                return protocol.send(socket, P);\n" ;
      BadRequests.push_back(M);
    }
    OS << "            }\n" ;
  }
  OS << "            }\n" ;
  OS << "            return false;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  for (auto P : Protocols) {
    auto Req = P->getValueAsDef("REQ");
    auto Rep = P->getValueAsDef("REP");
    OS << "        protected abstract void onRequest("<<Msg(Req)<<" Q, "<<Msg(Rep)<<" P);\n" ;
  }
  OS << "\n" ;
  OS << "        protected void onBadRequest(int tag) {}\n" ;
  OS << "    }\n" ;
  OS << "\n" ;
  OS << "    public static abstract class ReplyProcessor implements AutoCloseable\n" ;
  OS << "    {\n" ;
  OS << "        private Protocol protocol = null;\n" ;
  OS << "        private Socket socket = null;\n" ;
  OS << "\n" ;
  OS << "        public ReplyProcessor(Socket s)\n" ;
  OS << "        {\n" ;
  OS << "            protocol = new Protocol();\n" ;
  OS << "            socket = s;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  OS << "        public final Socket getSocket() { return socket; }\n" ;
  OS << "\n" ;
  OS << "        @Override\n" ;
  OS << "        public void close()\n" ;
  OS << "        {\n" ;
  OS << "            if (protocol != null) protocol.close();\n" ;
  OS << "            protocol = null;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  for (auto P : Protocols) {
    auto M = P->getValueAsDef("REQ");
    OS << "        public final boolean send("<<Msg(M)<<" m) { return protocol.send(socket, m); }\n" ;
  }
  OS << "\n" ;
  OS << "        public final boolean waitProcessReply()\n" ;
  OS << "        {\n" ;
  OS << "            assert(protocol != null);\n" ;
  OS << "            assert(socket != null);\n" ;
  OS << "            final int tag = protocol.recvToBuffer(socket);\n" ;
  OS << "            switch (tag) {\n" ;
  for (std::size_t MI = 0, MS = Messages.size(); MI < MS; ++MI) {
    auto M = Messages[MI];
    OS << "            case Message.tag_"<<M->getName()<<": {\n" ;
    auto C = 0;
    for (auto P : Protocols) {
      auto Req = P->getValueAsDef("REQ");
      auto Rep = P->getValueAsDef("REP");
      if (Req != M && Rep != M) continue;
      if (Rep == M) {
        OS << "                "<<Msg(Rep)<<" P = new "<<Msg(Rep)<<"();\n" ;
        OS << "                protocol.decodeMessage(P);\n" ;
        OS << "                onReply(P);\n" ;
        OS << "                return true;\n" ;
        C += 1;
      }
    }
    if (C == 0) {
      OS << "                onBadReply(tag);\n" ;
      OS << "                return true;\n" ;
    }
    OS << "            }\n" ;
  }
  OS << "            }\n" ;
  OS << "            return false;\n" ;
  OS << "        }\n" ;
  OS << "\n" ;
  for (auto P : Protocols) {
    auto Rep = P->getValueAsDef("REP");
    OS << "        protected abstract void onReply("<<Msg(Rep)<<" P);\n" ;
  }
  OS << "\n" ;
  OS << "        protected void onBadReply(int tag) {}\n" ;
  OS << "    }\n" ;
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
