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
    });

  std::thread tQ([] {
      protocol proto(ZMQ_REQ);
      proto.connect({ "inproc://example" });
      {
        nothing m;
        assert(proto.send(m) == tag_size + proto.size(m));
        assert(proto.recv(m) == tag_size + proto.size(m));
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
