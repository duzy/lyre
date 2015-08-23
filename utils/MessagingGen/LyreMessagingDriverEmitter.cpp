#include "MessagingGenBackends.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <cctype>
#include <map>
#include <set>
#include <string>

static const char * const BaseCode = R"***(//"
#include <boost/algorithm/string/split.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/noncopyable.hpp>
#include <cstdint>
#include <thread>
#include <chrono>
#include <memory>
#include <zmq.h>
#include <mutex>
#include <iostream>

namespace { static std::mutex logmux; }

#define DBG(x)                                            \
  do {                                                    \
    std::lock_guard <std::mutex> lock (logmux);           \
    std::clog << __FILE__ << ":" << __LINE__ << ": "      \
              << x << std::endl;                          \
  } while (false);

namespace meta = boost::mpl;

namespace
{

static const struct zmq_wrapper
{
  zmq_wrapper() : the_context( zmq_ctx_new () ) {}
  ~zmq_wrapper() { zmq_ctx_term (the_context); }
  inline void *context() const { return the_context; }
private:
  void * the_context;
} zmq;

/**
 *  `frame` wraps a zmq_msg_t for conveniences.
 */
struct frame : boost::noncopyable
{
  frame() { zmq_msg_init (&_msg); }

  explicit frame( std::size_t size ) { zmq_msg_init_size (&_msg, size); }

  ~frame() { zmq_msg_close (&_msg); }

  std::size_t size () { return zmq_msg_size (&_msg); }
  Uint8 *data () { return (Uint8*) zmq_msg_data (&_msg); }

  int copy (frame & dest) { return zmq_msg_copy (&dest._msg, &_msg); }
  int move (frame & dest) { return zmq_msg_move (&dest._msg, &_msg); }

  int more () { return zmq_msg_more (&_msg); }

  int get (int property) { return zmq_msg_get (&_msg, property); }
  int set (int property, int optval) { return zmq_msg_set (&_msg, property, optval); }

  /**
   *  The caller shall not modify or free the returned value,
   *  which shall be owned by the message. The encoding of the property and value
   *  shall be UTF8.
   */
  //const char *gets (const char *property) { return zmq_msg_gets (&_msg, property); }

  int send (void *s, int flags) { return zmq_msg_send (&_msg, s, flags); }
  int recv (void *s, int flags) { return zmq_msg_recv (&_msg, s, flags); }

private:
  zmq_msg_t _msg;
};

/**
 *  `message` wraps a list of `frame` instances.
 */
struct message : boost::ptr_list<frame>
{
  message() {}

  ~message() = default;

  /**
   *  Returns true if the message is not empty and any frame of it has any bytes.
   */
  bool has_data () // const
  {
    if (!empty()) for (auto & f : *this) if (f.size ()) return true;
    return false;
  }

  /**
   *  `recv` receive frames from `source`, returns zero on success or non-zero when partially
   *  or none received.
   */
  int recv (void *source, int flags)
  {
    int nbytes = 0, more, rc;
    while (true) {
      std::unique_ptr<frame> f( new (frame) );
      if ((rc = f->recv (source, flags)) < 0) {
        return rc;
      }

      nbytes += rc;
      more = f->more();
      push_back (f.release());

      if (!more) break;
    }
    return nbytes;
  }

  /**
   *  send message to `dest`, returns zero after successfully sent or non-zero on partially
   *  sent or none sent.
   */
  int send (void *dest, int flags)
  {
    int rc = -1;
    for (auto & f : *this) if ((rc = f.send (dest, flags)) != 0) break;
    return rc;
  }
};

/**
 *  `socket` wraps zmq_socket and related API.
 */
struct socket : boost::noncopyable
{
  socket (int type) : _handle( zmq_socket ( zmq.context(), type ) ) { assert (_handle != NULL); }

  ~socket () { zmq_close (_handle); }

  int setsockopt (int option, const void*optval, size_t optvallen) {
    assert (_handle != NULL);
    return zmq_setsockopt (_handle, option, optval, optvallen);
  }

  int getsockopt (int option, void *optval, size_t *optvallen) {
    assert (_handle != NULL);
    return zmq_getsockopt (_handle, option, optval, optvallen);
  }

