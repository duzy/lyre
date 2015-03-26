# Returns 'void'
proc foo1(a:int)
----
    say("foo1:void: %d", a);
----

# Returns an 'int'
proc foo2(a:int) int
----
    say("foo2:<int>: %d", a);
    return a + 1;
----

# Declared as 'variant' explicitly and only returns a 'variant'
proc foo3(a:int) variant
----
    say("foo3:<int>: %d", a);

    decl v variant;
    v = a + 1;
    return v;
----

foo1(1);

say("foo2: %d", foo2(1));

decl v = foo3(1);
decl n1 int;
decl n2 int;
n1 = v;
v = foo3(1);
n2 = foo3(1);
say("foo3: %d", n1);
say("foo3: %d", n2);

n1 = v;
say("foo3: %d", n1);

v = 100;
n2 = v;
say("foo3: %d", n2);
