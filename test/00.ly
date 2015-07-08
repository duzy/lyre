decl node1 = { name:123 };

# comment

proc foo()
----
----

proc main() int
---
  say('blah...');
  say(" blah blah $(foo()) $$ ... ");

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
