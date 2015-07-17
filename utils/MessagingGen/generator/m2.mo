var $defined_events = hash();
var $orthogonal_table = hash();
var $orthogonal_counter = hash();

def is_user_defined_event($_) {
    defined(.name) && .name ne 'enter' && .name ne 'exit'
}

def event_has_custom_reaction($_) {
    var $result = 0;
    if defined(.action) {
        $result = .action eq 'custom';
    } else {
        for ->action {
            if .name eq 'custom' {
                $result = 1;
            }
        }
    }
    $result
}

def mark_event_defined_once($_) {
    var $name = .name;
    var $node = .^;
    while (defined($node)) {
        if ($node.name() eq 'component') {
            $name = $node.name ~ '.' ~ $name;
        }
        $node = $node.^;
    }

    unless defined($defined_events{$name}) {
        $defined_events{$name} = 1;
        return 1;
    }
    0
}

def increase_counter($_, $name) {
    var $v = .get($name);
    $v = defined($v) ? $v + 1 : 1 ;

    .set($name, $v);

    $v
}

class orthogonal_info
{
    var $.name = %_<name>;
    var $.index = %_<index>;
    var @.states = list();

    method name() { $.name }
    method index() { $.index }
    method states() { @.states }

    method state($s) {
        for @.states {
            if .name eq $s.name {
                return $_;
            }
        }
        if $.name eq $s.orth {
            @.states.push($s);
            return $s;
        }
        null
    }

    method state_number($s) {
        var $num = 0;
        var $r = -1;
        for @.states {
            if .name eq $s.name {
                # return $num;
                $r = $num;
            } else {
                $num = $num + 1;
            }
        }
        $r
    }
}

def orthogonal($_) {
    unless defined(.orth) && defined(.^) {
        return null;
    }

    var $name = .^.name;
    var $node = .^.^;
    while (defined($node)) {
        if ($node.name() eq 'component' || $node.name() eq 'state') {
            $name = $node.name ~ '.' ~ $name;
        }
        $node = $node.^;
    }

    var $unid = $name ~ ':' ~ .orth;
    var $info = $orthogonal_table{$unid};
    unless defined($info) {
        var $index = 0;
        if defined($orthogonal_counter{$name}) {
            $index = +$orthogonal_counter{$name};
        }

        $info = new(orthogonal_info, :index($index), :name(.orth));
        $orthogonal_table{$unid} = $info;
        $orthogonal_counter{$name} = $index + 1;
    }

    # say("orth: $name:" ~ .orth~', '~$info.index());

    $info.state($_);
    $info;
}

def state_inner_initial_list($state) {
    var $list = list();
    for $state->state {
        var $orth = orthogonal($_);
        if defined($orth) {
            if ($orth.state_number($_) == 0) {
                $list.push($_);
            }
        } else {
            $list.push($_);
        }
    }
    $list
}

def state_tag($_) {
    if      .name() eq 'state' {
        "$(.^.^.name).$(.^.name).$(.name)"
    } elsif .name() eq 'event' {
        "$(.^.^.^.name).$(.^.^.name).$(.^.name)"
    } elsif .name() eq 'action' {
        "$(.^.^.^.^.name).$(.^.^.^.name).$(.^.^.name)"
    } else {
        "?"
    }
}

def collect_user_defined_state_events($_, $events)
{
    for ->event {
        var $event = $events{.name};
        unless defined($event) {
            $events{.name} = $event = new($_);
            $event.set('.', .name());
            $event.set('name', .name);
        }
        for ->field {
            var $field = new($_);
            $field.set('.', .name());
            $field.set('name', .name);
            $field.set('type', .type);
            $event.insert($field, null);
        }
    }
    for ->state {
        collect_user_defined_state_events($_, $events);
    }
}

def collect_user_defined_events($_)
{
    var $events = hash();
    for ->state {
        collect_user_defined_state_events($_, $events);
    }

    var $list = list();
    for $events {
        # TODO: This line is SEGFAULT:
        # $list.push($events{$_});

        var $v = $events{$_};
        $v.name(); # Parrot GC buggy
        $v.name;   # Parrot GC buggy
        $list.push($v);
    }
    $list
}

def collect_state_names($_, $names)
{
    if .name() eq 'state' {
        $names.push(.name);
    }
    for ->state {
        collect_state_names($_, $names);
    }
    $names
}

