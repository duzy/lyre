CXXFLAGS := -g -ggdb -std=c++11 -I.
LOADLIBES := 
LDLIBS := -lzmq -lboost_thread -lboost_system -lpthread

MOROOT := $(dir $(PWD))mo/gen/parrot
MO := parrot -I$(MOROOT) -L$(MOROOT) $(MOROOT)/mo.pbc

GENERATOR := m2

all : prototype/example prototype/test gen/example_test

prototype/test : prototype/test.o
prototype/test.o : *.inc

prototype/example : prototype/example.o
prototype/example prototype/test gen/example_test :
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

gen/example_test : gen/example_test.o
gen/example_test.o: gen/example_test.cpp gen/example.inc
gen/example_test.cpp: gen/example.inc
gen/example.inc : example.xml generator/$(GENERATOR).mo gen
	$(MO) example.xml generator/$(GENERATOR).mo

gen : ; @mkdir -p $@
test : gen/example_test ; $<
test-gen : generator/$(GENERATOR).mo example.xml gen
	@ulimit 10000000000
	$(MO) example.xml $<
