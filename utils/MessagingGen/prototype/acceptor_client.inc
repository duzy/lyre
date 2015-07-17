/** -*- c++ -*-
 *
 */
namespace acceptor
{
    namespace client // send-wait-send
    {
        struct event_idle     : sc::event< event_idle >       {};
        struct event_send     : sc::event< event_send >       {};
        struct event_wait     : sc::event< event_wait >       {};
        struct event_error    : sc::event< event_error >      { int error;            explicit event_error(int e) : error(e) {}};

        struct state_alive;         // 

        struct state_idle;          //
        struct state_waiting;       // 

        struct state_watching;      // watching errors
        struct state_solving;       // solving an error

        struct machine : messaging::base_client_machine < machine, state_alive, protocol >
        {
            template < class Context, class ProtoAddres > machine( Context ctx, ProtoAddres addres )
                : base_client_machine(ctx, addres) {}
        };

        struct app : messaging::base_app < app, machine >
        {
            app(const std::vector<std::string> & endpoints) : base_app(endpoints) {}

            void hello(const std::string & text)
            {
                
            }
        };

        struct state_alive : sc::simple_state
        <
            state_alive, machine,
            meta::list < state_idle, state_watching >,
            sc::has_no_history
        >
        {
            typedef meta::list
            <
                sc::custom_reaction< sc::exception_thrown >
            > reactions;

            state_alive ()
            {
                //DBG( __PRETTY_FUNCTION__ );
            }

            ~state_alive ()
            {
            }

            sc::result react( const sc::exception_thrown & )
            {
                try
                    {
                        throw;
                    }
                catch ( const std::runtime_error & )
                    {
                        // only std::runtime_errors will lead to a transition
                        // to state_watching...
                        // return transit< state_watching >();
                    }
                catch ( ... )
                    {
                        // ... all other exceptions are forwarded to our outer
                        // state(s). The state machine is terminated and the
                        // exception rethrown if the outer state(s) can't
                        // handle it either...
                        return forward_event();
                    }
                
                // Alternatively, if we want to terminate the machine
                // immediately, we can also either rethrow or throw
                // a different exception.
            }
        };

        template < class Derived > using state_alive_working    = sc::state < Derived, state_alive::orthogonal<0> >;
        template < class Derived > using state_alive_diagnosing = sc::state < Derived, state_alive::orthogonal<1> >;

        /**
         *  idle and waiting to send messages.
         */
        struct state_idle : state_alive_working < state_idle >
        {
            typedef meta::list
            <
                sc::custom_reaction< event_send >
            > reactions;

            state_idle( my_context ctx ) : my_base(ctx)
            {
                DBG( __FUNCTION__ ); //DBG( __PRETTY_FUNCTION__ );
            }

            sc::result react (const event_send & ev)
            {
                if (do_one_send (ev)) {
                    post_event ( event_wait() );
                }

                // TODO: ???

                return discard_event ();
            }

        private:
            bool do_one_send (const event_send & ev)
            {
                // TODO: send one message from the queue somehow..
                return false;
            }
        };

        /**
         *  client is waiting for a reply after successfully sending a message.
         */
        struct state_waiting : state_alive_working < state_waiting >
        {
            typedef meta::list
            <
                sc::transition< event_idle, state_idle >
            > reactions;

            state_waiting( my_context ctx ) : my_base(ctx)
            {
                auto proto = context< machine >().protocol();

                if (!proto->receive_and_process_message (this)) {
                    auto const e = proto->error();
                    post_event ( event_error(e) );
                }

                /**
                 *  After trying to receive a message, we shall post an idle event to ensure the
                 *  idle-wait-process-idle cycle, where the 'process' state might post a 'dispatch'
                 *  event.
                 */
                post_event ( event_idle() );
            }

            template < class Message >
            void process_message (Message & m)
            {
                DBG (__FUNCTION__ << ": message: " << int(m.TAG));
            }
        };

        /**
         *  watching for errors.
         */
        struct state_watching : state_alive_diagnosing < state_watching >
        {
            typedef meta::list
            <
                sc::custom_reaction< event_error >
            > reactions;

            state_watching( my_context ctx ) : my_base(ctx)
            {
                // DBG( __FUNCTION__ ); // DBG( __PRETTY_FUNCTION__ );
            }

            sc::result react( const event_error & ev )
            {
                auto proto = context< machine >().protocol();
                auto const e = proto->error();

                /**
                 *  Here we're expecting the error could be solved in the error state processor.
                 */
                if (e == 11 /* e.g. Resource temporarily unavailable. */) {
                    DBG ("error: " << proto->strerror(e) );
                    /**
                     *  If the socket is temporarily unavailable (e.g. timeout when receiving a
                     *  message).
                     */
                    return discard_event ();
                }

                return transit <state_solving> ();
            }
        };

        /**
         *  solving an error occured.
         */
        struct state_solving : state_alive_diagnosing < state_solving >
        {
            state_solving( my_context ctx ) : my_base(ctx)
            {
                DBG( __FUNCTION__ ); // DBG( __PRETTY_FUNCTION__ );

                if (!this->solve()) {
                    // terminate();
                }
            }

        private:
            bool solve()
            {
                auto proto = context< machine >().protocol();
                auto const e = proto->error();
                DBG (__FUNCTION__ << ": TODO: error: " << e << ", " << proto->strerror(e) );
                return true;
            }
        };
    }
}