  int bind (const char *addr) {
    assert (_handle != NULL);
    return zmq_bind (_handle, addr);
  }

  template < class Iterator >
  void bind (Iterator begin, Iterator end)
  {
    assert (_handle != NULL);
    for (auto it = begin; it != end; ++it) {
      int rc = zmq_bind ( _handle, it->c_str() );
      if (rc != 0) DBG ("bind: " << zmq_strerror (zmq_errno ()) << ": " << *it);
      assert (rc == 0);
    }
  }

  int unbind (const char *addr) {
    assert (_handle != NULL);
    return zmq_unbind (_handle, addr);
  }

  template < class Iterator >
  void unbind (Iterator begin, Iterator end)
  {
    assert (_handle != NULL);
    for (auto it = begin; it != end; ++it) {
      int rc = zmq_unbind ( _handle, it->c_str() );
      if (rc != 0) DBG ("unbind: " << zmq_strerror (zmq_errno ()) << ": " << *it);
      assert (rc == 0);
    }
  }

  int connect (const char *addr) {
    assert (_handle != NULL);
    return zmq_connect (_handle, addr);
  }

  template < class Iterator >
  void connect (Iterator begin, Iterator end)
  {
    assert (_handle != NULL);
    for (auto it = begin; it != end; ++it) {
      int rc = zmq_connect ( _handle, it->c_str() );
      if (rc != 0) DBG ("connect: " << zmq_strerror (zmq_errno ()) << ": " << *it);
      assert (rc == 0);
    }
  }

  int disconnect (const char *addr) {
    assert (_handle != NULL);
    return zmq_disconnect (_handle, addr);
  }

  template < class Iterator >
  void disconnect (Iterator begin, Iterator end)
  {
    assert (_handle != NULL);
    for (auto it = begin; it != end; ++it) {
      int rc = zmq_disconnect ( _handle, it->c_str() );
      if (rc != 0) DBG ("disconnect: " << zmq_strerror (zmq_errno ()) << ": " << *it);
      assert (rc == 0);
    }
  }

  int send (const void *buf, size_t len, int flags) {
    assert (_handle != NULL);
    return zmq_send (_handle, buf, len, flags);
  }

  int send_const (const void *buf, size_t len, int flags) {
    assert (_handle != NULL);
    return zmq_send_const (_handle, buf, len, flags);
  }

  int recv (void *buf, size_t len, int flags) {
    assert (_handle != NULL);
    return zmq_recv (_handle, buf, len, flags);
  }

  int send (message & m, int flags) { return m.send (_handle, flags); }
  int recv (message & m, int flags) { return m.recv (_handle, flags); }

  int monitor (const char *addr, int events) {
    assert (_handle != NULL);
    return zmq_socket_monitor (_handle, addr, events);
  }

private:
  void *_handle;
};

/**
 *  Basic protocol combining with a codec implementation.
 */
template < class Derived, template <class P, class A> class Codec >
struct base_protocol : boost::noncopyable
{
  struct accessor
  {
    template <class... T> static auto field_size(Derived *p, T&... t) { return p->field_size(t...); }
    template <class... T> static auto get(Derived *p, T&... t) { return p->get(t...); }
    template <class... T> static auto put(Derived *p, T&... t) { return p->put(t...); }
  };

  using codec = Codec<Derived, accessor>;

  ~base_protocol () = default;

  template <class Iterator> void bind_many(Iterator begin, Iterator end) { _socket.bind(begin, end); }
  template <class Iterator> void connect_many(Iterator begin, Iterator end) { _socket.connect(begin, end); }

  template <class Message>
  auto size (const Message & m) const //-> decltype(codec::size(this, m)) 
  { return codec::size(static_cast<Derived*>(const_cast<base_protocol*>(this)), m); }

  template <class Message>
  int send (const Message & m)
  {
    auto size = codec::size (static_cast<Derived*>(this), m);
    this->reset ( 2 + sizeof (typename codec::tag_value_t) + size );
    this->put ( Uint16( 0xABC0 | 0 ) );
    this->put ( typename codec::tag_value_t(codec::t(m)) );
    codec::encode ( static_cast<Derived*>(this), m );
    int rc = send_with_flags (0);
    if (rc < 0) DBG ("error: " << __FUNCTION__ << ": " << error() << ", " << strerror());
    return rc;
  }

