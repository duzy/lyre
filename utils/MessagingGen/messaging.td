class Component<string type> {
    string Type = type;
}

class Application : Component<""> {
}

class Server<string type> : Component<type> {
}

class Client<string type> : Component<type> {
}

class Message<int id> {
    Component App = ?;
    int Id = id;
}

class Field<string type> {
    Message M = ?;
    string Type = type;
}

class State {
    Component App = ?;
}

class Event {
    State S = ?;
}

class Action {
    Event E = ?;
}
