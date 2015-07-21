include "messaging.td"

def example : Namespace;

let NS = example in {
    
    def nothing : Message<0>;
    
    def ping : Message<1, [ String<"text"> ]>;
    def pong : Message<2, [ String<"text"> ]>;

    def hello : Message<3, [ String<"token"> ]>;
    def welcome : Message<4>, Fields<[ String<"token">, Uint32<"time"> ]>;

    def : Protocol<nothing, nothing>;
    def : Protocol<ping, pong>;
    def : Protocol<hello, welcome>;

    def ExampleServer : StateMachine, SOCK_REP;
    def ExampleClient : StateMachine, SOCK_REQ;

    def enter : Event;
    def send : Event;
    def receive : Event;

    def receiving : State;
    def sending : State;
    def alive : State;

}
