include "messaging.td"

def Example : Component;

let COMPONENT = Example in {
    
    def nothing : Message<0>;
    
    def ping : Message<1, [ String<"text"> ]>;
    def pong : Message<2, [ String<"text"> ]>;

    def hello : Message<3, [ String<"token"> ]>;
    def welcome : Message<4, [ String<"token">, Uint32<"time"> ]>;

    def ExampleServer : Component, SOCK_REP;
    def ExampleClient : Component, SOCK_REQ;

}

let COMPONENT = ExampleServer in {

    /*
    def enter : Event;
    def send : Event;
    def receive : Event;
    */

    def receiving : State, ACT_ENTER;
    def alive : State;
    
}
