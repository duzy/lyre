// http://llvm.org/docs/TableGen/LangIntro.html
// http://llvm.org/docs/TableGen/LangRef.html

class Namespace {
    Namespace NS = ?; // Parent namespace if defined.
}

class FieldBase<string name> { string NAME = name; }
class Int8<string name>   : FieldBase<name>;
class Int16<string name>  : FieldBase<name>;
class Int32<string name>  : FieldBase<name>;
class Int64<string name>  : FieldBase<name>;
class Uint8<string name>  : FieldBase<name>;
class Uint16<string name> : FieldBase<name>;
class Uint32<string name> : FieldBase<name>;
class Uint64<string name> : FieldBase<name>;
class String<string name> : FieldBase<name>;

// Allowing 'def msg : Message<1, [String["text"]]>;'
class Message<int id, list<FieldBase> fields = []> {
    Namespace NS = ?;
    int ID = id;
    list<FieldBase> FIELDS = fields;
}
// Allowing 'def msg : Message<1>, Fields<[String["text"]]>;'
class Fields<list<FieldBase> fields = []> { list<FieldBase> FIELDS = fields; }

//========

class SocketType;
def SocketTypeREP : SocketType;
def SocketTypeREQ : SocketType;

class SOCK_REP { SocketType TYPE = SocketTypeREP; }
class SOCK_REQ { SocketType TYPE = SocketTypeREQ; }

//========---- State Machine related ----========

class Action {
    Namespace NS = ?;
}

class Event {
    Namespace NS = ?;
    list<Action> ACTIONS = [];
}

class State {
    Namespace NS = ?;
    list<Event> EVENTS = []; // accepted events in this state
    list<State> STATES = []; // sub-states
}

class StateMachine {
    Namespace NS = ?; // Parent namespace if defined.
    SocketType TYPE = ?;
}

//========---- Standard Actions for common messaging tasks. ----========

// Action to transit to another state.
def TransitAction : Action { 
    State STATE; // Target state.
}

// Action to post an event to the state machine.
def PostAction : Action {
    Event EVENT; // Event to post.
}

/*
// Action to send a message immediately.
def SendAction : Action {
    string MESSAGE; // e.g. "event.message"; ""
};
*/

// Action to queue a message for pending sending.
def QueueAction : Action {
    string MESSAGE; // e.g. "event.message"; ""
}

// Action to send a message from the sending queue (synchronously).
def SendQueuedAction : Action {
}

// Action to wait and receive a message and dispatch to the handlers.
def ReceiveDispatchAction : Action {
}

// Action to call external function, e.g. user defined function.
def ExternalAction : Action {
    
}
