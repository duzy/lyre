package com.lyrecode;

import com.lyrecode.messaging;
import com.lyrecode.messaging.Message;
import org.zeromq.ZMQ;
import org.zeromq.ZMQ.Context;
import org.zeromq.ZMQ.Socket;

public class example
{ 
  static class TestProtoServer extends Thread
  {
    ZMQ.Context context = null;
    
    public TestProtoServer(Context c) { context = c; }

    @Override
    public void run()
    {
      System.out.println("server: start");
      try (
           Socket socket = context.socket(ZMQ.REP);
           messaging.Protocol protocol = new messaging.Protocol();
           ) {
          socket.bind("ipc://test-channel-x");
          
          messaging.Message m = protocol.recv(socket);
          System.out.printf("server: received: "+m+"\n");
      } catch (Exception e) {
        System.out.println("E: server "+e);
      }
      System.out.println("server: stop");
    }
  }

  static class TestProtoClient extends Thread
  {
    ZMQ.Context context = null;
    
    public TestProtoClient(Context c) { context = c; }

    @Override
    public void run()
    {
      System.out.println("client: start");
      try (
           Socket socket = context.socket(ZMQ.REQ);
           messaging.Protocol protocol = new messaging.Protocol();
           ) {
          socket.connect("ipc://test-channel-x");
          
          Message.nothing m = new Message.nothing();
          if (protocol.send(socket, m)) {
            System.out.printf("client: sent: "+m+"\n");
          } else {
            System.out.printf("client: sent: failed\n");
          }
      } catch (Exception e) {
        System.out.println("E: client "+e);
      }
      System.out.println("client: stop");
    }
  }

  static class TestRequestProcessor extends Thread
  {
    ZMQ.Context context = null;

    TestRequestProcessor(ZMQ.Context c) { context = c; }
    
    @Override
    public void run()
    {
      System.out.println("server: start");
      try (
           Socket socket = context.socket(ZMQ.REP);
           messaging.RequestProcessor server = new messaging.RequestProcessor(socket) 
             {
               protected void onRequest(Message.nothing Q, Message.nothing P) {
                 System.out.println("request: nothing");
               }
               protected void onRequest(Message.ping Q, Message.pong P) {
                 System.out.println("request: ping.text = "+Q.text);
                 P.text = "pong reply";
               }
               protected void onRequest(Message.pong Q, Message.ping P) {
                 System.out.println("request: pong.text = "+Q.text);
                 P.text = "ping reply";
               }
               protected void onRequest(Message.hello Q, Message.welcome P) {
                 System.out.println("request: hello.token = "+Q.token);
                 P.token = "welcome-token";
                 P.time = 1234567890;
               }
             };
           ) {
        socket.bind("ipc://test-channel-xx");
        
        if (!server.waitProcessRequest()) System.out.println("E: wait request failed");
        if (!server.waitProcessRequest()) System.out.println("E: wait request failed");
        if (!server.waitProcessRequest()) System.out.println("E: wait request failed");
      } catch (Exception e) {
        System.out.println("E: request: "+e);
        e.printStackTrace();
      }
      System.out.println("server: stop");
    }
  };

  static class TestReplyProcessor extends Thread
  {
    ZMQ.Context context = null;
    
    TestReplyProcessor(ZMQ.Context c) { context = c; }

    @Override
    public void run()
    {
      System.out.println("client: start");
      try (
           Socket socket = context.socket(ZMQ.REQ);
           messaging.ReplyProcessor client = new messaging.ReplyProcessor(socket)
             {
               protected void onReply(Message.nothing P) {
                 System.out.println("reply: nothing");
               }
               protected void onReply(Message.pong P) {
                 System.out.println("reply: pong.text = "+P.text);
               }
               protected void onReply(Message.ping P) {
                 System.out.println("reply: ping.text = "+P.text);
               }
               protected void onReply(Message.welcome P) {
                 System.out.println("reply: welcome.token = "+P.token+", welcome.time = "+P.time);
               }
             };
           ) {
        socket.connect("ipc://test-channel-xx");
        
        Message.nothing m = new Message.nothing();
        if (client.send(m) == false) System.out.println("E: send request failed");
        System.out.println("client: sent: "+m);
        if (client.waitProcessReply() == false) System.out.println("E: wait reply failed");

        Message.ping ping = new Message.ping();
        ping.text = "ping";
        if (client.send(ping) == false) System.out.println("E: send request failed");
        System.out.println("client: sent: "+ping);
        if (client.waitProcessReply() == false) System.out.println("E: wait reply failed");

        Message.hello hello = new Message.hello();
        hello.token = "hello-token";
        if (client.send(hello) == false) System.out.println("E: send request failed");
        System.out.println("client: sent: "+hello);
        if (client.waitProcessReply() == false) System.out.println("E: wait reply failed");
      } catch (Exception e) {
        System.out.println("E: reply: "+e);
        e.printStackTrace();
      }
      System.out.println("client: stop");
    }
  };
  
  public static void main(String[] args) throws Exception 
  {
    ZMQ.Context context = ZMQ.context(5);
    
    new TestProtoClient(context).start();
    new TestProtoServer(context).start();

    // Run for 2 seconds
    Thread.sleep(2 * 1000);

    System.out.println("-------------------------");

    new TestRequestProcessor(context).start();
    new TestReplyProcessor(context).start();
    
    // Run for 2 seconds
    Thread.sleep(2 * 1000);

    context.close();

    System.out.println("done");
    System.out.println("-------------------------");
  }
}