  template < class Message >
  int recv (Message & m)
  {
    int rc = recv_with_flags (0);
    if (rc < 0) {
      DBG ("error: " << __FUNCTION__ << ": " << error() << ", " << strerror());
      return rc;
    }
            
    Uint16 signature = 0;
    if (this->get (signature) != 2) {
      DBG ("error: " << __FUNCTION__ << ": wrong message");
      return -1;
    }

    if (signature != (0xABC0 | 0)) {
      DBG ("error: " << __FUNCTION__ << ": wrong signature");
      return -1;
    }
            
    auto tag = typename codec::tag_t( get<typename codec::tag_value_t>() );

    if (codec::t(m) == tag) {
      codec::decode (static_cast<Derived*>(this), m);
    } else {
      DBG ("error: " << __FUNCTION__ << ": tag not match: " << int(tag) << " != " << int(codec::t(m)));
      rc = -1;
    }

    return rc;
  }

  int error() const { return zmq_errno(); }
  const char * strerror(int e) const { return zmq_strerror( e ); }
  const char * strerror() const { return zmq_strerror( error() ); }

protected:
  explicit base_protocol (int type, int sndtimeo = 12*1000, int rcvtimeo = 12*1000)
    : _sockmux  ( )
    , _socket   ( type )
    , _message  ( )
    , _cur      ( NULL )
    , _end      ( NULL )
  {
    int rc;
    if ((rc = _socket.setsockopt (ZMQ_SNDTIMEO, &sndtimeo, sizeof (sndtimeo))) < 0) {
      DBG ("error: " << __FUNCTION__ << ": SNDTIMEO: " << error() << ", " << strerror());
    }
    if ((rc = _socket.setsockopt (ZMQ_RCVTIMEO, &rcvtimeo, sizeof (rcvtimeo))) < 0) {
      DBG ("error: " << __FUNCTION__ << ": RCVTIMEO: " << error() << ", " << strerror());
    }
  }

  int send_with_flags (int flags)
  {
    std::lock_guard< std::mutex > lock (_sockmux);
    return ( _message.has_data () ) ? _socket.send (_message, flags) : 0;
  }

  int recv_with_flags (int flags)
  {
    std::lock_guard< std::mutex > lock (_sockmux);

    /*if (_message.has_data())*/ _message.clear();

    int rc = _socket.recv (_message, flags);
    if (0 < rc && _message.has_data ()) {
      message::reference frame = _message.front ();
      _cur = frame.data ();
      _end = _cur + frame.size ();
    } else {
      if (rc < 0) {
        if (error () == 11 /* e.g. Resource temporarily unavailable. */) {
          /**
           *  It could be a timeout on receiving message.
           */
        } else {
          DBG ("error: " << __FUNCTION__ << ": " << error() << ", " << strerror());
        }
      }
      _cur = _end = NULL;
    }
    return rc;
  }

  // friend typename codec::accessor;

  std::size_t field_size (Uint8 ) const { return sizeof (Uint8 ); }
  std::size_t field_size (Uint16) const { return sizeof (Uint16); }
  std::size_t field_size (Uint32) const { return sizeof (Uint32); }
  std::size_t field_size (Uint64) const { return sizeof (Uint64); }
  std::size_t field_size (const TinyString & s) const { return sizeof(Uint8) + s.size(); }
  std::size_t field_size (const ShortString & s) const { return sizeof(Uint16) + s.size(); }
  std::size_t field_size (const LongString & s) const { return sizeof(Uint32) + s.size(); }

  void put (Uint8 v)
  {
    // DBG (__FUNCTION__<<": Uint8: "<<int(v));
    assert (_cur != NULL);
    *_cur = v;
    _cur++;
  }

  void put (Uint16 v)
  {
    // DBG (__FUNCTION__<<": Uint16: "<<int(v));
    assert (_cur != NULL);
    _cur [0] = Uint8(((v >> 8)  & 0xFF));
    _cur [1] = Uint8(((v)       & 0xFF)) ;
    _cur += 2;
  }

