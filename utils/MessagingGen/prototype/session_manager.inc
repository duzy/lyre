/** -*- c++ -*-
 *
 */
namespace session_manager
{
    struct protocol : messaging::base_protocol < protocol, messages::codec >
    {
        explicit protocol (int type) : base_protocol (type) {}
    };

    namespace server
    {
        struct event_new  : sc::event< event_new > {};
        struct event_kill : sc::event< event_kill > {};

        struct state_alive;
        struct state_idle;
        struct state_newing;
        struct state_killing;

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
            state_alive ()
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
