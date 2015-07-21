/**  -*- c++ -*-
 *
 *  The common messaging part.
 *  
 */
#include <boost/algorithm/string/split.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/asynchronous_state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/exception_translator.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/noncopyable.hpp>
#include <cstdint>
#include <thread>
#include <chrono>
#include <memory>
#include <mutex>
#include <zmq.h>

namespace { static std::mutex logmux; }
#define DBG(x)                                            \
    do {                                                  \
        std::lock_guard <std::mutex> lock (logmux);       \
        std::clog << __FILE__ << ":" << __LINE__ << ": "  \
                  << x << std::endl;                      \
    } while (false);

namespace meta = boost::mpl;
namespace sc = boost::statechart;

namespace {
typedef std::int8_t     Int8;
typedef std::int16_t    Int16;
typedef std::int32_t    Int32;
typedef std::int64_t    Int64;
typedef std::uint8_t    Uint8;
typedef std::uint16_t   Uint16;
typedef std::uint32_t   Uint32;
typedef std::uint64_t   Uint64;
typedef std::string     String;

namespace messaging
{
    static struct zmq_wrapper
    {
        zmq_wrapper() : zmq_ctx( zmq_ctx_new () ) {}
        ~zmq_wrapper() { zmq_ctx_term (zmq_ctx); }

        inline void *context() const { return zmq_ctx; }

    private:
        void * zmq_ctx;
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
     *  Basic server combins a state machine.
     */
    template < class Derived, class Machine >
    struct base_app
    {
        typedef sc::fifo_scheduler<> Scheduler;

        void quit ()
        {
            _scheduler.terminate();
        }

        void run ()
        {
            // while (!_scheduler.terminated()) {
            //     _scheduler (/* MaxEventCount = */ 1);
            // }
            _scheduler ();
        }

        virtual ~base_app() = default;

        template < class Event >
        void post()
        {
            this->post (boost::intrusive_ptr< Event >(new Event()));
        }

        template < class Event, class A >
        void post(const A & a)
        {
            this->post (boost::intrusive_ptr< Event >(new Event(a)));
        }

        template < class Event, class A1, class A2 >
        void post(const A1 & a1, const A2 & a2)
        {
            this->post (boost::intrusive_ptr< Event >(new Event(a1, a2)));
        }

        template < class Event, class A1, class A2, class A3 >
        void post(const A1 & a1, const A2 & a2, const A3 & a3)
        {
            this->post (boost::intrusive_ptr< Event >(new Event(a1, a2, a3)));
        }

        template < class Event, class A1, class A2, class A3, class A4 >
        void post(const A1 & a1, const A2 & a2, const A3 & a3, const A4 & a4)
        {
            this->post (boost::intrusive_ptr< Event >(new Event(a1, a2, a3, a4)));
        }

        template < class Event, class A1, class A2, class A3, class A4, class A5 >
        void post(const A1 & a1, const A2 & a2, const A3 & a3, const A4 & a4, const A5 & a5)
        {
            this->post (boost::intrusive_ptr< Event >(new Event(a1, a2, a3, a4, a5)));
        }

    protected:
        base_app()
            : _scheduler ( true )
            , _processor ( _scheduler.create_processor < Machine > () )
        {
            _scheduler.initiate_processor( _processor );
        }

        template < class Arg1 >
        base_app( const Arg1 & arg1 )
            : _scheduler ( true )
            , _processor ( _scheduler.create_processor < Machine > (arg1) )
        {
            _scheduler.initiate_processor( _processor );
        }

        void post ( const typename Scheduler::event_ptr_type & event )
        {
            _scheduler.queue_event ( _processor, event );
        }

    private:
        typedef typename Scheduler::processor_handle Processor;

        Scheduler _scheduler;
        Processor _processor;
    };

    struct user_defined_message
    {
        virtual ~user_defined_message() = default;
    };

    /**
     *  Basic state machine (asynchronous) combining with a protocol.
     */
    template < class Derived, class InitState, class Protocol >
    struct base_machine : sc::asynchronous_state_machine < Derived, InitState >
    {
        template < class Context > base_machine( Context ctx, int type )
            : sc::asynchronous_state_machine < Derived, InitState > (ctx)
            , _protocol ( new Protocol(type) )
        {
        }

        virtual ~base_machine()
        {
            for (auto m : _messages) { delete m; }
        }

        Protocol *protocol() const { return _protocol.get(); }

        void queue(user_defined_message *m) { _messages.push_back(m); }

        user_defined_message *dequeue()
        {
            auto m = _messages.front();
            _messages.pop_front();
            return m;
        }

    protected:
        template < class Iterator > void bind (Iterator begin, Iterator end) { _protocol->bind (begin, end); }
        template < class Iterator > void unbind (Iterator begin, Iterator end) { _protocol->unbind (begin, end); }
        template < class Iterator > void connect (Iterator begin, Iterator end) { _protocol->connect (begin, end); }
        template < class Iterator > void disconnect (Iterator begin, Iterator end) { _protocol->disconnect (begin, end); }

    private:
        std::unique_ptr<Protocol> _protocol;
        std::list<user_defined_message*> _messages;
    };

