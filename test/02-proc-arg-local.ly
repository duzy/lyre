proc fun(a:int) int
----
    decl v = a + 1;
    a = 1;
    a = v + a;
    return a;
----

say("fun: %d", fun(2));
