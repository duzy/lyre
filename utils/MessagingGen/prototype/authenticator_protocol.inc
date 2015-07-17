/** -*- c++ -*-
 *
 */
namespace authenticator
{
    struct protocol : messaging::base_protocol < protocol, messages::codec >
    {
        explicit protocol (int type) : base_protocol (type) {}
    };
}
