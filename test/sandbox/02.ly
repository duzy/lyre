with { #* nothing but comment *# }
----------------------------------
----------------------------------

fun1 #* comment *#  (); 

fun2(1, 2, 3, fun3('a', 'b', 'c'));

speak template > bash > python
------------------------------
speak "blah, blah, blah"
------------------------------

type T1
-------
    std.out("blah, blah, ...");
-------

proc foo() --- ; ---

type T2
-------
    
-------

.name;

:tag(value);
:tag(1, 2, 3);

'this is a non-escape string';

"this is an escape string";

1 + 2;

(1 + 2) * 3;

!true;

1 == 1;
1 != 2;

std.out("blah, blah, blah...");

std #* inline comment *# .out ( "blah, blah, blah..." );

123.123;

a = 123;

{};
{ name:'foo', value:100 }.get("name");

with {} --- ---

with { #* nothing but comment *# }
------------------------------
------------------------------

with { name:'foo', value:100 }
------------------------------
    std.out("this is $(.name) from an in-mem node\n");
------------------------------

see v
-----------
-----------