def field_type($_) 
{
    if .type eq 'string'
        'std::string';
    elsif .type eq 'strings'
        'std::list<std::string>';
    elsif .type eq 'uint8'
        'uint8_t';
    elsif .type eq 'uint16'
        'uint16_t';
    elsif .type eq 'uint32'
        'uint32_t';
    elsif .type eq 'int8'
        'int8_t';
    elsif .type eq 'int16'
        'int16_t';
    elsif .type eq 'int32'
        'int32_t';
    elsif .type eq 'message'
        'messaging::user_defined_message*';
    else
        'auto';
    end
}

template cxx_message_field
--------------------------
    $(field_type($_)) $(.name);
-----------------------end

template cxx_message_field_test_value
--------------------------
.if    defined(.type) && .type eq 'string' ;
"$(.^.name).string.$(.name)"
.elsif defined(.type) && .type eq 'strings' ;
{"$(.^.name).string.$(.name).1", "$(.^.name).string.$(.name).2", "$(.^.name).string.$(.name).3"}
.elsif defined(.type) && .type eq 'uint8' ;
256
.elsif defined(.type) && .type eq 'uint16' ;
65535
.elsif defined(.type) && .type eq 'uint32' ;
123456789
.elsif defined(.type) && .type eq 'int8' ;
-123
.elsif defined(.type) && .type eq 'int16' ;
-12345
.elsif defined(.type) && .type eq 'int32' ;
-123456789
.else ;
???
.end
-----------------------end

template cxx_protocol
---------------------
namespace messages
{
.if +->message < 256 ;
    typedef uint8_t tag_value_base;
.else ;
    typedef uint16_t tag_value_base;
.end ;

    enum class tag : tag_value_base
    {
.for ->message ;
        $(.name) = $(.id), // $(strip(.text()))
.end ;
    };

    constexpr std::size_t tag_size = sizeof(messages::tag);

.for ->message ;
    struct $(.name) : messaging::user_defined_message
    {
        constexpr static tag TAG = tag::$(.name);
.   for ->field ;
        $(strip(str cxx_message_field)) // $(strip(.text()))
.   end ;
    };

.end ;

    struct handler
    {
        virtual ~handler() = default;
.for ->message ;
        virtual void handle_message(const $(.name) & ) {}
.end ;
    };

    template < class P, class accessor >
    struct codec
    {
        typedef messages::tag_value_base        tag_value_base;
        typedef messages::tag                   tag;

.for ->message ;
        static std::size_t size (P *p, const $(.name) & m)
        {
.  var $fields = +->field ;
.  if $fields == 0 ;
            return 0;
.  elsif $fields == 1 ;
            return accessor::field_size (p, m.$(->field[0].name));
.  else ;
            return accessor::field_size (p, m.$(->field[0].name))
.    for slice(->field, 1, $fields-2) ;
               +   accessor::field_size (p, m.$(.name))
.    end ;
               +   accessor::field_size (p, m.$(->field[$fields-1].name));
.  end ;
        }
        static void encode (P *p, const $(.name) & m)
        {
.  for ->field ;
            accessor::put (p, m.$(.name));
.  end ;
        }
        static void decode (P *p, $(.name) & m)
        {
.  for ->field ;
            accessor::get (p, m.$(.name));
.  end ;
        }

.end ;

        template < class Message, class Context >
        static void parse (P *p, Context *ctx)
        {
            Message m;
            decode (p, m);
            p->process_message (ctx, m);
        }

        template < class Context >
        static bool parse (P *p, Context *ctx, tag t)
        {
            switch (t) {
.for ->message ;
            case tag::$(.name): parse<$(.name)>(p, ctx); return true;
.end
            default: return false;
            }
        }
    };
}//namespace messages

struct protocol : messaging::base_protocol < protocol, messages::codec >
{
    explicit protocol (int type) : base_protocol (type) {}

    template < class Context, class Message >
    void process_message (Context *ctx, Message & m)
    {
        ctx->process_message (m);
    }

.for ->message ;
    template < class Context >
    void process_message (Context *, messages::$(.name) & m)
    {
    }

.end
};
------------------end

template cxx_statechart_state_events
------------------------------------
.for many is_user_defined_event ->event ;
.    if mark_event_defined_once($_) ;
    struct event_$(.name) : sc::event<event_$(.name)> {};
.    end
.end
.for ->state
    $(str cxx_statechart_state_events)
.end
---------------------------------end

template cxx_statechart_state_event_action_code_receivemessage
--------------------------------------------------------------
            do {
                auto proto = context< machine >().protocol();
                if (!proto->receive_and_process_message (this)) {
                    auto const e = proto->error();
                    // post_event ( event_error(e) );
                    DBG ("$(state_tag($_)): ["<<e<<"] "<<proto->strerror(e));
                }
            } while (0);
-----------------------------------------------------------end

