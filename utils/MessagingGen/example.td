include "messaging.td"

def Example : Application;

let App = Example in {
    
    def nothing : Message<0>;
    
    def ping : Message<1>;
    def pong : Message<2>;

    def hello : Message<3>;
    def welcome : Message<4>;
    
    let M = ping in {
        def text : Field<"string">;
    }

    let M = pong in {
        def text : Field<"string">;
    }
}
