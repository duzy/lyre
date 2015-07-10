## decl
decl node1 = { name:123 };

# comment

proc foo()
----
----

proc main() int
---
  say('blah...');
  say("    blah blah    $(  foo()  )    $$    ...    ");

  see 1
  ---
    say("okay");
  ---

  see 0
  ----
    say("xxx");
  --->
    say("okay");
  ----

  see 0
  ---> 0:
    say("000");
  ---> 1:
    say("111");
  ---> 2:
    say("222");
  ---> 3:
    say("333");
  ----

  with { name:"foobar" }
  ----
    say("This is $(.name)...");
  ----

  with {} foo;
  with { name:"foobar" } foo;

  with { name:"foobar" } speak template
  ----
blah, blah, blah...
  ----

  speak foo
  -----
blah, blah, blah...
  -----
  
---

# ABNF: https://tools.ietf.org/html/rfc5234
# EBNF: https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_Form
language foolang with ABNF
------------------------------------------------
digit excluding zero = "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
digit                = "0" | digit excluding zero ;

twelve                          = "1", "2" ;
two hundred one                 = "2", "0", "1" ;
three hundred twelve            = "3", twelve ;
twelve thousand two hundred one = twelve, two hundred one ;

natural number = digit excluding zero, { digit } ;
integer        = "0" | [ "-" ], natural number ;
------------------------------------------------
