/** -*- c++ -*-
 *
 */
namespace acceptor
{
    namespace messages
    {
        typedef uint8_t tag_value_base;

        enum class tag : tag_value_base
        {
            NOTHING, // server to client to discard a message 

            PING, // client to server
            PONG, // server to client as replying a PING message

            HELLO, // client to server, greeting
        
            SIGNIN,
            SIGNOUT,
        };

        template <tag T>
        struct base
        {
            static constexpr tag TAG = T;
        };

        struct nothing : base <tag::NOTHING> {};

        struct ping : base <tag::PING>
        {
            ping () : text() {}
            explicit ping (const std::string & s) : text(s) {}
            std::string text;
        };

        struct pong : base <tag::PONG>
        {
            pong () : text() {}
            explicit pong (const std::string & s) : text(s) {}
            std::string text;
        };

        struct hello : base <tag::HELLO>
        {
            std::string token; // per client token (identity)
        };

        struct signin : base <tag::SIGNIN>
        {
        };

        struct signout : base <tag::SIGNOUT>
        {
        };

        template < class P, class accessor >
        struct codec
        {
            typedef messages::tag_value_base        tag_value_base;
            typedef messages::tag                   tag;

            static std::size_t size (P *p, const nothing & m) { return 0; }
            static std::size_t size (P *p, const ping & m)    { return accessor::field_size (p, m.text); }
            static std::size_t size (P *p, const pong & m)    { return accessor::field_size (p, m.text); }
            static std::size_t size (P *p, const hello & m)   { return accessor::field_size (p, m.token); }

            static void encode (P *p, const nothing & m) {}
            static void decode (P *p, const nothing & m) {}

            static void encode (P *p, const ping & m)
            {
                accessor::put (p, m.text);
            }
            static void decode (P *p, ping & m)
            {
                int n = accessor::get (p, m.text);
                // DBG ("ping: " << n << ", " << m.text);
            }

            static void encode (P *p, const pong & m)
            {
                accessor::put (p, m.text);
            }
            static void decode (P *p, pong & m)
            {
                int n = accessor::get (p, m.text);
                // DBG ("pong: " << n << ", " << m.text);
            }

            static void encode (P *p, const hello & m)
            {
                accessor::put (p, m.token);
            }
            static void decode (P *p, hello & m)
            {
                int n = accessor::get (p, m.token);
            }

            static void encode (P *p, const signin & m)
            {
            }
            static void decode (P *p, signin & m)
            {
            }

            static void encode (P *p, const signout & m)
            {
            }
            static void decode (P *p, signout & m)
            {
            }

            template < class Message, class Context >
            static void parse (P *p, Context *ctx)
            {
                Message m;
                decode (p, m);
                p->process_message (ctx, m);
            }

            template < class Context >
            static bool parse (P *p, Context *ctx, tag t)
            {
                switch (t) {
                case tag::PING:           parse<ping>       (p, ctx); return true;
                case tag::PONG:           parse<pong>       (p, ctx); return true;
                case tag::HELLO:          parse<hello>      (p, ctx); return true;
                case tag::SIGNIN:         parse<signin>     (p, ctx); return true;
                case tag::SIGNOUT:        parse<signout>    (p, ctx); return true;
                default:                                              return false;
                }
            }
        };
    }

    struct protocol : messaging::base_protocol < protocol, messages::codec >
    {
        explicit protocol (int type) : base_protocol (type) {}

        template < class Context, class Message >
        void process_message (Context *ctx, Message & m)
        {
            ctx->process_message (m);
        }

        template < class Context >
        void process_message (Context *, messages::ping & m)
        {
            DBG (__FUNCTION__ << ": ping: " << m.text);
            messages::pong pong (m.text);
            send_message (pong);
        }

        template < class Context >
        void process_message (Context *, messages::pong & m)
        {
            DBG (__FUNCTION__ << ": pong: " << m.text);
            send_message (messages::nothing()); // ERROR!
        }

        template < class Context >
        void process_message (Context *, messages::hello & m)
        {
            DBG (__FUNCTION__ << ": hello: " << m.token);
            messages::hello reply;
            reply.token = "token-nil";
            send_message (reply);
        }
    };
}
