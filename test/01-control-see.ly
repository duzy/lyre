see true
--- ## bare block
    say("1: okay");
---

see true
--- ## bare block
    say("2: okay");
---> ## fork block
    say("2: xxx");
---

see false
---> ## fork block
    say("2: xxx");
---> ## fork block
    say("2: okay");
---

see true
--->> 1 == 0: ## conditional block
     say("3: xxx");
--->> 1 == 1: ## conditional block
     say("3: okay");
--->> 1 == 2: ## conditional block
     say("3: xxx");
--->> ## fork block
     say("3: xxx");
---

decl a = 1;

see a
---> 0: ## conditional block
     say("4: xxx");
---> 1: ## conditional block
     say("4: okay");
---> 2: ## conditional block
     say("4: xxx");
----

a = 2;
see a
---> 0: ## conditional block
     say("5: xxx");
---> 1: ## conditional block
     say("5: xxx");
---> 2: ## conditional block
     say("5: okay");
---> 2: ## conditional block
     say("5: xxx");
----
