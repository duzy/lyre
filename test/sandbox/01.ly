#* inline comment *#

with { name:'foo', value:100 }
------------------------------
    std.out("this is $(.name) from an in-mem node\n");
------------------------------

with { name : 'bar' } --- std.outln("$(.name)") ---

with :load("something.xml"); # apply to global scope

std.outln("this is $(.node.name) from something.xml");

decl v = 1, vv = 2;

type c (a1, a2)
-------------- # this is a special case of one line comment
    decl .name = 0;
    decl .type = "?";

    .name = 100 + len(.type);

    func foo(a, b, c)
    -----------------
        std.out( "something $(1+2)" );
    -----------------

    func .foo(a, b, c)
    ------------------
        see .name
        ----------
            foo(a, b, c);
        ----------
        .bar(a);
        .bar(b);
        .bar(c);
        ..name(); # accessing parent scope
        _.bar(a);
        _.bar(b);
        _.bar(c);
        _..name();
    ------------------

    func .bar(a)
    ------------
        
    ------------
------------

func foo(a, b, c)
-----------------
    any :one { 1 < _ } [1, 2, 3, 4, 5, 6, 7, 8, 9]
-----------------

decl a speak template --- blah blah blah $(_)... ---
decl b = speak template --- blah blah blah... ---

std.outln(a(_));
std.outln(b);

speak template
------------------------
any text would go here...
.   if a == 0

.   end
blah, blah, blah...
------------------------

with { name:'foo' } speak template
--------------------------------------
$(.name)...
--------------------------------------

speak template > bash
-------------------------
# This is a common Bash script block
.....if 1
echo "okay, go this branch"
.........
echo "hmmm, this is discarded"
.....end
-------------------------

see a == 1
-----------
    see b == 0
    -----------
        
    -----------
-----------
    #* blah blah... *#
-----------

see v
------:= 1
    std.outln("this is case 1");
------:= 2
    std.outln("this is case 2");
------:= 3
    std.outln("this is case 3");
------
    std.outln("this is case anyting else");
------

speak rule
--------------------------------------------------
    "some/file/a" : "something"
    --------------------------
        see v
        --------
            std.outln("blah blah blah...");
        --------
    --------------------------

    "some/file/b" : "something"
    --------------------------:exec("bash")
        echo "blah blah blah"
    --------------------------
--------------------------------------------------
