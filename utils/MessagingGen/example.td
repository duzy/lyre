include "messaging.td"

def example : Namespace;

let NS = example in {
    
    def nothing : Message<0>;
    
    def ping : Message<1, [ String<"text"> ]>;
    def pong : Message<2, [ String<"text"> ]>;

    def hello : Message<3, [ String<"token"> ]>;
    def welcome : Message<4>, Fields<[ String<"token">, Uint64<"time"> ]>;

    def : Protocol<nothing, nothing>;
    def : Protocol<ping, pong>;
    def : Protocol<pong, ping>;
    def : Protocol<hello, welcome>;

    //========== State Machines ==========

    def send_request : Event, Do<[ Send, Transit<"waiting_reply"> ]>;

    def waiting_request : State,
        Enter<[ WaitProcessRequest ]>;
    def server_idle : State, Do<[ Transit<"waiting_request"> ]>;

    def waiting_reply : State,
        Enter<[ WaitProcessReply, Transit<"client_idle"> ]>;
    def client_idle : State, Can<[ send_request ]>;

    def server : RequestProcessMachine<[ server_idle ]>, SOCK_REP;
    def client : ReplyProcessMachine<[ client_idle ]>, SOCK_REQ;
}
