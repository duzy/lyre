package com.lyrecode;

import com.lyrecode.messaging;
import org.zeromq.ZMQ;
import org.zeromq.ZMQ.Context;
import org.zeromq.ZMQ.Socket;

public class example
{ 
  static class TestProtoServer extends Thread
  {
    private messaging.Protocol protocol = new messaging.Protocol();
    private Socket socket = null;
    
    public TestProtoServer(Context c)
    {
      socket = c.socket(ZMQ.REP);
    }

    @Override
    public void run()
    {
      System.out.println("Start server thread");
      try {
        socket.bind("ipc://test-server-x");

        messaging.Message m = protocol.recv(socket);
        System.out.printf("server: received: \n");
      } catch (Exception e) {
        System.out.println("E: server "+e);
      }

      socket.close();
      System.out.println("Stop server thread");
    }
  }

  static class TestProtoClient extends Thread
  {
    private messaging.Protocol protocol = new messaging.Protocol();
    private Socket socket = null;
    
    public TestProtoClient(Context c)
    {
      socket = c.socket(ZMQ.REQ);
    }

    @Override
    public void run()
    {
      System.out.println("Start client thread");
      try {
        socket.connect("ipc://test-server-x");
        
        messaging.Message m = new messaging.Message.nothing();
        if (protocol.send(socket, m)) {
          System.out.printf("client: sent: nothing\n");
        } else {
          System.out.printf("client: sent: failed\n");
        }
      } catch (Exception e) {
        System.out.println("E: client "+e);
      }

      socket.close();
      System.out.println("Stop client thread");
    }
  }
  
  public static void main(String[] args) throws Exception 
  {
    ZMQ.Context context = ZMQ.context(1);
    
    new TestProtoClient(context).start();
    new TestProtoServer(context).start();

    //  Run for 10 seconds then quit
    Thread.sleep(10 * 1000);

    context.close();

    System.out.println("done");
    System.out.println("-------------------------");
  }
}
