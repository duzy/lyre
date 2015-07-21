#include "example.cc"
#include <thread>
#include <chrono>
#include <zmq.h>

static void test_protocol()
{
  std::thread tP([] {
      protocol proto(ZMQ_REP);
      proto.bind({ "inproc://example" });
      {
        nothing m;
        assert(proto.recv(m) == tag_size + proto.size(m));
        assert(proto.send(m) == tag_size + proto.size(m));
      }
      {
        ping req;
        pong res = { "example-pong" };
        assert(proto.recv(req) == tag_size + proto.size(req));
        assert(proto.send(res) == tag_size + proto.size(res));
        assert(req.text == "example-ping");
      }

      proto.receive_and_process(&proto);
    });

  std::thread tQ([] {
      protocol proto(ZMQ_REQ);
      proto.connect({ "inproc://example" });
      {
        nothing m;
        assert(proto.send(m) == tag_size + proto.size(m));
        assert(proto.recv(m) == tag_size + proto.size(m));
      }
      {
        ping req = { "example-ping" };
        pong res;
        assert(proto.send(req) == tag_size + proto.size(req));
        assert(proto.recv(res) == tag_size + proto.size(res));
        assert(res.text == "example-pong");
      }

      {
        nothing m;
        assert(proto.send(m) == tag_size + proto.size(m));
        //assert(proto.recv(m) == tag_size + proto.size(m));
      }
    });

  tQ.join();
  tP.join();
};

int main(int argc, char **argv)
{
  test_protocol();
  return 0;
}
