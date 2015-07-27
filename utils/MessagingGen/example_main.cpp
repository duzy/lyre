#include "example.cc"
#include <thread>
#include <chrono>
#include <zmq.h>
#include <iostream>

static void test_protocol()
{
  std::thread tP([] {
      protocol proto(ZMQ_REP);
      proto.bind({ "inproc://example-protocol" });
      {
        nothing m;
        assert(proto.recv(m) == tag_size + 2 + proto.size(m));
        assert(proto.send(m) == tag_size + 2 + proto.size(m));
      }
      {
        ping req;
        pong res = { "example-pong" };
        assert(proto.recv(req) == tag_size + 2 + proto.size(req));
        assert(proto.send(res) == tag_size + 2 + proto.size(res));
        assert(req.text == "example-ping");
      }
    });

  std::thread tQ([] {
      protocol proto(ZMQ_REQ);
      proto.connect({ "inproc://example-protocol" });
      {
        nothing m;
        assert(proto.send(m) == tag_size + 2 + proto.size(m));
        assert(proto.recv(m) == tag_size + 2 + proto.size(m));
      }
      {
        ping req = { "example-ping" };
        pong res;
        assert(proto.send(req) == tag_size + 2 + proto.size(req));
        assert(proto.recv(res) == tag_size + 2 + proto.size(res));
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
      processor.bind({ "inproc://example-processor" });
      processor.wait_process_request(&processor);
    });

  std::thread tQ([] {
      reply_processor processor(ZMQ_REQ);
      processor.connect({ "inproc://example-processor" });
      processor.send(nothing());
    });

  tQ.join();
  tP.join();
}

struct server : request_processor
{
  server() : request_processor(ZMQ_REP) {}
protected:
  void on_request(const nothing &Q, nothing &P) override
  {
    std::clog << "request: nothing" << std::endl;
  }
  void on_request(const ping &Q, pong &P) override
  {
    std::clog << "request: ping: " << Q.text << std::endl;
    P.text = "example-pong";
  }
  void on_request(const pong &Q, ping &P) override
  {
    std::clog << "request: pong: " << Q.text << std::endl;
    P.text = "example-ping";
  }
  void on_request(const hello &Q, welcome &P) override
  {
    std::clog << "request: hello: " << Q.token << std::endl;
    P.token = "example-welcome", P.time = 123456;
  }
};

struct client : reply_processor
{
  client() : reply_processor(ZMQ_REQ) {}
protected:
  void on_reply(const nothing &m) override
  {
    std::clog << "reply: nothing" << std::endl;
  }
  void on_reply(const ping &m) override
  {
    std::clog << "reply: ping: " << m.text << std::endl;
  }
  void on_reply(const pong &m) override
  {
    std::clog << "reply: pong: " << m.text << std::endl;
  }
  void on_reply(const welcome &m) override
  {
    std::clog << "reply: welcome: " << m.token << ", " << m.time << std::endl;
  }
};

static void test_processors_2()
{
  std::thread tP([] {
      server S;
      S.bind({ "inproc://example-processor-2" });
      S.wait_process_request(&S);
      S.wait_process_request(&S);
      S.wait_process_request(&S);
      S.wait_process_request(&S);
    });

  std::thread tQ([] {
      client C;
      C.connect({ "inproc://example-processor-2" });

      C.send(nothing());
      C.wait_process_reply(&C);

      C.send(ping{ "example-ping" });
      C.wait_process_reply(&C);

      C.send(pong{ "example-pong" });
      C.wait_process_reply(&C);

      C.send(hello{ "example-hello" });
      C.wait_process_reply(&C);
    });

  tQ.join();
  tP.join();
}

int main(int argc, char **argv)
{
  /*
  test_protocol();
  test_processors();
  test_processors_2();
  */

  server S;
  S.bind({ "tcp://127.0.0.1:18888" });
  S.wait_process_request(&S);
  S.wait_process_request(&S);
  S.wait_process_request(&S);
  
  std::clog << "------------------------=" << std::endl;
  return 0;
}
