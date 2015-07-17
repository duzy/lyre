/** -*- c++ -*-
 *
 */
namespace authenticator
{
    namespace server
    {
        struct event_authenticate : sc::event< event_authenticate > {};
        struct event_reject       : sc::event< event_reject > {}; // signin fail
        struct event_serve        : sc::event< event_serve > {}; // new session ( session_manager )

        struct state_alive;
        struct state_idle;
        struct state_checking;

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

        template < class Derived >
        using state_alive_working = sc::state < Derived, state_alive::orthogonal<0> >;

        struct state_idle : state_alive_working < state_idle >
        {
            state_idle( my_context ctx ) : my_base(ctx)
            {
                //DBG( __PRETTY_FUNCTION__ );
            }
        };
    }
}