  void put (Uint32 v)
  {
    // DBG (__FUNCTION__<<": Uint32: "<<int(v));
    assert (_cur != NULL);
    _cur [0] = Uint8(((v >> 24) & 0xFF));
    _cur [1] = Uint8(((v >> 16) & 0xFF));
    _cur [2] = Uint8(((v >> 8)  & 0xFF));
    _cur [3] = Uint8(((v)       & 0xFF));
    _cur += 4;
  }

  void put (Uint64 v)
  {
    // DBG (__FUNCTION__<<": Uint64: "<<int(v));
    assert (_cur != NULL);
    _cur [0] = Uint8(((v >> 56) & 0xFF));
    _cur [1] = Uint8(((v >> 48) & 0xFF));
    _cur [2] = Uint8(((v >> 40) & 0xFF));
    _cur [3] = Uint8(((v >> 32) & 0xFF));
    _cur [4] = Uint8(((v >> 24) & 0xFF));
    _cur [5] = Uint8(((v >> 16) & 0xFF));
    _cur [6] = Uint8(((v >> 8)  & 0xFF));
    _cur [7] = Uint8(((v)       & 0xFF));
    _cur += 8;
  }

  int get (Uint8 & v)
  {
    if (_end < _cur + 1) return -1;
    assert (_cur != NULL);
    v = *_cur;
    _cur++;
    return 1;
  }

  int get (Uint16 & v)
  {
    if (_end < _cur + 2) return -1;
    assert (_cur != NULL);
    v = ((Uint16) (_cur [0]) << 8)
      + ((Uint16) (_cur [1])) ;
    _cur += 2;
    return 2;
  }

  int get (Uint32 & v)
  {
    if (_end < _cur + 4) return -1;
    assert (_cur != NULL);
    v = ((Uint32) (_cur [0]) << 24)
      + ((Uint32) (_cur [1]) << 16)
      + ((Uint32) (_cur [2]) << 8)
      +  (Uint32) (_cur [3]) ;
    _cur += 4;
    return 4;
  }

  int get (Uint64 & v)
  {
    if (_end < _cur + 8) return -1;
    assert (_cur != NULL);
    v = ((Uint64) (_cur [0]) << 56)
      + ((Uint64) (_cur [1]) << 48)
      + ((Uint64) (_cur [2]) << 40)
      + ((Uint64) (_cur [3]) << 32)
      + ((Uint64) (_cur [4]) << 24)
      + ((Uint64) (_cur [5]) << 16)
      + ((Uint64) (_cur [6]) << 8)
      +  (Uint64) (_cur [7]) ;
    _cur += 8;
    return 8;
  }

  template <class SizeType>
  void put_string (const std::string & s)
  {
    std::size_t size = s.size();
    put(SizeType(size));
    memcpy (_cur, &s[0], size);
    _cur += size;
  }

  template <class SizeType>
  int get_string (std::string & s)
  {
    std::size_t size = std::size_t( get<SizeType>() );
    if (_end < _cur + size) return -1;
    s.resize( size );
    assert (_cur != NULL);
    memcpy (&s[0], _cur, size);
    _cur += size;
    return size;
  }

  void put (const TinyString & s) { put_string<Uint8>(s); }
  void put (const ShortString & s) { put_string<Uint16>(s); }
  void put (const LongString & s) { put_string<Uint32>(s); }
  int get (TinyString & s) { return get_string<Uint8>(s); }
  int get (ShortString & s) { return get_string<Uint16>(s); }
  int get (LongString & s) { return get_string<Uint32>(s); }

  void put (Uint8 *data, std::size_t size)
  {
    assert (_cur != NULL);
    memcpy (_cur, data, size);
    _cur += size;
  }

  int get (Uint8 *data, std::size_t size)
  {
    if (_end < _cur + size) return -1;
    assert (_cur != NULL);
    memcpy (data, _cur, size);
    _cur += size;
    return size;
  }

  template <class T>
  T get()
  {
    T t;
    if (get(t) < 0) {
      // TODO: throw a protocol error
    }
    return t;
  }

