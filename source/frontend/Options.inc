/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Option Parsing Definitions                                                 *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

/////////
// Prefixes

#ifdef PREFIX
#define COMMA ,
PREFIX(prefix_0, {0})
PREFIX(prefix_2, {"-" COMMA 0})
PREFIX(prefix_1, {"-" COMMA "--" COMMA 0})
#undef COMMA
#endif

/////////
// Groups

#ifdef OPTION

//////////
// Options

OPTION(prefix_0, "<input>", INPUT, Input, INVALID, INVALID, 0, 0, 0, 0, 0)
OPTION(prefix_0, "<unknown>", UNKNOWN, Unknown, INVALID, INVALID, 0, 0, 0, 0, 0)
OPTION(prefix_1, "help", help, Flag, INVALID, INVALID, 0, NoDriverOption, 0,
       "Display available options", 0)
OPTION(prefix_2, "o", o, JoinedOrSeparate, INVALID, INVALID, 0, RenderAsInput | DriverOption, 0,
       "Write output to <file>", "<file>")
#endif