    /**
     *  Basic state machine (asynchronous) combining with a protocol.
     */
    template < class Derived, class InitState, class Protocol >
    struct base_server_machine : base_machine < Derived, InitState, Protocol >
    {
        template < class Context, class ProtoAddres > base_server_machine( Context ctx, ProtoAddres addres )
            : base_machine < Derived, InitState, Protocol > (ctx, ZMQ_REP)
        {
            this->protocol()->bind (addres.begin(), addres.end());
        }
    };

    /**
     *  Basic state machine (asynchronous) combining with a protocol.
     */
    template < class Derived, class InitState, class Protocol >
    struct base_client_machine : base_machine < Derived, InitState, Protocol >
    {
        template < class Context, class ProtoAddres > base_client_machine( Context ctx, ProtoAddres addres )
            : base_machine < Derived, InitState, Protocol > (ctx, ZMQ_REQ)
        {
            this->protocol()->connect (addres.begin(), addres.end());
        }
    };

    /**
     *  Basic protocol combining with a codec implementation.
     */
    template < class Derived, template <class P, class A> class Codec >
    struct base_protocol
    {
        struct accessor
        {
            template < class... T > static auto field_size(Derived *p, T&... t) -> decltype(p->field_size(t...)) { return p->field_size(t...); }
            template < class... T > static auto get(Derived *p, T&... t) -> decltype(p->get(t...)) { return p->get(t...); }
            template < class... T > static auto put(Derived *p, T&... t) -> decltype(p->put(t...)) { return p->put(t...); }
        };

        using codec = Codec<Derived, accessor>; //typedef Codec<Derived, accessor> codec;

        ~base_protocol () = default;

        template <class Iterator>
        void bind (Iterator begin, Iterator end) { _socket.bind (begin, end); }
        void bind (const std::initializer_list<std::string> && a) { bind (a.begin(), a.end()); }

        template <class Iterator>
        void connect (Iterator begin, Iterator end) { _socket.connect (begin, end); }
        void connect (const std::initializer_list<std::string> && a) { connect (a.begin(), a.end()); }

        template < class Message >
        auto size (const Message & m) const //-> decltype(codec::size(this, m)) 
        { return codec::size(static_cast<Derived*>(const_cast<base_protocol*>(this)), m); }

        template < class Message >
        int send (const Message & m)
        {
            auto size = codec::size (static_cast<Derived*>(this), m);
            this->reset ( sizeof (typename codec::tag_value_t) + size );
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

            auto tag = typename codec::tag_t( get<typename codec::tag_value_t>() );
            
            if (codec::t(m) == tag) {
                codec::decode (static_cast<Derived*>(this), m);
            } else {
                DBG ("error: " << __FUNCTION__ << ": tag not match: " << int(tag) << " != " << int(codec::t(m)));
                rc = -1;
            }

            return rc;
        }

        /**
         *  On errors, it should return false, and the error() should return the error code.
         */
        template < class Context >
        bool receive_and_process ( Context *ctx )
        {
            int rc = recv_with_flags (0);
            if (rc < 0) {
                if (error () == 11 /* e.g. Resource temporarily unavailable. */) {
                    /**
                     *  It could be timeout on receiving messages.
                     */
                } else {
                    DBG ("error: " << __FUNCTION__ << ": " << error() << ", " << strerror());
                }
                return false;
            }

            auto tag = typename codec::tag_t( get<typename codec::tag_value_t>() );
            return codec::parse ( static_cast<Derived*>(this), ctx, tag );
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
        std::size_t field_size (const std::string & s, bool longstr = false) const {
            return (longstr ? sizeof(Uint32) : sizeof(Uint8)) + s.size();
        }

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

        void put (const std::string & s, bool longstr = false)
        {
            std::size_t size = s.size();
            longstr ? put(Uint32(size)) : put(Uint8(size));
            memcpy (_cur, &s[0], size);
            _cur += size;
        }

        int get (std::string & s, bool longstr = false)
        {
            std::size_t size = std::size_t( longstr ? get<Uint32>() : get<Uint8>() );
            if (_end < _cur + size) return -1;
            s.resize( size );
            assert (_cur != NULL);
            memcpy (&s[0], _cur, size);
            _cur += size;
            return size;
        }

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
         *  Each time the protocol is about to encode a message, this `reset` must be called.
         */
        void reset (std::size_t framesize)
        {
            _message.clear ();

            std::unique_ptr<frame> f( new frame (framesize) );
            _cur = f->data ();
            _end = _cur + f->size ();
            _message.push_back (f.release());
        }

        template < class Message >
        void encode_message (const Message & m) { codec::encode(static_cast<Derived*>(this), m); }

        template < class Message >
        void decode_message (Message & m) { codec::decode(static_cast<Derived*>(this), m); }

    private:
        std::mutex _sockmux;
        socket _socket;
        message _message;
        Uint8 *_cur;
        Uint8 *_end;
    };

    namespace actors
    {
        struct sender
        {
            virtual void send() = 0;
        };

        struct receiver
        {
            virtual void receive() = 0;
        };
    }
} // end namespace messaging
} // end anonymous namespace
