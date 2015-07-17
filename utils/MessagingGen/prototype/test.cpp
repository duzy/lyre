#include "../messaging.inc"
#include "acceptor_protocol.inc" /* This should be code-generated. */
#include "acceptor_server.inc" /* This should be code-generated. */
#include "acceptor_client.inc" /* This should be code-generated. */
//#include "authenticator_protocol.inc"
//#include "authenticator_server.inc"
//#include "authenticator_client.inc"
//#include "session_manager.inc"
//#include "session_talker.inc"

struct server
{
    server() : _threads () {}

    ~server() = default;

    void serve()
    {
        _threads.push_back ( launch< acceptor::server::app        >({ "inproc://acceptor"        }) );
        //_threads.push_back ( launch< authenticator::server::app   >({ "inproc://authenticator"   }) );
        //_threads.push_back ( launch< session_manager::server::app >({ "inproc://session-manager" }) );
        //_threads.push_back ( launch< session_talker::server::app  >({ "inproc://session-talker"  }) );

        join();
    }

private:
    template < class Server >
    std::thread launch(const std::initializer_list<std::string> && a)
    {
        std::vector<std::string> addres(a.begin(), a.end());
        auto worker = [ addres ] {
            std::unique_ptr< Server > server ( new Server(addres) );
            server->run();
        };
        return std::thread ( worker );
    }

    void join()
    {
        for (auto & t : _threads) t.join();
    }

    std::vector< std::thread > _threads;
};

void test_acceptor_protocol()
{
    std::this_thread::sleep_for( std::chrono::milliseconds(100) );

    acceptor::protocol proto (ZMQ_REQ);
    proto.connect ({ "inproc://acceptor" });

    int rc;

    typedef acceptor::protocol::codec codec;

    acceptor::messages::ping ping ("test");
    rc = proto.send_message (ping);
    DBG (__FUNCTION__ << ": sent: ping: " << rc << ", " << ping.text);
    assert (rc == sizeof (acceptor::messages::tag) + codec::size (&proto, ping));
    assert (codec::size (&proto, ping) == sizeof (uint8_t) + ping.text.size ());

    acceptor::messages::pong pong;
    rc = proto.recv_message (pong);
    DBG (__FUNCTION__ << ": received: pong: " << rc << ", " << pong.text.size() << ", " << pong.text);
    assert (!pong.text.empty() && ping.text == pong.text);

    acceptor::messages::hello hello;
    hello.token = "token-client-test";
    rc = proto.send_message (hello);
    DBG (__FUNCTION__ << ": sent: hello: " << rc << ", " << codec::size (&proto, hello));
    assert (rc == codec::size (&proto, hello) + sizeof (acceptor::messages::tag));

    hello.token.clear ();
    rc = proto.recv_message (hello);
    DBG (__FUNCTION__ << ": received: hello: " << rc << ", " << hello.token);
    assert (rc == codec::size (&proto, hello) + sizeof (acceptor::messages::tag));
}

void test_acceptor_client ()
{
    acceptor::client::app client(std::vector<std::string>{ "inproc://acceptor" });
    client.run();
}

void test_authenticator_protocol ()
{
}

void test_authenticator_client ()
{
}

void test()
{
    test_acceptor_protocol ();
    test_acceptor_client ();

    test_authenticator_protocol ();
    test_authenticator_client ();
}

int main()
{
    std::thread t(test);

    server s;
    s.serve();

    t.join();
    return EXIT_SUCCESS;
}
