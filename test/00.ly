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
language FooLang :spec(ABNF) :start("postal-address")
------------------------------------------------
postal-address   = name-part street zip-part

name-part        = *(personal-part SP) last-name [SP suffix] CRLF
name-part        =/ personal-part CRLF

personal-part    = first-name / (initial ".")
first-name       = *ALPHA
initial          = ALPHA
last-name        = *ALPHA
suffix           = ("Jr." / "Sr." / 1*("I" / "V" / "X"))

street           = [apt SP] house-num SP street-name CRLF
apt              = 1*4DIGIT
house-num        = 1*8(DIGIT / ALPHA)
street-name      = 1*VCHAR

zip-part         = town-name "," SP state 1*2SP zip-code CRLF
town-name        = 1*(ALPHA / SP)
state            = 2ALPHA
zip-code         = 5DIGIT ["-" 4DIGIT]
------------------------------------------------

# EBNF: https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_Form
language BarLang :spec(EBNF)
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

semantics FooSema :lang(FooLang)
--------------------------------

--------------------------------
