# Returns 'void'
proc foo1(a:int)
----
    say("foo1:void: %d", a);
----

# Returns an 'int'
proc foo2(a:int)
----
    say("foo2:<int>: %d", a);
    return a + 1;
----

# Declared as 'variant' explicitly and only returns a 'variant'
proc foo3(a:int) variant
----
    decl v variant
    v = a;
    return v;
----

foo1(1);

say("foo2: %d", foo2(1));

decl v = foo3(1);
decl n int = v;
say("foo3: %d", n);

decl v2 = foo3(1);
decl n1 int;
decl n2 int;
n1 = v2;
n2 = foo3(1);
say("foo3: %d", n1);
say("foo3: %d", n2);