template cxx_statechart_state_event_action_code_sendmessage
-----------------------------------------------------------
            do {
                auto & mach = context< machine >();
                std::unique_ptr<messaging::user_defined_message> msg(mach.dequeue());
                
            } while (0);
--------------------------------------------------------end

template cxx_statechart_state_event_action_code_post
----------------------------------------------------
            post_event ( event_$(.event)() );
-------------------------------------------------end

template cxx_statechart_state_event_action_code_queue
-----------------------------------------------------
            if ( event.$(.field) ) {
                auto & m = context< machine >();
                m.queue( event.$(.field) );
            }
--------------------------------------------------end

template cxx_statechart_state_event_action_code_transit
-------------------------------------------------------
            return transit < state_$(.state) > ();
----------------------------------------------------end

template cxx_statechart_state_event_action_code_terminate
---------------------------------------------------------
            return terminate ();
------------------------------------------------------end

template cxx_statechart_state_event_action_code
-----------------------------------------------
.if defined(.action)
.   if    .action eq 'receive_message' ;
            $(str cxx_statechart_state_event_action_code_receivemessage)
.   elsif .action eq 'send_message' ;
            $(str cxx_statechart_state_event_action_code_sendmessage)
.   elsif .action eq 'post' ;
            $(str cxx_statechart_state_event_action_code_post)
.   elsif .action eq 'queue' ;
            $(str cxx_statechart_state_event_action_code_queue)
.   elsif .action eq 'transit'
.       if increase_counter($_, '-counter-action-transit') == 1
            $(str cxx_statechart_state_event_action_code_transit)
.       end
.   elsif .action eq 'terminate'
.       if increase_counter($_, '-counter-action-terminate') == 1
            $(str cxx_statechart_state_event_action_code_terminate)
.       end
.   elsif .action eq 'custom' ;
            $(.text())
.   else ;
            #error "no action code for '$(.action)'"
.   end
.elsif +->action
.   for ->action
.       if    .name eq 'receive_message' ;
            $(str cxx_statechart_state_event_action_code_receivemessage)
.       elsif .name eq 'send_message' ;
            $(str cxx_statechart_state_event_action_code_sendmessage)
.       elsif .name eq 'transit'
.           if increase_counter(.^, '-counter-action-transit') == 1
            $(str cxx_statechart_state_event_action_code_transit)
.           end
.       elsif .name eq 'terminate'
.           if increase_counter($_, '-counter-action-terminate') == 1
            $(str cxx_statechart_state_event_action_code_terminate)
.           end
.       elsif .name eq 'post' ;
            $(str cxx_statechart_state_event_action_code_post)
.       elsif .name eq 'queue' ;
            $(str cxx_statechart_state_event_action_code_queue)
.       elsif .name eq 'custom' ;
            $(.text())
.       else ;
            #error "no action code for '$(.name)'"
.       end
.   end
.end
--------------------------------------------end

template cxx_statechart_state_declaration
-----------------------------------------
.;
    struct state_$(.name);
.for ->state
    $(str cxx_statechart_state_declaration)
.end
--------------------------------------end

template cxx_statechart_state_definition
----------------------------------------
.var $orth = orthogonal($_);
.if defined($orth) ;
.   if  $orth.state_number($_) == 0 ;
    template < class Derived > using state_$(.^.name)_$(.orth) = sc::state < Derived, state_$(.^.name)::orthogonal<$($orth.index())> >;
.   end ;

    struct state_$(.name) : state_$(.^.name)_$(.orth) < state_$(.name) >
    {
.
.   var $entr_events = many { defined(.name) && .name eq 'enter' } ->event ;
.   var $exit_events = many { defined(.name) && .name eq 'exit' } ->event ;
.   var $events = many { defined(.name) && .name ne 'enter' && .name ne 'exit' } ->event ;
.   var $numevs = +$events;
.   if  $numevs == 1 ;
        typedef sc::custom_reaction< event_$($events[ 0 ].name) > reactions;

.   elsif 1 < $numevs ;
        typedef meta::list
        <
.       for slice($events, 0, $numevs-1) ;
            sc::custom_reaction< event_$(.name) >,
.       end ;
            sc::custom_reaction< event_$($events[ $numevs-1 ].name) >
        > reactions;

.   end ;
.
        state_$(.name)( my_context ctx ) : my_base(ctx)
        {
            DBG ("$(state_tag($_)) (enter)");
.   for $entr_events ;
            $(strip(str cxx_statechart_state_event_action_code))
.   end
        }
.
.   if +$exit_events ;

        ~state_$(.name)()
        {
            DBG ("$(state_tag($_)) (exit)");
.       for $exit_events ;
            $(strip(str cxx_statechart_state_event_action_code))
.       end
        }
.   end
.
.   for $events ;
.       if !defined(.action) && +->action == 0 && strip(.text()) eq '';

        sc::result react( const event_$(.name) & event );
.
.       else ;

        sc::result react( const event_$(.name) & event )
        {
            DBG ("$(state_tag($_)) ($(.name))");
            $(strip(str cxx_statechart_state_event_action_code))
.           if !defined(.get('-counter-action-transit')) ;
            return discard_event (); // forward_event ();
.           end ;
        }
.
.       end
.   end ;
    };