  /**
   *  Each time the protocol is about to encode a message, this
   *  `reset` must be called.
   */
  void reset (std::size_t framesize)
  {
    _message.clear ();

    std::unique_ptr<frame> f( new frame (framesize) );
    _cur = f->data ();
    _end = _cur + f->size ();
    _message.push_back (f.release());
  }

private:
  std::mutex _sockmux;
  socket _socket;
  message _message;
  Uint8 *_cur;
  Uint8 *_end;
};

/**
 *  Basic protocol combining with a codec implementation.
 */
template < class Derived, template <class P, class A> class Codec >
struct base_processor : base_protocol < Derived, Codec >
{
  using base_type = base_protocol< Derived, Codec >;
  using typename base_type::codec;
  using base_type::size;
  using base_type::error;
  using base_type::strerror;

  const base_type & protocol() const { return *this; }
  base_type & protocol() { return *this; }

protected:
  explicit base_processor (int type, int sndtimeo = 12*1000, int rcvtimeo = 12*1000)
      : base_type (type, sndtimeo, rcvtimeo)
  {}
};
)***";//"

static const char * const ReceivingCode = R"***(//"
    int rc = this->recv_with_flags (0);
    if (rc < 0) {
      if (this->error () == 11 /* e.g. Resource temporarily unavailable. */) {
        /**
         *  It could be timeout on receiving messages.
         */
      } else {
        DBG ("error: " << __FUNCTION__ << ": " <<
             this->error() << ", " << this->strerror());
      }
      return false;
    }

    Uint16 signature = 0;
    if (this->get (signature) != 2) {
      DBG ("error: " << __FUNCTION__ << ": wrong message");
      return false;
    }

    if (signature != (0xABC0 | 0)) {
      DBG ("error: " << __FUNCTION__ << ": wrong signature");
      return false;
    }
      
    auto id = typename codec::tag_t(this->template get<typename codec::tag_value_t>());
)***";//"

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
  OS << "};\n" ;
  OS << "\n" ;

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
  OS << "\n" ;
  OS << "private:\n" ;
  OS << "  codec() = delete;\n" ;
  OS << "  ~codec() = delete;\n" ;
  OS << "  void operator=(const codec &) = delete;\n" ;
  OS << "}; // end struct codec\n" ;
  OS << "\n" ;

#if 0
  // The "protocol" definition.
  OS << "struct protocol : base_protocol<protocol, codec>\n" ;
  OS << "{\n" ;
  OS << "  explicit protocol(int type) : base_protocol(type) {}\n" ;
  OS << "}; // end struct protocol\n" ;
  OS << "\n" ;
