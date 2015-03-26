decl v variant;
decl n int;
decl m int;

n = 1;
v = 1;
v = n;

m = v;
say("m = %d", m);

m = v + 1;
say("m = %d", m);

m = v + n;
say("m = %d", m);

m = n + v + n;
say("m = %d", m);

m = n + v + v;
say("m = %d", m);

proc fun(a:int) int
----
    return a;
----

say("fun: %d", fun(2));
