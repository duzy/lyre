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
    });

  tQ.join();
  tP.join();
}

static void test_processors()
{
  std::thread tP([] {
      request_processor processor(ZMQ_REP);
      processor.bind({ "inproc://example" });
      {
        nothing m;
        assert(processor.recv(m) == tag_size + processor.size(m));
        assert(processor.send(m) == tag_size + processor.size(m));
      }
    });

  std::thread tQ([] {
      reply_processor processor(ZMQ_REQ);
      processor.connect({ "inproc://example" });
      {
        nothing m;
        assert(processor.send(m) == tag_size + processor.size(m));
        assert(processor.recv(m) == tag_size + processor.size(m));
      }
    });

  tQ.join();
  tP.join();
}

int main(int argc, char **argv)
{
  test_protocol();
  test_processors();
  return 0;
}