#endif

  // The "request_processor" definition.
  OS << "struct request_processor : base_processor<request_processor, codec>\n" ;
  OS << "{\n" ;
  OS << "  explicit request_processor(int type) : base_processor(type) {}\n" ;
  OS << "\n" ;
  OS << "  bool wait_process_request(MessageResponder *H)\n" ;
  OS << "  { " ;
  OS << ReceivingCode ;
  OS << "    switch (id) {\n" ;
  for (auto P : Protocols) {
    auto Req = P->getValueAsDef("REQ");
    auto Rep = P->getValueAsDef("REP");
    OS << "    case ::tag::"<<Req->getName()<<":\n" ;
    OS << "    {\n" ;
    OS << "      "<<Req->getName()<<" Q;\n" ;
    OS << "      "<<Rep->getName()<<" P;\n" ;
    OS << "      codec::decode(this, Q);\n";
    OS << "      H->on_request(Q, P);\n" ;
    OS << "      int rc = base_processor::send(P);\n" ;
    OS << "      return rc == 0;\n" ;
    OS << "    }\n" ;
  }
  OS << "    default:\n" ;
  OS << "    {\n" ;
  OS << "      ERROR P = make_error(-2, \"bad\");\n" ;
  OS << "      //on_bad_request(id);\n" ;
  OS << "      int rc = base_processor::send(P);\n" ;
  OS << "      return rc == 0;\n" ;
  OS << "    }\n" ;
  if (!HasUserDefinedErrorMessage) {
    // TODO: ...
  }
  OS << "    }\n" ;
  OS << "    return false;\n" ;
  OS << "  }\n" ;
  OS << "\n" ;
  OS << "private:\n" ;
  OS << "  ERROR make_error(Uint16 n, const char *s) {\n" ;
  if (!HasUserDefinedErrorMessage) {
    OS << "    return ERROR{ n, s };\n" ;
  } else {
    OS << "    ERROR E;\n" ;
    OS << "    // TODO: init E;\n" ;
    OS << "    return E;\n" ;
  }
  OS << "  }\n" ;
  OS << "}; // end struct request_processor\n\n" ;
  OS << "\n" ;

  // The "reply_processor" definition.
  OS << "struct reply_processor : base_processor<reply_processor, codec>\n" ;
  OS << "{\n" ;
  OS << "  explicit reply_processor(int type) : base_processor(type) {}\n" ;
  OS << "\n" ;
  OS << "  bool wait_process_reply(MessageRequester *H)\n" ;
  OS << "  { " ;
  OS << ReceivingCode ;
  OS << "    switch (id) {\n" ;
  for (auto P : Protocols) {
    auto Req = P->getValueAsDef("REQ");
    auto Rep = P->getValueAsDef("REP");
    OS << "    case ::tag::"<<Req->getName()<<":\n" ;
    OS << "    {\n" ;
    OS << "      "<<Rep->getName()<<" P;\n" ;
    OS << "      codec::decode(this, P);\n";
    OS << "      H->on_reply(P);\n" ;
    OS << "      return true;\n" ;
    OS << "    }\n" ;
  }
  OS << "    default:\n" ;
  OS << "      //H->on_bad_reply(id);\n" ;
  OS << "      break;\n" ;
  OS << "    }\n" ;
  OS << "    return false;\n" ;
  OS << "  }\n" ;
  OS << "\n" ;
  if (true /*!HasUserDefinedErrorMessage*/) {
    //OS << "  virtual void on_reply(const ERROR &E) {}\n" ;
  }
  OS << "}; // end struct reply_processor\n\n" ;
  OS << "\n" ;
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

    OS << BaseCode ;

    if (SharedHeader.empty())
      emitMessageStructs(Messages, OS);

    OS << "\n" ;

    emitProtocols(Protocols, Messages, OS);
    
    OS << "} // end anonymous namespace\n" ;
    OS << "\n" ;
    if (!Namespace.empty()) OS << "namespace " << Namespace << " {\n" ;
    OS << "struct responder : request_processor { using request_processor::request_processor; };\n" ;
    OS << "struct requester : reply_processor { using reply_processor::reply_processor; };\n" ;
    OS << "\n" ;
    OS << "MessageResponder::MessageResponder(int type) : Base(new responder(type)) {}\n" ;
    OS << "MessageResponder::~MessageResponder() { delete Base; }\n" ;
    for (auto P : Protocols) {
      auto Rep = P->getValueAsDef("REP");
      OS << "int MessageResponder::send(const "<<Rep->getName()<<"&P) { return Base->send(P); }\n" ;
    }
    OS << "bool MessageResponder::wait_request() { return Base->wait_process_request(this); }\n" ;
    OS << "void MessageResponder::bind(const std::initializer_list<std::string> &&a) { Base->bind_many(a.begin(), a.end()); }\n" ;
    OS << "void MessageResponder::connect(const std::initializer_list<std::string> &&a) { Base->connect_many(a.begin(), a.end()); };\n" ;
    OS << "\n" ;
    OS << "MessageRequester::MessageRequester(int type) : Base(new requester(type)) {}\n" ;
    OS << "MessageRequester::~MessageRequester() { delete Base; }\n" ;
    for (auto P : Protocols) {
      auto Req = P->getValueAsDef("REQ");
      OS << "int MessageRequester::send(const "<<Req->getName()<<"&Q) { return Base->send(Q); }\n" ;
    }
    OS << "bool MessageRequester::wait_reply() { return Base->wait_process_reply(this); }\n" ;
    OS << "void MessageRequester::bind(const std::initializer_list<std::string> &&a) { Base->bind_many(a.begin(), a.end()); }\n" ;
    OS << "void MessageRequester::connect(const std::initializer_list<std::string> &&a) { Base->connect_many(a.begin(), a.end()); };\n" ;
    if (!Namespace.empty()) OS << "} // end namespace " << Namespace << ";\n" ;
  }

  void EmitMessagingDriverHH(RecordKeeper &Records, raw_ostream &OS)
  {
    std::vector<Record*> Protocols = Records.getAllDerivedDefinitions("Protocol");
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
    OS << "#include <memory>\n" ;
    OS << "\n" ;
    
    if (!Namespace.empty())
      OS << "namespace " << Namespace << "\n{\n" ;
    
    emitMessageStructs(Messages, OS);

    OS << "struct MessagingWorker\n" ;
    OS << "{\n" ;
    OS << "  virtual ~MessagingWorker() {}\n" ;
    OS << "  virtual void bind(const std::string &a) final { bind({ a }); }\n" ;
    OS << "  virtual void bind(const std::initializer_list<std::string> &&a) = 0;\n" ;
    OS << "  virtual void connect(const std::string &a) final { connect({ a }); }\n" ;
    OS << "  virtual void connect(const std::initializer_list<std::string> &&a) = 0;\n" ;
    OS << "};\n" ;
    OS << "\n" ;

    OS << "struct MessageResponder : MessagingWorker\n" ;
    OS << "{\n" ;
    OS << "  explicit MessageResponder(int type);\n" ;
    OS << "  ~MessageResponder();\n" ;
    OS << "\n" ;
    OS << "  virtual void bind(const std::initializer_list<std::string> &&a) override;\n" ;
    OS << "  virtual void connect(const std::initializer_list<std::string> &&a) override;\n" ;
    OS << "\n" ;
    for (auto P : Protocols) {
      auto Rep = P->getValueAsDef("REP");
      OS << "  int send(const "<<Rep->getName()<<"&P);\n" ;
    }
    OS << "\n" ;
    OS << "  bool wait_request();\n" ;
    OS << "\n" ;
    for (auto P : Protocols) {
      auto Req = P->getValueAsDef("REQ");
      auto Rep = P->getValueAsDef("REP");
      OS << "  virtual void on_request(const "<<Req->getName()<<" &Req, "
         << Rep->getName() << " &Rep) {}\n" ;
    }
    OS << "\n" ;
    OS << "private:\n" ;
    OS << "  struct responder *Base;\n" ;
    OS << "  MessageResponder(const MessageResponder &) = delete;\n" ;
    OS << "  void operator=(const MessageResponder &) = delete;\n" ;
    OS << "};\n" ;
    OS << "\n" ;
    
    OS << "struct MessageRequester : MessagingWorker\n" ;
    OS << "{\n" ;
    OS << "  explicit MessageRequester(int type);\n" ;
    OS << "  ~MessageRequester();\n" ;
    OS << "\n" ;
    OS << "  virtual void bind(const std::initializer_list<std::string> &&a) override;\n" ;
    OS << "  virtual void connect(const std::initializer_list<std::string> &&a) override;\n" ;
    OS << "\n" ;
    for (auto P : Protocols) {
      auto Req = P->getValueAsDef("REQ");
      OS << "  int send(const "<<Req->getName()<<"&Q);\n" ;
    }
    OS << "\n" ;
    OS << "  bool wait_reply();\n" ;
    OS << "\n" ;
    for (auto P : Protocols) {
      auto Req = P->getValueAsDef("REQ");
      auto Rep = P->getValueAsDef("REP");
      OS << "  virtual void on_reply(const "<<Rep->getName()<<" &Rep) {}\n" ;
    }
    if (true /*!HasUserDefinedErrorMessage*/) {
      OS << "  virtual void on_reply(const struct error &E) {}\n" ;
    }
    OS << "\n" ;
    OS << "private:\n" ;
    OS << "  struct requester *Base;\n" ;
    OS << "  MessageRequester(const MessageRequester &) = delete;\n" ;
    OS << "  void operator=(const MessageRequester &) = delete;\n" ;
    OS << "};\n" ;
    OS << "\n" ;

    if (!Namespace.empty())
      OS << "} // end namespace " << Namespace << "\n";

    OS << "#endif//__"<<toMacroName(OutputFilename)<<"__\n" ;
  }
} // end namespace lyre
