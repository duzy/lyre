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

    /*    
    def server_receiving : State, Do<[ ReceiveAction, TransitAction<"server_sending"> ]>;
    def server_sending : State, Do<[ SendAction, TransitAction<"server_receiving"> ]>;
    def server_alive : State<[ server_receiving, server_sending ]>;
    def server : RequestProcessMachine<[ server_alive ]>, SOCK_REP;

    def client_sending : State, Do<[ SendAction, TransitAction<"client_receiving"> ]>;
    def client_receiving : State, Do<[ ReceiveAction, TransitAction<"client_sending"> ]>;
    def client_alive : State<[ client_sending, client_receiving ]>;
    def client : ReplyProcessMachine<[ client_alive ]>, SOCK_REQ;
    */

    def wait_process_request : Action;
    def server_alive : State, Do<[ wait_process_request ]>;
    def server : RequestProcessMachine<[ server_alive ]>, SOCK_REP;

    def wait_process_reply : Action;
    def send : Event, Do<[ SendAction, TransitAction<"waiting_reply"> ]>;
    
    def waiting_reply : State, Do<[ wait_process_reply ]>;
    def client_alive : State, Can<[ send ]>;
    def client : ReplyProcessMachine<[ client_alive ]>, SOCK_REQ;
}
