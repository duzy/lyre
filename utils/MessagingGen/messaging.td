// http://llvm.org/docs/TableGen/LangIntro.html
// http://llvm.org/docs/TableGen/LangRef.html

class SocketType;
def SocketTypeREP : SocketType;
def SocketTypeREQ : SocketType;

class Component {
    Component COMPONENT = ?; // Parent component if defined.
    SocketType TYPE = ?;
}
class SOCK_REP { SocketType TYPE = SocketTypeREP; }
class SOCK_REQ { SocketType TYPE = SocketTypeREQ; }

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
    int ID = id;
    Component COMPONENT = ?;
    list<FieldBase> FIELDS = fields;
}
// Allowing 'def msg : Message<1>, Fields<[String["text"]]>;'
class Fields<list<FieldBase> fields = []> { list<FieldBase> FIELDS = fields; }

class State {
    Component COMPONENT = ?;
}

class Action<string name> {
    string NAME = name;
}

class Event {
    State STATE = ?;
    list<Action> ACTIONS = [];
}

class ACT_ENTER {
    list<Action> ACTIONS = !listconcat(ACTIONS, Action<"enter">);
}
