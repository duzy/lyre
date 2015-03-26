proc foo(a:int) int
----
    decl b = 0;
    b = (a + 1) * 2;
    a = b * b;
    return a;
----

decl a = 1 + 2 + 3 + foo(4);

say("foo: %d", foo(0));
say("foo: %d", foo(1));
say("foo: %d", foo(4));
say("foo: %d", foo(5));
say("foo: %d", foo(foo(5)));
say("foo: %d", a);
say("foo: %d", foo(a));
say("foo: %d", foo(a * 5));
say("foo: %d", foo(a * 5) * 10);