.
.else ;
.
    struct state_$(.name) : sc::simple_state
    <
        state_$(.name), machine,
.   var $inner_initial_list = state_inner_initial_list($_);
.   if +$inner_initial_list == 0 ;
        meta::list <>,
.   else
        meta::list < $(join(', ', map { 'state_'~.name } $inner_initial_list)) >,
.   end
        sc::has_no_history
    >
    {
        state_$(.name)()
        {
            DBG ("$(state_tag($_)) (enter)");
        }
    };
.end

.for ->state
    $(str cxx_statechart_state_definition)
.end
-------------------------------------end

template cxx_statechart_init_state_
----------------------------------
meta::list
        <
.var $states = +->state ;
.if $states
.    for slice(->state, 0, $states-1) ;
            state_$(.name),
.    end ;
            state_$(->state[ $states-1 ].name)
.else ;
            #error "initial states required"
.end ;
        >
-------------------------------end
template cxx_statechart_init_state
----------------------------------
state_$(->state[0].name)
-------------------------------end

template cxx_statechart
-----------------------
.for collect_user_defined_events($_) ;
.   if +->field ;
    struct event_$(.name) : sc::event<event_$(.name)>
    {
.       for ->field ;
        $(field_type($_)) $(.name);
.       end ;

        event_$(.name)
        (
.       var $num_fields = +->field;
.       var $last = ->field[ $num_fields-1 ];
.       for slice(->field, 0, $num_fields-1) ;
            $(field_type($_)) a_$(.name) ,
.       end ;
            $(field_type($last)) a_$($last.name)
        ) :
.       for slice(->field, 0, $num_fields-1) ;
            $(.name)(a_$(.name)),
.       end ;
            $($last.name)(a_$($last.name))
        {
        }
    };
.   else ;
    struct event_$(.name) : sc::event<event_$(.name)> {};
.   end
.end

.for collect_state_names($_, list()) ;
    struct state_$($_);
.end ;

    struct machine : messaging::base_machine
    <
        machine, $(strip(str cxx_statechart_init_state)), protocol
    >
    {
        template < class Context > machine( Context ctx ) : base_machine(ctx, ZMQ_$(.type))
        {
.if +->bind || +->connect;
            auto pred = [](char c) -> bool { return c == ',' || c == ';'; };
.end
.if +->bind ;
            std::vector<std::string> bind_targets;
.   for ->bind ;
            boost::algorithm::split (bind_targets, "$(.targets)", pred);
.   end ;
            this->bind(bind_targets.begin(), bind_targets.end());
.end ;
.if +->connect ;
            std::vector<std::string> connect_targets;
.   for ->connect ;
            boost::algorithm::split (connect_targets, "$(.targets)", pred);
.   end ;
            this->connect(connect_targets.begin(), connect_targets.end());
.end ;
        }
    };

    struct app : messaging::base_app < app, machine >, private messages::handler
    {
    };

.for ->state
    $(str cxx_statechart_state_definition)
.end
--------------------end

template cxx_component
----------------------
namespace $(.name)
{
.if +->message
    $(str cxx_protocol)

.end
.if +->state
    $(str cxx_statechart)

.end
.for ->component
    $(str cxx_component)

.end
}//namespace $(.name)
-------------------end

template cxx_component_call_test
--------------------------------
$(.name)::test (threads);
.for ->component ;
    $(str cxx_component_call_test)
.end
-----------------------------end

