see true
---
    say("1: okay");
---

see false
--->
    say("2: xxx");
--->
    say("2: okay");
---

see true
--->> 1 == 0:
     say("3: xxx");
--->> 1 == 1:
     say("3: okay");
--->> 1 == 2:
     say("3: xxx");
--->>
     say("3: xxx");
---

decl a = 1;

see a
---> 0:
     say("4: xxx");
---> 1:
     say("4: okay");
---> 2:
     say("4: xxx");
----

a = 2;
see a
---> 0:
     say("5: xxx");
---> 1:
     say("5: xxx");
---> 2:
     say("5: okay");
---> 2:
     say("5: xxx");
----
