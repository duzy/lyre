# variable declaration
decl a_boolean = true;
decl a_integer = 0;
decl a_float = 0.1;
decl a_string = "foobar";

# A 'node' is a representation of any key-value based objects. In advance, a 'node' here could have
# parent and children, thus they could be used to construct a 'tree'. And lyre have a bundle of
# language facility to conveniently operate on a node tree.
decl a_node = { name:'foobar' };

# scalar: byte (1), int (4), float (4), string
# array, vector, map, type

# variable declaration without initialization
decl another_boolean bool;
decl another_integer int;
decl another_float float;
decl another_string string;

# variant variable, could be working with any type
decl a_variant;
a_variant = a_boolean; # okay
a_variant = a_integer; # okay
a_variant = a_float;   # okay
a_variant = a_string;  # okay

see a_variant is string
---
    say("a_variant is a string");
---

# function definition returning an integer. Blocks are enclosed with 3 or more dashes.
proc a_function(arg1:int, arg2:string) int
----
    decl a int # declare a local variable with unknown value

    a = arg1 * 2;

    # this is an error -- the name '_' can't be used in a 'decl' statement.
    # The name '_' is specially used as "the current node", it's type is always a builtin 'node'.
    # Yet the name '_' could be used as a argument, and forcely be type 'node'. So specifying a
    # 'type attribute' for the '_' argument is forbidded.
    decl _ int

    return a;
----

# type alias
type int32 is int:bits(32)
type uint is int:unsigned
type uint32 is int:unsigned:bits(32)
type uint256 is int:unsigned:bits(256)
type float256 is float:bits(256)

# define a 'class' type, which accepts two arguments when creating instances.
type a_class (arg1:string, arg2:int)
----
    # declare fields (private), and initialized with args
    decl .name = arg1
    decl .age = arg2

    # declare a public field
    decl .Weight int

    # other statements are allowed, and they are treated as in the type constructor
    see .name == "" --- .name = "anonymous" ---
    see .age <= 0   --- .age = 1 ---

    # define a static procedure, e.g. the name is not started with dot
    # 
    proc a_static_function() ---  ---

    # define a method (private)
    proc .set_name(arg:string) --- .name = arg ---

    # define a public method
    proc .SetName(arg:string) --- .set_name(arg) ---
----

# inheritance
type a_derived_class (name:string) is a_class(name, 0)
------------------------------------------------------
    proc .GetName() string --- return .name ---
------------------------------------------------------

# extending the builtin node, the ext_node could be working in circumstance the builtin node
# could fit.
type ext_node is node
---------------------
    proc .DoSomethingSpecial() --- #* something special could happen *# ---
---------------------

# speaking a language, e.g. template.
speak template
-----
.see a_integer ;
    none of 0, 1, 2, 3
....: 0 ;
    value is 0
....: 1 ;
    value is 1
....: 2 ;
    value is 2
....: 3 ;
    value is 3
.end
-----

# calling a function, the function must had been defined before calling it
a_function(1, "test");

# creating instances of a type
decl an_instance = a_class("A", 10);
an_instance.SetName("Alpha");

# checking 'instance-of'
see an_instance is a_class
---
    say("an_instance is a_class")
---

decl an_empty_instance a_class
an_empty_instance = a_class("B", 11);
an_empty_instance.Weight = 100;
an_empty_instance.SetName("Beta");

decl a_node ext_node

# checking 'instance-of'
see a_node is node
---
    # a_node is node
---
see a_node is ext_node
---
    # a_node is ext_node
---


# speaking many languages, the outputs are pipelined
speak template > bash
-----
.see a_integer ;
    echo "none of 0, 1, 2, 3"
....: 0 ;
    echo "value is 0"
....: 1 ;
    echo "value is 1"
....: 2 ;
    echo "value is 2"
....: 3 ;
    echo "value is 3"
.end
-----

# speaking external files
decl text = speak text in "a.txt";  # string
decl root = speak xml  in "a.xml";  # node
decl data = speak json in "a.json"; # node

# conditional
see a_integer == 0
----
    # the integer is zero
----
    # the integer is not zero
----

# switch many cases
see a_integer
----> 0:
    # the value is 0
----> 1:
    # the value is 1
----> 2:
    # the value is 2
---->
    # the value is none of the above
----

# cases branching 
see true
----> a_integer == 0:
    # equal 0
----> a_integer < 0:
    # less than 0
----> a_integer > 0:
    # greater than 0
---->
    # anything else? not possible here.
----

# speak with something
with { name:"test" } speak template
--------------------------------------
Hi, my name is $(.name). How are you!
--------------------------------------

# executing a block 'with' something
with { name:"test", age:100 }
-----------------------------
    see .age == 100 --- #* the age is 100 *# ---
    see .name == "test" --- #* the name is 'test' *# ---
-----------------------------

# calling a function (which must have exactly only one argument) with something
proc doit(_)
----
    see .name == "test" --- #* the name is 'test' *# ---
----
with { name:"test", age:100 } doit

# pre-looping (as a statement)
per n in 0,1,2,3,4,5 --- #* work with 'n' *# ---
per {name:'child-1'}, {name:'child-2'}, {name:'child-3'} doit

# post-looping (as an expression, returning a list, could be used to filter a list)
decl list_a = doit per {name:'child-1'}, {name:'child-2'}, {name:'child-3'}
decl list_b = 0 < _ per -3, -2, -1, 0, 1, 2, 3
decl list_c = 0 < n per n in -3, -2, -1, 0, 1, 2, 3

#->name;
#->name.foo();
#->name.name.foo();

#.name;
#.name.foo();
#.name->name.foo();