template cxx_component_test
---------------------------
namespace $(.name)
{
.for ->component
    $(str cxx_component_test)

.end
.if +->message ;
void test_protocol_run_q ()
{
    constexpr std::size_t ts = messages::tag_size;
    DBG ("Q: $(.name) ..");
    protocol proto(ZMQ_REQ);
    proto.connect ({"inproc://test-$(.name)"});
.   for ->test ;
    {
        messages::$(.q) q;
.    var $q = .q ;
.    var $p = .p ;
.       for .^->message->{ .name eq $q }->field ;
        q.$(.name) = $(strip(str cxx_message_field_test_value));
.       end ;
        assert (proto.send_message (q) == ts + protocol::codec::size(&proto, q));

        messages::$(.p) p;
        assert (proto.recv_message (p) == ts + protocol::codec::size(&proto, p));
.       for .^->message->{ .name eq $p }->field ;
        assert (p.$(.name) == $(strip(str cxx_message_field_test_value)));
.       end ;
    }
.   end
    DBG ("Q: $(.name) (OK)");
}
void test_protocol_run_p ()
{
    DBG ("P: $(.name) ..");
    constexpr std::size_t ts = messages::tag_size;
    protocol proto(ZMQ_REP);
    proto.bind ({"inproc://test-$(.name)"});
.   for ->test ;
    {
        messages::$(.q) q;
        assert (proto.recv_message (q) == ts + protocol::codec::size(&proto, q));
.       var $q = .q ;
.       var $p = .p ;
.       for .^->message->{ .name eq $q }->field ;
        assert (q.$(.name) == $(strip(str cxx_message_field_test_value)));
.       end ;

        messages::$(.p) p;
.       for .^->message->{ .name eq $p }->field ;
        p.$(.name) = $(strip(str cxx_message_field_test_value));
.       end ;
        assert (proto.send_message (p) == ts + protocol::codec::size(&proto, p));
    }
.   end
    DBG ("P: $(.name) (OK)");
}
void test_protocol (std::vector<std::thread> & threads)
{
    threads.push_back(std::thread(test_protocol_run_q));
    threads.push_back(std::thread(test_protocol_run_p));
}
.end
.if +->state
struct test_app : app
{
.   for .^->message ;
    virtual void handle_message(const messages::$(.name) & m)
    {
        DBG ("handle_message: $(.name)");
    }
.   end ;
};

void test_statechart_run ()
{
    test_app a;

    std::thread t1([&a](){
        std::this_thread::sleep_for(std::chrono::seconds (30));
        a.quit ();
        DBG ("quit $(.name).");
    });

    std::thread t2([&a](){
        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
.   for ->test ;
.       if defined(.event) ;
        // $(.message) -> $(.result)
        {
            auto msg = new messages::$(.message)();
            a.post< event_$(.event) >( msg );
        }
.       end
.   end ;
    });

    // Detach threads to avoid "terminate called whithout an active exception"!
    t1.detach ();
    t2.detach ();

    a.run ();

    DBG ("$(.name) stopped.");
}
void test_statechart (std::vector<std::thread> & threads)
{
    threads.push_back (std::thread(test_statechart_run));
}
.end
void test (std::vector<std::thread> & threads)
{
.if +->bind || +->connect ;
    auto pred = [](char c) -> bool { return c == ',' || c == ';'; };
.end
.if +->bind ;
    std::vector<std::string> bind_targets;
.   for ->bind ;
    boost::algorithm::split (bind_targets, "$(.targets)", pred);
.   end ;
    DBG ("$(.name): binds: "<<boost::algorithm::join(bind_targets, ", "));
.end
.if +->connect ;
    std::vector<std::string> connect_targets;
.   for ->connect ;
    boost::algorithm::split (connect_targets, "$(.targets)", pred);
.   end ;
    DBG ("$(.name): connects: "<<boost::algorithm::join(connect_targets, ", "));
.end
.for ->component ;
    $(str cxx_component_call_test)
.end
.if +->message ;
    test_protocol (threads);
.end
.if +->state ;
    test_statechart (threads);
.end
}
}//namespace $(.name)
------------------------end

template cxx_code
---------------------
/** -*- c++ -*-
 *  The $(.name). This file is 100% generated, don't change it unless
 *  you really understand what you're doing.
 */
$(str cxx_component)

.if isreg("gen/$(.name)_customs.inc") ;
#include "gen/$(.name)_customs.inc"
.end
------------------end

template cxx_test
---------------------
/**
 *  The $(.name). This file is 100% generated, don't change it unless
 *  you really understand what you're doing.
 */
#include "messaging.inc"
#include "$(.name).inc"
#include <boost/algorithm/string/join.hpp>

/**
 *  Auto test -- $(.name)::test().
 */
$(str cxx_component_test)

int main()
{
    std::vector<std::thread> threads;
    for (int n = 0; n < 1; ++n) {
        DBG ("test: round #"<<n<<"..");
        $(.name)::test (threads);
    }
    for (auto & t : threads) t.join();
    return EXIT_SUCCESS;
}
------------------end

def write_file($filename, $content) {
    var $file = open($filename, 'w');
    $file.print($content);
    $file.close();
}

write_file("gen/$(.name).inc", str cxx_code)
write_file("gen/$(.name)_test.cpp", str cxx_test)
