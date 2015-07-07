decl node1 = { name:123 };

# comment

proc main() int
---
  say("blah...");

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

  speak foo
  -----
blah, blah, blah...
  -----
  
---
