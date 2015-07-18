/** -*- c++ -*-
 *
 */
namespace session_talker
{
    struct protocol : messaging::base_protocol < protocol, messages::codec >
    {
        explicit protocol (int type) : base_protocol (type) {}
    };

    namespace server
    {
        struct event_create   : sc::event< event_create >   {};
        struct event_protocol : sc::event< event_protocol > {}; // turn into working state.
        struct event_signout  : sc::event< event_signout >  {}; // got a signout protocol

        struct state_alive;
        struct state_idle;
        struct state_working; // on a protocol

        struct machine : messaging::base_server_machine < machine, state_alive, protocol >
        {
            template < class Context, class ProtoBinds > machine( Context ctx, ProtoBinds binds )
                : base_server_machine(ctx, binds) {}
        };

        struct app : messaging::base_app < app, machine >
        {
            app(const std::vector<std::string> & endpoints) : base_app(endpoints) { /*DBG(__PRETTY_FUNCTION__);*/ }
        };

        struct state_alive : sc::simple_state
        <
            state_alive, machine,
            meta::list < state_idle >,
            sc::has_no_history
            >
        {
            state_alive()
            {
                //DBG( __PRETTY_FUNCTION__ );
            }
        };

        struct state_idle : sc::state < state_idle, state_alive::orthogonal<0> >
        {
            state_idle( my_context ctx ) : my_base(ctx)
            {
                //DBG( __PRETTY_FUNCTION__ );
            }
        };
    }
}

